// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbPawn.h"
#include "ZenbLogChannels.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Transactions/ZenbTransaction.h"
#include "Tools/ZenbPaintTool.h"
#include "Tools/ZenbGrabTool.h"
#include "Tools/ZenbMoveTool.h"
#include "Tools/ZenbResizeTool.h"
#include "Transactions/ZenbSpawnTransaction.h"
#include "Transactions/ZenbDestroyTransaction.h"
#include "Transactions/ZenbChangeToolTransaction.h"
#include "Tools/ZenbSelectTool.h"
#include "Tools/ZenbExplosionTool.h"
#include "Transactions/ZenbSelectionTransaction.h"
#include "Tools/ZenbBoxSelectTool.h"
#include "ZenbPawnMovement.h"
#include "ZenbGameModeBase.h"
#include "ZenbTransactionManager.h"
#include "ZenbPlayerController.h"

#define LOCTEXT_NAMESPACE "Zenb"

#if ENABLE_DRAW_DEBUG
static bool bDrawDuplicateBounds = false;
static FAutoConsoleVariableRef CVarDrawDragDuplicateBounds(TEXT("z.DrawDuplicateBounds"), bDrawDuplicateBounds, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

AZenbPawn::AZenbPawn(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UZenbPawnMovement>(ADefaultPawn::MovementComponentName))
{
	bAddDefaultMovementBindings = false;
	MoveSpeedFactor = 1.4f;
}

// Public Functions:

AZenbPlayerController* AZenbPawn::GetZenbPlayerController()
{
	return GetController<AZenbPlayerController>();
}

UZenbTool* AZenbPawn::FindToolByClass(TSubclassOf<class UZenbTool> InClass)
{
	for (UZenbTool* Tool : ToolInstances)
	{
		if (Tool->IsA(InClass))
		{
			return Tool;
		}
	}

	return nullptr;
}

void AZenbPawn::SetActiveTool(UZenbTool* InTool)
{
	if (ActiveTool == InTool)
		return;

	if (ActiveTool)
	{
		ActiveTool->OnDequip();
	}

	ActiveTool = InTool;

	if (ActiveTool)
	{
		ActiveTool->OnEquip();
	}

	OnActiveToolChanged.Broadcast();
}

void AZenbPawn::RequestChangeTool(UZenbTool* InTool)
{
	if (!ensure(InTool))
		return;

	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot change tool while using current tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotChangeToolWhileUsing", "Cannot change tool while using current tool"));
		return;
	}

	UZenbChangeToolTransaction* Transaction = GetTransactionManager()->NewTransaction<UZenbChangeToolTransaction>();
	Transaction->OldTool = GetActiveTool();
	Transaction->NewTool = InTool;

	// Paint tool does not support selection.
	if (InTool->ShouldClearSelectionWhenEquipped())
	{
		Transaction->RemovedActors = SelectedActors;
		Transaction->OldActiveActor = GetActiveActor();
		ClearSelection();
	}

	SetActiveTool(InTool);

	GetTransactionManager()->AddTransaction(Transaction);
}

void AZenbPawn::SetActiveActor(class AZenbBrickActor* InActor)
{
	if (PrivateActiveActor == InActor)
		return;

	if (PrivateActiveActor)
	{
		PrivateActiveActor->SetIsActive(false);
	}

	PrivateActiveActor = InActor;

	if (PrivateActiveActor)
	{
		PrivateActiveActor->SetIsActive(true);
		check(!PrivateActiveActor->IsHidden());
		check(PrivateActiveActor->CanBeSelected());
	}
}

bool AZenbPawn::DoesSelectionContain(AZenbBrickActor* InActor) const
{
	return SelectedActors.Contains(InActor);
}

void AZenbPawn::ToggleSelectActor(AZenbBrickActor* InActor)
{
	check(InActor);

	int32 NumRemoved = SelectedActors.Remove(InActor);
	if (NumRemoved > 0)
	{
		InActor->SetIsSelected(false);

		if (GetActiveActor() == InActor)
		{
			SetActiveActor(nullptr);
		}
	}
	else
	{
		check(!InActor->IsHidden());
		check(InActor->CanBeSelected());
		SelectedActors.Add(InActor);
		InActor->SetIsSelected(true);
		SetActiveActor(InActor);
	}
}

