// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbResizeTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"

#if ENABLE_DRAW_DEBUG
static bool bDrawResizeTool = false;
static FAutoConsoleVariableRef CVarDrawResizeTool(TEXT("z.DrawResizeTool"), bDrawResizeTool, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

// Public Interfaces:

//~ Begin UZenbTool Interface
void UZenbResizeTool::Tick()
{
	Super::Tick();

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	AZenbBrickActor* ActiveActor = Pawn->GetActiveActor();
	if (!ActiveActor)
		return;

	if (bIsDraggingHandle)
	{
#if ENABLE_DRAW_DEBUG
		if (bDrawResizeTool)
			DrawDebugDirectionalArrow(GetWorld(), DragOrigin, DragOrigin + HandleWorldAxis * 100.f, 10.f, FColor::Red, false, -1.f, SDPG_Foreground, 1.f);
#endif // ENABLE_DRAW_DEBUG

		float MouseDistanceAlongAxis = Pawn->ProjectRayOntoRay(MouseRayLocation, MouseRayDirection, DragOrigin, HandleWorldAxis);
		float RelativeDistance = MouseDistanceAlongAxis - InitialMouseDistanceAlongAxis;

		FVector AbsLocalVector = HandleLocalAxis.GetAbs() * RelativeDistance;
		FIntVector DimensionsDelta = FIntVector(FMath::GridSnap(AbsLocalVector.X, 20.f), FMath::GridSnap(AbsLocalVector.Y, 20.f), FMath::GridSnap(AbsLocalVector.Z, 8.f));
		FIntVector NewDimensions = InitialDimensions + DimensionsDelta;
		float SnapInterval = FMath::Abs(HandleLocalAxis.Z) > 0.5f ? 8.f : 20.f;
		FVector NewLocation = DragOrigin + HandleWorldAxis * FMath::GridSnap(RelativeDistance, SnapInterval) * 0.5f;

#if ENABLE_DRAW_DEBUG
		if (bDrawResizeTool)
		{
			DrawDebugBox(GetWorld(), DragOrigin, FVector(InitialDimensions) * 0.5f, InitialRotation, FColor::Red, false, -1., SDPG_Foreground, 0.f);
			DrawDebugBox(GetWorld(), NewLocation, FVector(NewDimensions) * 0.5f, InitialRotation, FColor::Blue, false, -1., SDPG_Foreground, 0.f);
		}
#endif // ENABLE_DRAW_DEBUG

		if (TObjectPtr<class UStaticMesh>* NewMesh = ActiveActor->Meshes.Find(NewDimensions))
		{
			ActiveActor->SetActorLocation(NewLocation);
			ActiveActor->SetActorRotation(InitialRotation);

			ActiveActor->Dimensions = NewDimensions;
			ActiveActor->GetStaticMeshComponent()->SetStaticMesh(*NewMesh);
		}
		else
		{
			// Maybe we can rotate and resize?
			Swap(NewDimensions.X, NewDimensions.Y);

			if (TObjectPtr<class UStaticMesh>* NewMesh2 = ActiveActor->Meshes.Find(NewDimensions))
			{
				ActiveActor->SetActorLocation(NewLocation);

				ActiveActor->SetActorRotation(InitialRotation * FRotator(0.f, 90.f, 0.f).Quaternion());
				ActiveActor->Dimensions = NewDimensions;
				ActiveActor->GetStaticMeshComponent()->SetStaticMesh(*NewMesh2);
			}
		}
	}
}

void UZenbResizeTool::OnLeftMouseReleased()
{
	Super::OnLeftMouseReleased();

	EndDraggingHandle();
}

void UZenbResizeTool::BeginDraggingHandle(FVector InLocalVector)
{
	AZenbBrickActor* ActiveActor = Pawn->GetActiveActor();
	if (ActiveActor == nullptr)
		return;

	bIsDraggingHandle = true;
	HandleLocalAxis = InLocalVector;
	HandleWorldAxis = ActiveActor->GetActorTransform().TransformVector(InLocalVector);
	DragOrigin = ActiveActor->GetActorLocation();

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;
	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	InitialDimensions = ActiveActor->Dimensions;
	InitialMouseDistanceAlongAxis = Pawn->ProjectRayOntoRay(MouseRayLocation, MouseRayDirection, DragOrigin, HandleWorldAxis);
	PreviousDragDistanceAlongAxis = 0.f;
	InitialRotation = ActiveActor->GetActorQuat();

	BeginTransformTransaction();
	for (AZenbBrickActor* Actor : Pawn->GetSelection())
	{
		TrackInitialTransform(Actor);
	}
}

void UZenbResizeTool::EndDraggingHandle()
{
	if (bIsDraggingHandle)
	{
		EndTransformTransaction();
	}

	bIsDraggingHandle = false;
}
//~ End UZenbTool Interface
