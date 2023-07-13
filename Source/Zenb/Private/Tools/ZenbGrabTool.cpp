// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbGrabTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"
#include "Transactions/ZenbTransaction.h"
#include "Transactions/ZenbSelectionTransaction.h"
#include "ZenbTransactionManager.h"

#if ENABLE_DRAW_DEBUG
static bool bDrawDragBottomStud = false;
static FAutoConsoleVariableRef CVarDrawDragBottomStud(TEXT("z.DrawDragBottomStud"), bDrawDragBottomStud, TEXT(""));

static bool bDrawFloorGridSnap = false;
static FAutoConsoleVariableRef CVarDrawFloorGridSnap(TEXT("z.DrawFloorGridSnap"), bDrawFloorGridSnap, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

// Public Interfaces:

//~ Begin UZenbTool Interface
void UZenbGrabTool::Tick()
{
	Super::Tick();

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	if (bIsConsideringDragDrop && !bIsDragDropping)
	{
		FVector2D CurrentMousePosition;
		PC->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);
		if ((CurrentMousePosition - MouseDownCursorPosition).SizeSquared() > 4.0f)
		{
			bIsConsideringDragDrop = false;
			bIsDragDropping = true;

			BeginTransformTransaction();
			for (AZenbBrickActor* Actor : Pawn->GetSelection())
			{
				TrackInitialTransform(Actor);
			}
		}
	}

	if (!bIsDragDropping || MouseDownActor == nullptr)
		return;

	MouseRayLocation -= HitRelativeToBottomStud;

	FCollisionQueryParams QueryParams = FCollisionQueryParams::DefaultQueryParam;
	for (AZenbBrickActor* Actor : Pawn->GetSelection())
	{
		QueryParams.AddIgnoredActor(Actor);
	}

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, MouseRayLocation, MouseRayLocation + MouseRayDirection * 10000.f, ECC_Visibility, QueryParams))
	{

	}
	else
	{
		// Backup flat plane at world origin to allow placing without baseplate.
		FPlane WorldPlane(FVector::ZeroVector, FVector::UpVector);
		Hit.Location = FMath::RayPlaneIntersection(MouseRayLocation, MouseRayDirection, WorldPlane);
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawDragBottomStud)
	{
		FVector BottomStudLocation = MouseDownActor->GetActorTransform().TransformPosition(ActiveNearestLocalBottomStud);
		DrawDebugPoint(GetWorld(), BottomStudLocation, 5.f, FColor::Red, false, -1.f, SDPG_Foreground);
	}
#endif // ENABLE_DRAW_DEBUG

	FVector NewLocation;

	AZenbBrickActor* HitActor = Cast<AZenbBrickActor>(Hit.GetActor());
	if (HitActor)
	{
		FVector NearestWorldTopStud = HitActor->GetNearestTopStudWorldSpace(Hit.Location);
		FVector Offset = MouseDownActor->GetActorTransform().TransformVector(ActiveNearestLocalBottomStud);
		NewLocation = NearestWorldTopStud - Offset;
	}
	else
	{
		NewLocation = Hit.Location;
		// This does not *really* matter, but the grid is offset by 10 so that an evenly sized baseplate
		// placed at the origin has an equal number of studs on each side of the world.
		NewLocation.X = FMath::GridSnap(NewLocation.X - 10.f, 20.f) + 10.f;
		NewLocation.Y = FMath::GridSnap(NewLocation.Y - 10.f, 20.f) + 10.f;

#if ENABLE_DRAW_DEBUG
		if (bDrawFloorGridSnap)
		{
			DrawDebugCoordinateSystem(GetWorld(), FVector::ZeroVector, FRotator::ZeroRotator, 100.f, false, -1.f, SDPG_Foreground);
			DrawDebugPoint(GetWorld(), NewLocation, 5.f, FColor::Red, false, -1.f, SDPG_Foreground);
			DrawDebugString(GetWorld(), NewLocation, NewLocation.ToString(), nullptr, FColor::Red, 0.f, true);
		}
#endif // ENABLE_DRAW_DEBUG

		NewLocation -= MouseDownActor->GetActorTransform().TransformVector(ActiveNearestLocalBottomStud);
	}

	FVector OldLocation = MouseDownActor->GetActorLocation();
	if (OldLocation != NewLocation)
	{
		FVector DeltaLocation = NewLocation - OldLocation;
		for (AActor* Actor : Pawn->GetSelection())
		{
			Actor->AddActorWorldOffset(DeltaLocation);
		}
	}
}