void AZenbPawn::SelectActor(AZenbBrickActor* InActor)
{
	check(InActor);
	check(!InActor->IsHidden());
	check(InActor->CanBeSelected());

	SelectedActors.Add(InActor);
	InActor->SetIsSelected(true);
	
	SetActiveActor(InActor);
}

void AZenbPawn::DeselectActor(AZenbBrickActor* InActor)
{
	check(InActor);

	SelectedActors.Remove(InActor);
	InActor->SetIsSelected(false);
	
	if (GetActiveActor() == InActor)
	{
		SetActiveActor(nullptr);
	}
}

void AZenbPawn::ClearSelection()
{
	SetActiveActor(nullptr);
	for (AZenbBrickActor* Actor : SelectedActors)
	{
		if (Actor)
		{
			Actor->SetIsSelected(false);
		}
	}
	SelectedActors.Reset();
}

void AZenbPawn::RequestClearSelection()
{
	if (SelectedActors.Num() > 0)
	{
		UZenbSelectionTransaction* Transaction = GetTransactionManager()->NewTransaction<UZenbSelectionTransaction>();
		Transaction->OldActiveActor = GetActiveActor();
		Transaction->RemovedActors = GetSelection();
		ClearSelection();
		GetTransactionManager()->AddTransaction(Transaction);
	}
}

void AZenbPawn::SetWorldDirtyForAutosave()
{
	GetWorld()->GetAuthGameMode<AZenbGameModeBase>()->SetDirtyForAutosave();
}

void AZenbPawn::SetHoveredActor(AZenbBrickActor* NewHoveredActor)
{
	if (HoveredActor != NewHoveredActor)
	{
		if (HoveredActor)
		{
			HoveredActor->SetIsHovered(false);
		}
		HoveredActor = NewHoveredActor;
		if (HoveredActor)
		{
			HoveredActor->SetIsHovered(true);
		}
	}
}

float AZenbPawn::ProjectRayOntoRay(FVector Origin0, FVector Direction0, FVector Origin1, FVector Direction1)
{
	FVector n = FVector::CrossProduct(Direction1, Direction0);
	FVector n2 = FVector::CrossProduct(Direction0, n);
	return FVector::DotProduct(Origin0 - Origin1, n2) / FVector::DotProduct(Direction1, n2);
}

// Public Interfaces:

//~ Begin AActor Interface
void AZenbPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ActiveTool)
		return;
	
	ActiveTool->Tick();
}

void AZenbPawn::BecomeViewTarget(APlayerController* PC)
{
	Super::BecomeViewTarget(PC);

	for (TSubclassOf<class UZenbTool> ToolClass : ToolClasses)
	{
		UZenbTool* ToolInstance = NewObject<UZenbTool>(this, ToolClass);
		if (ToolInstance)
		{
			ToolInstance->Pawn = this;
			ToolInstances.Add(ToolInstance);
		}
	}

	TransactionManager = NewObject<UZenbTransactionManager>(this);
}
//~ End AActor Interface

// Protected Interfaces:

