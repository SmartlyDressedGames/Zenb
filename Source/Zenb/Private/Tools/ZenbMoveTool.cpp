// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbMoveTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"

#if ENABLE_DRAW_DEBUG
static bool bDrawMoveTool = false;
static FAutoConsoleVariableRef CVarDrawMoveTool(TEXT("z.DrawMoveTool"), bDrawMoveTool, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

// Public Interfaces:

//~ Begin UZenbTool Interface
void UZenbMoveTool::Tick()
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

	if (bIsDraggingHandle)
	{
#if ENABLE_DRAW_DEBUG
		if (bDrawMoveTool)
			DrawDebugDirectionalArrow(GetWorld(), DragOrigin, DragOrigin + HandleWorldAxis * 100.f, 10.f, FColor::Red, false, -1.f, SDPG_Foreground, 1.f);
#endif // ENABLE_DRAW_DEBUG

		float MouseDistanceAlongAxis = Pawn->ProjectRayOntoRay(MouseRayLocation, MouseRayDirection, DragOrigin, HandleWorldAxis);
		float RelativeDistance = MouseDistanceAlongAxis - InitialMouseDistanceAlongAxis;

		if (ActiveActor)
		{
			float SnapInterval = FMath::Abs(HandleLocalAxis.Z) > 0.5f ? 8.f : 20.f;
			float NewDistanceAlongAxis = FMath::GridSnap(RelativeDistance, SnapInterval);
			if (PreviousDragDistanceAlongAxis != NewDistanceAlongAxis)
			{
				float DeltaAlongAxis = NewDistanceAlongAxis - PreviousDragDistanceAlongAxis;
				PreviousDragDistanceAlongAxis = NewDistanceAlongAxis;

				FVector DeltaLocation = HandleWorldAxis * DeltaAlongAxis;
				for (AActor* Actor : Pawn->GetSelection())
				{
					Actor->AddActorWorldOffset(DeltaLocation);
				}
			}
		}
	}
}

void UZenbMoveTool::OnLeftMouseReleased()
{
	Super::OnLeftMouseReleased();

	EndDraggingHandle();
}

void UZenbMoveTool::BeginDraggingHandle(FVector InLocalVector)
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

void UZenbMoveTool::EndDraggingHandle()
{
	if (bIsDraggingHandle)
	{
		EndTransformTransaction();
	}

	bIsDraggingHandle = false;
}
//~ End UZenbTool Interface
