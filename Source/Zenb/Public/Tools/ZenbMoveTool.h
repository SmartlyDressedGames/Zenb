// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbGrabTool.h"
#include "ZenbMoveTool.generated.h"

UCLASS()
class ZENB_API UZenbMoveTool : public UZenbGrabTool
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTool Interface
	void Tick() override;
	void OnLeftMouseReleased() override;
	bool ShouldHoverActorUnderMouse() const override { return !bIsDragDropping && !bIsConsideringDragDrop && !bIsDraggingHandle; }
	void BeginDraggingHandle(FVector InLocalVector) override;
	void EndDraggingHandle() override;
	bool ShouldHandlesBeVisible() const override { return !bIsDragDropping && !bIsConsideringDragDrop && !bIsDraggingHandle; }
	//~ End UZenbTool Interface

private: // Properties

	/** Whether a 2D handle button is currently held. */
	UPROPERTY()
	uint8 bIsDraggingHandle:1;

	UPROPERTY()
	FVector HandleWorldAxis;

	UPROPERTY()
	FVector HandleLocalAxis;

	UPROPERTY()
	FVector DragOrigin;

	UPROPERTY()
	FIntVector InitialDimensions;

	UPROPERTY()
	FQuat InitialRotation;

	UPROPERTY()
	float InitialMouseDistanceAlongAxis;

	UPROPERTY()
	float PreviousDragDistanceAlongAxis;
};