//~ Begin APawn Interface
void AZenbPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AZenbPawn::OnMoveForwardInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &AZenbPawn::OnMoveRightInput);
	PlayerInputComponent->BindAxis("MouseYaw", this, &AZenbPawn::OnYawInput);
	PlayerInputComponent->BindAxis("MousePitch", this, &AZenbPawn::OnPitchInput);
	PlayerInputComponent->BindAxis("MoveSpeed", this, &AZenbPawn::OnMoveSpeedInput);

	PlayerInputComponent->BindKey(EKeys::Left, IE_Pressed, this, &AZenbPawn::OnMoveLeftPressed);
	PlayerInputComponent->BindKey(EKeys::Right, IE_Pressed, this, &AZenbPawn::OnMoveRightPressed);
	PlayerInputComponent->BindKey(EKeys::Up, IE_Pressed, this, &AZenbPawn::OnMoveUpPressed);
	PlayerInputComponent->BindKey(EKeys::Down, IE_Pressed, this, &AZenbPawn::OnMoveDownPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Left, EModifierKey::Shift), IE_Pressed, this, &AZenbPawn::OnGrowLeftPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Right, EModifierKey::Shift), IE_Pressed, this, &AZenbPawn::OnGrowRightPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Up, EModifierKey::Shift), IE_Pressed, this, &AZenbPawn::OnGrowUpPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Down, EModifierKey::Shift), IE_Pressed, this, &AZenbPawn::OnGrowDownPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Left, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnShrinkLeftPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Right, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnShrinkRightPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Up, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnShrinkUpPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Down, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnShrinkDownPressed);

	PlayerInputComponent->BindKey(FInputChord(EKeys::C, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnCopyPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::V, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnPastePressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::D, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnDuplicatePressed);
	PlayerInputComponent->BindKey(EKeys::R, IE_Pressed, this, &AZenbPawn::OnRotatePressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::R, EModifierKey::Shift), IE_Pressed, this, &AZenbPawn::OnRotateCounterclockwisePressed);
	PlayerInputComponent->BindKey(EKeys::Delete, IE_Pressed, this, &AZenbPawn::OnDeletePressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::A, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnSelectAllPressed);
	PlayerInputComponent->BindKey(EKeys::Y, IE_Pressed, this, &AZenbPawn::OnExplodePressed);

	PlayerInputComponent->BindKey(EKeys::Q, IE_Pressed, this, &AZenbPawn::OnSelectPressed);
	PlayerInputComponent->BindKey(EKeys::B, IE_Pressed, this, &AZenbPawn::OnBoxSelectPressed);
	PlayerInputComponent->BindKey(EKeys::W, IE_Pressed, this, &AZenbPawn::OnMovePressed);
	PlayerInputComponent->BindKey(EKeys::E, IE_Pressed, this, &AZenbPawn::OnResizePressed);
	PlayerInputComponent->BindKey(EKeys::T, IE_Pressed, this, &AZenbPawn::OnPaintPressed);
	PlayerInputComponent->BindKey(EKeys::LeftAlt, IE_Pressed, this, &AZenbPawn::OnSampleColorPressed);
	PlayerInputComponent->BindKey(EKeys::LeftAlt, IE_Released, this, &AZenbPawn::OnSampleColorReleased);

	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AZenbPawn::OnLeftMousePressed);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AZenbPawn::OnLeftMouseReleased);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AZenbPawn::OnRightMousePressed);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Released, this, &AZenbPawn::OnRightMouseReleased);

	PlayerInputComponent->BindKey(FInputChord(EKeys::S, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnSavePressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Z, EModifierKey::Control), IE_Pressed, this, &AZenbPawn::OnUndoPressed);
	PlayerInputComponent->BindKey(FInputChord(EKeys::Z, EModifierKey::Control | EModifierKey::Shift), IE_Pressed, this, &AZenbPawn::OnRedoPressed);
}
//~ End APawn Interface

// Private Functions:

FVector AZenbPawn::GetBestAlignedLocalVector(FVector InControlVector)
{
	AZenbBrickActor* ActiveActor = GetActiveActor();
	if (!ActiveActor)
	{
		return FVector::UpVector;
	}

	const FVector WorldVector = GetControlRotation().RotateVector(InControlVector);
	const FVector ActiveForward = ActiveActor->GetActorForwardVector();
	const FVector ActiveRight = ActiveActor->GetActorRightVector();
	const FVector ActiveUp = ActiveActor->GetActorUpVector();

	float DotForward = FVector::DotProduct(WorldVector, ActiveForward);
	float DotRight = FVector::DotProduct(WorldVector, ActiveRight);
	float DotUp = FVector::DotProduct(WorldVector, ActiveUp);

	if (FMath::Abs(DotForward) > FMath::Abs(DotRight))
	{
		if (FMath::Abs(DotForward) > FMath::Abs(DotUp))
		{
			return FVector::ForwardVector * FMath::Sign(DotForward);
		}
		else
		{
			return FVector::UpVector * FMath::Sign(DotUp);
		}
	}
	else
	{
		if (FMath::Abs(DotRight) > FMath::Abs(DotUp))
		{
			return FVector::RightVector * FMath::Sign(DotRight);
		}
		else
		{
			return FVector::UpVector * FMath::Sign(DotUp);
		}
	}
}

void AZenbPawn::MoveAlongLocalVector(FVector InLocalVector)
{
	AZenbBrickActor* ActiveActor = GetActiveActor();
	if (!ActiveActor)
	{
		return;
	}

	ActiveActor->AddActorLocalOffset(InLocalVector * FVector(20.0f, 20.0f, 24.0f));
}