void UZenbGrabTool::OnLeftMousePressed()
{
	MouseDownActor = nullptr;

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	FHitResult MouseDownHit;
	if (!GetWorld()->LineTraceSingleByChannel(MouseDownHit, MouseRayLocation, MouseRayLocation + MouseRayDirection * 10000.f, ECC_Visibility))
	{
		// Clearing selection when clicking on the sky is expected.
		Pawn->RequestClearSelection();
		return;
	}

	MouseDownActor = Cast<AZenbBrickActor>(MouseDownHit.GetActor());
	if (!MouseDownActor || !MouseDownActor->CanBeSelected())
	{
		// Clearing selection when clicking on baseplate is expected.
		Pawn->RequestClearSelection();
		MouseDownActor = nullptr;
		return;
	}
	
	if (PC->IsInputKeyDown(EKeys::LeftShift))
	{
		UZenbSelectionTransaction* Transaction = Pawn->GetTransactionManager()->NewTransaction<UZenbSelectionTransaction>();
		Transaction->OldActiveActor = Pawn->GetActiveActor();
		if (Pawn->DoesSelectionContain(MouseDownActor))
		{
			if (Pawn->GetActiveActor() == MouseDownActor)
			{
				Pawn->DeselectActor(MouseDownActor);
				Transaction->RemovedActors.Add(MouseDownActor);
			}	
			else
			{
				Pawn->SetActiveActor(MouseDownActor);
			}
		}
		else
		{
			Pawn->SelectActor(MouseDownActor);
			Transaction->AddedActors.Add(MouseDownActor);
		}
		Transaction->NewActiveActor = Pawn->GetActiveActor();
		Pawn->GetTransactionManager()->AddTransaction(Transaction);
	}
	else if (!Pawn->DoesSelectionContain(MouseDownActor))
	{
		UZenbSelectionTransaction* Transaction = Pawn->GetTransactionManager()->NewTransaction<UZenbSelectionTransaction>();
		Transaction->OldActiveActor = Pawn->GetActiveActor();
		Transaction->RemovedActors = Pawn->GetSelection();
		Transaction->AddedActors.Add(MouseDownActor);
		Pawn->ClearSelection();
		Pawn->SelectActor(MouseDownActor);
		Transaction->NewActiveActor = Pawn->GetActiveActor();
		Pawn->GetTransactionManager()->AddTransaction(Transaction);
	}

	PC->CurrentMouseCursor = EMouseCursor::GrabHandClosed;
	bIsConsideringDragDrop = true;
	PC->GetMousePosition(MouseDownCursorPosition.X, MouseDownCursorPosition.Y);

	ActiveNearestLocalBottomStud = MouseDownActor->GetNearestBottomStud(MouseDownHit.Location);
	FVector NearestWorldBottomStud = MouseDownActor->GetActorTransform().TransformPosition(ActiveNearestLocalBottomStud);
	HitRelativeToBottomStud = MouseDownHit.Location - NearestWorldBottomStud;
}

void UZenbGrabTool::OnLeftMouseReleased()
{
	if (bIsDragDropping)
	{
		EndTransformTransaction();
	}

	APlayerController* PC = Pawn->GetController<APlayerController>();
	bIsDragDropping = false;
	bIsConsideringDragDrop = false;
	if (PC)
	{
		PC->CurrentMouseCursor = EMouseCursor::Default;
	}
}

void UZenbGrabTool::OnRotateClockwisePressed()
{
	RotateGrabbed(90.f);
}

void UZenbGrabTool::OnRotateCounterClockwisePressed()
{
	RotateGrabbed(-90.f);
}

bool UZenbGrabTool::ShouldHoverActor(AZenbBrickActor* InActor) const
{
	return InActor->CanBeSelected();
}
//~ End UZenbTool Interface

// Protected Functions:

void UZenbGrabTool::BeginTransformTransaction()
{
	PendingTransformChanges.Reset();
}

void UZenbGrabTool::TrackInitialTransform(AZenbBrickActor* InActor)
{
	if (ensure(InActor))
	{
		for (const FZenbTransformDelta& Item : PendingTransformChanges)
		{
			if (Item.Actor == InActor)
				return;
		}

		FZenbTransformDelta& Item = PendingTransformChanges.AddDefaulted_GetRef();
		Item.Actor = InActor;
		Item.OldLocation = InActor->GetActorLocation();
		Item.OldQuat = InActor->GetActorQuat();
		Item.OldDimensions = InActor->Dimensions;
	}
}

void UZenbGrabTool::EndTransformTransaction()
{
	PendingTransformChanges.RemoveAllSwap([](FZenbTransformDelta& Delta)
	{
		Delta.NewLocation = Delta.Actor->GetActorLocation();
		Delta.NewQuat = Delta.Actor->GetActorQuat();
		Delta.NewDimensions = Delta.Actor->Dimensions;
		bool bNearlyEqual = Delta.OldLocation.Equals(Delta.NewLocation);
		bNearlyEqual &= Delta.OldQuat.Equals(Delta.NewQuat);
		bNearlyEqual &= Delta.OldDimensions == Delta.NewDimensions;
		return bNearlyEqual;
	}, /*bAllowShrinking*/ false);

	if (PendingTransformChanges.Num() > 0)
	{
		UZenbTransformTransaction* Transaction = Pawn->GetTransactionManager()->NewTransaction<UZenbTransformTransaction>();
		Transaction->ModifiedBricks = PendingTransformChanges;
		PendingTransformChanges.Reset();
		Pawn->GetTransactionManager()->AddTransaction(Transaction);
		Pawn->SetWorldDirtyForAutosave();
	}
}

// Private Functions:

void UZenbGrabTool::RotateGrabbed(float InYawDelta)
{
	if (!bIsDragDropping || MouseDownActor == nullptr)
		return;

	FVector BottomStudLocation = MouseDownActor->GetActorTransform().TransformPosition(ActiveNearestLocalBottomStud);
	FQuat OldQuat = MouseDownActor->GetActorQuat();
	FQuat NewPivotToWorld = OldQuat * FRotator(0.f, InYawDelta, 0.f).Quaternion();
	FQuat WorldToOldPivot = OldQuat.Inverse();
	for (AZenbBrickActor* Actor : Pawn->GetSelection())
	{
		FVector RelativeLocation = Actor->GetActorLocation() - BottomStudLocation;
		RelativeLocation = WorldToOldPivot.RotateVector(RelativeLocation);
		FQuat RelativeQuat = WorldToOldPivot * Actor->GetActorQuat();
		RelativeLocation = NewPivotToWorld.RotateVector(RelativeLocation);
		RelativeQuat = NewPivotToWorld * RelativeQuat;
		Actor->SetActorLocationAndRotation(RelativeLocation + BottomStudLocation, RelativeQuat);
	}
}
