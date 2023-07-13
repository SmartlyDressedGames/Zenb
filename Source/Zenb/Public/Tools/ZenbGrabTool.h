// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Tools/ZenbTool.h"
#include "Transactions/ZenbTransformTransaction.h"
#include "ZenbGrabTool.generated.h"

UCLASS()
class ZENB_API UZenbGrabTool : public UZenbTool
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTool Interface
	void Tick() override;
	void OnLeftMousePressed() override;
	void OnLeftMouseReleased() override;
	void OnRotateClockwisePressed() override;
	void OnRotateCounterClockwisePressed() override;
	bool ShouldHoverActorUnderMouse() const override { return !bIsDragDropping && !bIsConsideringDragDrop; }
	bool ShouldHoverActor(class AZenbBrickActor* InActor) const override;
	//~ End UZenbTool Interface

protected: // Functions

	void BeginTransformTransaction();
	void TrackInitialTransform(class AZenbBrickActor* InActor);
	void EndTransformTransaction();

protected: // Properties

	/** Actor under the cursor when left mouse button was pressed. */
	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> MouseDownActor;

	/** Cursor position when the left mouse button was clicked. */
	UPROPERTY()
	FVector2D MouseDownCursorPosition;

	/** If true, left mouse was pressed over a drag-droppable actor but has not moved from MouseDownCursorPosition yet. */
	UPROPERTY()
	uint8 bIsConsideringDragDrop:1;

	/** Whether actors are currently being drag-dropped in 3D following the cursor. */
	UPROPERTY()
	uint8 bIsDragDropping:1;

	UPROPERTY()
	FVector ActiveNearestLocalBottomStud;
	
	UPROPERTY()
	FVector HitRelativeToBottomStud;

private: // Functions

	void RotateGrabbed(float InYawDelta);

private: // Properties

	UPROPERTY(Transient)
	TArray<FZenbTransformDelta> PendingTransformChanges;
};