void AZenbPawn::GrowAlongLocalVector(FVector InLocalVector)
{
	AZenbBrickActor* ActiveActor = GetActiveActor();
	if (!ActiveActor)
	{
		return;
	}

	FVector AbsLocalVector = InLocalVector.GetAbs();
	FIntVector DimensionsDelta = FIntVector(FMath::RoundToInt(AbsLocalVector.X * 20.f), FMath::RoundToInt(AbsLocalVector.Y * 20.f), FMath::RoundToInt(AbsLocalVector.Z * 24.f));
	FIntVector NewDimensions = ActiveActor->Dimensions + DimensionsDelta;

	if (TObjectPtr<class UStaticMesh>* NewMesh = ActiveActor->Meshes.Find(NewDimensions))
	{
		ActiveActor->AddActorLocalOffset(InLocalVector * FVector(10.0f, 10.0f, 12.0f));
		ActiveActor->Dimensions = NewDimensions;
		ActiveActor->GetStaticMeshComponent()->SetStaticMesh(*NewMesh);
	}
	else if (ActiveActor->Dimensions.X == ActiveActor->Dimensions.Y)
	{
		// Part is currently rotationally symmetrical, so maybe we can rotate and resize?
		Swap(NewDimensions.X, NewDimensions.Y);

		if (TObjectPtr<class UStaticMesh>* NewMesh2 = ActiveActor->Meshes.Find(NewDimensions))
		{
			ActiveActor->AddActorLocalOffset(InLocalVector * FVector(10.0f, 10.0f, 12.0f));
			ActiveActor->AddActorLocalRotation(FRotator(0.0f, 90.0f, 0.0f));
			ActiveActor->Dimensions = NewDimensions;
			ActiveActor->GetStaticMeshComponent()->SetStaticMesh(*NewMesh2);
		}
	}
}

void AZenbPawn::ShrinkAlongLocalVector(FVector InLocalVector)
{
	AZenbBrickActor* ActiveActor = GetActiveActor();
	if (!ActiveActor)
	{
		return;
	}

	FVector AbsLocalVector = InLocalVector.GetAbs();
	FIntVector DimensionsDelta = FIntVector(FMath::RoundToInt(AbsLocalVector.X * 20.f), FMath::RoundToInt(AbsLocalVector.Y * 20.f), FMath::RoundToInt(AbsLocalVector.Z * 24.f));
	FIntVector NewDimensions = ActiveActor->Dimensions - DimensionsDelta;

	if (TObjectPtr<class UStaticMesh>* NewMesh = ActiveActor->Meshes.Find(NewDimensions))
	{
		ActiveActor->AddActorLocalOffset(InLocalVector * FVector(-10.0f, -10.0f, -12.0f));
		ActiveActor->Dimensions = NewDimensions;
		ActiveActor->GetStaticMeshComponent()->SetStaticMesh(*NewMesh);
	}
	else if (ActiveActor->Dimensions.X == ActiveActor->Dimensions.Y)
	{
		// Part is currently rotationally symmetrical, so maybe we can rotate and resize?
		Swap(NewDimensions.X, NewDimensions.Y);

		if (TObjectPtr<class UStaticMesh>* NewMesh2 = ActiveActor->Meshes.Find(NewDimensions))
		{
			ActiveActor->AddActorLocalOffset(InLocalVector * FVector(-10.0f, -10.0f, -12.0f));
			ActiveActor->AddActorLocalRotation(FRotator(0.0f, 90.0f, 0.0f));
			ActiveActor->Dimensions = NewDimensions;
			ActiveActor->GetStaticMeshComponent()->SetStaticMesh(*NewMesh2);
		}
	}
}

void AZenbPawn::OnMoveForwardInput(float InAxisValue)
{
	if (bIsFlying)
	{
		MoveForward(InAxisValue);
	}
}

void AZenbPawn::OnMoveRightInput(float InAxisValue)
{
	if (bIsFlying)
	{
		MoveRight(InAxisValue);
	}
}

void AZenbPawn::OnMoveSpeedInput(float InAxisValue)
{
	if (bIsFlying)
	{
		MoveSpeedFactor = FMath::Max(1.f, MoveSpeedFactor + InAxisValue * 0.05f);
		if (UFloatingPawnMovement* Movement = Cast<UFloatingPawnMovement>(GetMovementComponent()))
		{
			Movement->MaxSpeed = FMath::Pow(100.0f, MoveSpeedFactor);
		}
	}
}

void AZenbPawn::OnPitchInput(float InAxisValue)
{
	if (bIsFlying)
	{
		AddControllerPitchInput(InAxisValue);
	}
}

void AZenbPawn::OnYawInput(float InAxisValue)
{
	if (bIsFlying)
	{
		AddControllerYawInput(InAxisValue);
	}
}

void AZenbPawn::OnMoveLeftPressed()
{
	MoveAlongLocalVector(GetBestAlignedLocalVector(FVector::LeftVector));
}

void AZenbPawn::OnMoveRightPressed()
{
	MoveAlongLocalVector(GetBestAlignedLocalVector(FVector::RightVector));
}

void AZenbPawn::OnMoveUpPressed()
{
	MoveAlongLocalVector(GetBestAlignedLocalVector(FVector::UpVector));
}

void AZenbPawn::OnMoveDownPressed()
{
	MoveAlongLocalVector(GetBestAlignedLocalVector(FVector::DownVector));
}

void AZenbPawn::OnGrowLeftPressed()
{
	GrowAlongLocalVector(GetBestAlignedLocalVector(FVector::LeftVector));
}

void AZenbPawn::OnGrowRightPressed()
{
	GrowAlongLocalVector(GetBestAlignedLocalVector(FVector::RightVector));
}

void AZenbPawn::OnGrowUpPressed()
{
	GrowAlongLocalVector(GetBestAlignedLocalVector(FVector::UpVector));
}

void AZenbPawn::OnGrowDownPressed()
{
	GrowAlongLocalVector(GetBestAlignedLocalVector(FVector::DownVector));
}

void AZenbPawn::OnShrinkLeftPressed()
{
	ShrinkAlongLocalVector(GetBestAlignedLocalVector(FVector::LeftVector));
}

void AZenbPawn::OnShrinkRightPressed()
{
	ShrinkAlongLocalVector(GetBestAlignedLocalVector(FVector::RightVector));
}

void AZenbPawn::OnShrinkUpPressed()
{
	ShrinkAlongLocalVector(GetBestAlignedLocalVector(FVector::UpVector));
}

void AZenbPawn::OnShrinkDownPressed()
{
	ShrinkAlongLocalVector(GetBestAlignedLocalVector(FVector::DownVector));
}

void AZenbPawn::OnSelectPressed()
{
	if (!bIsFlying)
	{
		RequestChangeTool(FindToolByClass(UZenbSelectTool::StaticClass()));
	}
}

void AZenbPawn::OnBoxSelectPressed()
{
	if (!bIsFlying)
	{
		RequestChangeTool(FindToolByClass(UZenbBoxSelectTool::StaticClass()));
	}
}

void AZenbPawn::OnMovePressed() 
{
	if (!bIsFlying)
	{
		RequestChangeTool(FindToolByClass(UZenbMoveTool::StaticClass()));
	}
}

void AZenbPawn::OnResizePressed()
{
	if (!bIsFlying)
	{
		RequestChangeTool(FindToolByClass(UZenbResizeTool::StaticClass()));
	}
}

void AZenbPawn::OnPaintPressed()
{
	if (!bIsFlying)
	{
		RequestChangeTool(FindToolByClass(UZenbPaintTool::StaticClass()));
	}
}

void AZenbPawn::OnSampleColorPressed()
{
	if (ActiveTool)
	{
		ActiveTool->OnSampleColorPressed();
	}
}

void AZenbPawn::OnSampleColorReleased()
{
	if (ActiveTool)
	{
		ActiveTool->OnSampleColorReleased();
	}
}

void AZenbPawn::OnCopyPressed()
{
	CopyDetails.Actors.Reset();

	FBox TotalBounds;
	bool bHasAnyBounds = false;
	for (AZenbBrickActor* Actor : SelectedActors)
	{
		if (bHasAnyBounds)
		{
			TotalBounds += Actor->GetComponentsBoundingBox(true, false);
		}
		else
		{
			TotalBounds = Actor->GetComponentsBoundingBox(true, false);
			bHasAnyBounds = true;
		}

		FZenbActorCopyInfo& CopyInfo = CopyDetails.Actors.AddDefaulted_GetRef();
		CopyInfo.ActorClass = Actor->GetClass();
		CopyInfo.Location = Actor->GetActorLocation();
		CopyInfo.Rotation = Actor->GetActorQuat();
		CopyInfo.Dimensions = Actor->Dimensions;
		CopyInfo.Color = Actor->GetColor();
	}

	TotalBounds.GetCenterAndExtents(CopyDetails.BoundsOrigin, CopyDetails.BoundsExtent);
}

void AZenbPawn::OnPastePressed()
{
	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot paste while using tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotPasteWhileUsingTool", "Cannot paste while using tool"));
		return;
	}

	if (CopyDetails.Actors.Num() < 1)
	{
		return;
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawDuplicateBounds)
	{
		DrawDebugBox(GetWorld(), CopyDetails.BoundsOrigin, CopyDetails.BoundsExtent, FQuat::Identity, FColor::Blue, true, 5.f, SDPG_World, 1.f);
	}
#endif // ENABLE_DRAW_DEBUG

	float UpwardDistance = 10000.0f;
	FVector SweepStart = CopyDetails.BoundsOrigin + FVector(0.f, 0.f, UpwardDistance);
	FVector SweepEnd = CopyDetails.BoundsOrigin;

	FCollisionShape SweepShape = FCollisionShape::MakeBox(CopyDetails.BoundsExtent);

	FHitResult WorldHit;
	bool bHitAnything = GetWorld()->SweepSingleByChannel(WorldHit, SweepStart, SweepEnd, FQuat::Identity, ECC_Visibility, SweepShape);
	if (!bHitAnything)
	{
		UE_LOG(LogZenb, Warning, TEXT("Nothing to place part on top of"));
		return;
	}

	float PlacementDistanceAboveOrigin = UpwardDistance - WorldHit.Distance;

#if ENABLE_DRAW_DEBUG
	if (bDrawDuplicateBounds)
	{
		DrawDebugBox(GetWorld(), CopyDetails.BoundsOrigin + FVector(0.f, 0.f, PlacementDistanceAboveOrigin), CopyDetails.BoundsExtent, FQuat::Identity, FColor::Red, true, 5.f, SDPG_World, 1.f);
	}
#endif // ENABLE_DRAW_DEBUG

	UZenbSpawnTransaction* Transaction = GetTransactionManager()->NewTransaction<UZenbSpawnTransaction>();

	Transaction->OldActiveActor = GetActiveActor();
	for (AZenbBrickActor* Actor : SelectedActors)
	{
		Transaction->SourceActors.Add(Actor);
	}
	ClearSelection();

	for (const FZenbActorCopyInfo& SourceInfo : CopyDetails.Actors)
	{
		FVector NewActorLocation = SourceInfo.Location + FVector(0.0f, 0.0f, PlacementDistanceAboveOrigin);
		FTransform NewActorTransform(SourceInfo.Rotation, NewActorLocation);

		FActorSpawnParameters SpawnParams = FActorSpawnParameters();
		SpawnParams.ObjectFlags |= RF_Transient;
		AZenbBrickActor* NewActor = GetWorld()->SpawnActor<AZenbBrickActor>(SourceInfo.ActorClass, SpawnParams);
		NewActor->SetActorTransform(NewActorTransform);
		NewActor->Dimensions = SourceInfo.Dimensions;
		NewActor->UpdateMeshFromDimensions();
		NewActor->SetColor(SourceInfo.Color);

		Transaction->Actors.Add(NewActor);
		SelectActor(NewActor);
	}

	if (AZenbBrickActor* HitPart = Cast<AZenbBrickActor>(WorldHit.GetActor()))
	{
		FVector HitStud = HitPart->GetNearestTopStudWorldSpace(WorldHit.Location);
		FVector NearestBottomStud;
		float NearestDistance = 10000.0f;
		bool bHasBottomStud = false;

		for (AZenbBrickActor* NewActor : Transaction->Actors)
		{
			FVector TestStud = NewActor->GetNearestBottomStudWorldSpace(HitStud);
			float TestDistance = (TestStud - HitStud).SizeSquared();
			if (TestDistance < NearestDistance)
			{
				NearestBottomStud = TestStud;
				NearestDistance = TestDistance;
				bHasBottomStud = true;
			}
		}

		if (bHasBottomStud)
		{
			FVector DeltaLocation = HitStud - NearestBottomStud;
			for (AZenbBrickActor* NewActor : Transaction->Actors)
			{
				NewActor->AddActorWorldOffset(DeltaLocation);
			}
		}
	}

	Transaction->NewActiveActor = GetActiveActor();
	GetTransactionManager()->AddTransaction(Transaction);
	SetWorldDirtyForAutosave();
}

void AZenbPawn::OnDuplicatePressed()
{
	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot duplicate while using tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotDuplicateWhileUsingTool", "Cannot duplicate while using tool"));
		return;
	}

	if (SelectedActors.IsEmpty())
	{
		return;
	}

	OnCopyPressed();
	OnPastePressed();
}

void AZenbPawn::OnRotatePressed()
{
	if (ActiveTool)
	{
		ActiveTool->OnRotateClockwisePressed();
	}
}

void AZenbPawn::OnRotateCounterclockwisePressed()
{
	if (ActiveTool)
	{
		ActiveTool->OnRotateCounterClockwisePressed();
	}
}

void AZenbPawn::OnDeletePressed()
{
	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot delete while using tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotDeleteWhileUsingTool", "Cannot delete while using tool"));
		return;
	}

	if (SelectedActors.Num() < 1)
		return;

	UZenbDestroyTransaction* Transaction = GetTransactionManager()->NewTransaction<UZenbDestroyTransaction>();
	TSet<TObjectPtr<AZenbBrickActor>> CopyOfActors = SelectedActors;
	Transaction->OldActiveActor = GetActiveActor();
	SelectedActors.Reset();
	SetActiveActor(nullptr);
	for (AZenbBrickActor* Actor : CopyOfActors)
	{
		Transaction->AddActor(Actor);
	}
	GetTransactionManager()->AddTransaction(Transaction);
	SetWorldDirtyForAutosave();
}

void AZenbPawn::OnSelectAllPressed()
{
	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot select all while using tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotSelectAllWhileUsingTool", "Cannot select all while using tool"));
		return;
	}

	AZenbBrickActor* OldActiveActor = GetActiveActor();
	TSet<TObjectPtr<class AZenbBrickActor>> AddActors;

	for (auto Iter = TActorIterator<AZenbBrickActor>(GetWorld()); Iter; ++Iter)
	{
		if (Iter->IsHidden())
		{
			// Hidden by undo/redo spawn/destroy, so should not be duplicated.
			continue;
		}

		if (!Iter->CanBeSelected())
			continue;

		if (DoesSelectionContain(*Iter))
			continue;

		AddActors.Add(*Iter);
		SelectActor(*Iter);
	}

	if (AddActors.Num() < 1)
	{
		// All actors are already selected!
		return;
	}

	UZenbSelectionTransaction* Transaction = GetTransactionManager()->NewTransaction<UZenbSelectionTransaction>();
	Transaction->AddedActors = AddActors;
	Transaction->OldActiveActor;
	Transaction->NewActiveActor = GetActiveActor();
	GetTransactionManager()->AddTransaction(Transaction);
}

void AZenbPawn::OnExplodePressed()
{
	if (!bIsFlying)
	{
		RequestChangeTool(FindToolByClass(UZenbExplosionTool::StaticClass()));
	}
}

void AZenbPawn::OnLeftMousePressed()
{
	bIsLeftMouseHeld = true;
	if (ActiveTool)
	{
		ActiveTool->OnLeftMousePressed();
	}
}

void AZenbPawn::OnLeftMouseReleased()
{
	if (ActiveTool)
	{
		ActiveTool->OnLeftMouseReleased();
	}
	bIsLeftMouseHeld = false;
}

void AZenbPawn::OnRightMousePressed()
{
	bIsFlying = true;

	APlayerController* PC = GetController<APlayerController>();
	if (!PC)
		return;

	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());
}

void AZenbPawn::OnRightMouseReleased()
{
	bIsFlying = false;

	APlayerController* PC = GetController<APlayerController>();
	if (!PC)
		return;

	PC->bShowMouseCursor = true;
	PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
}

void AZenbPawn::OnSavePressed()
{
	GetWorld()->GetAuthGameMode<AZenbGameModeBase>()->RequestSave();
}

void AZenbPawn::OnUndoPressed()
{
	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot undo while using tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotUndoWhileUsingTool", "Cannot undo while using tool"));
		return;
	}

	if (ensure(TransactionManager))
	{
		TransactionManager->Undo();
	}
}

void AZenbPawn::OnRedoPressed()
{
	if (bIsLeftMouseHeld)
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot redo while using tool"));
		GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_CannotRedoWhileUsingTool", "Cannot redo while using tool"));
		return;
	}

	if (ensure(TransactionManager))
	{
		TransactionManager->Redo();
	}
}

#undef LOCTEXT_NAMESPACE
