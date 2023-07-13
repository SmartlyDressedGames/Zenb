// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Tools/ZenbTool.h"
#include "Transactions/ZenbPaintTransaction.h"
#include "ZenbPaintTool.generated.h"

UCLASS()
class ZENB_API UZenbPaintTool : public UZenbTool
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTool Interface
	void Tick() override;
	void OnLeftMousePressed() override;
	void OnLeftMouseReleased() override;
	void OnSampleColorPressed() override;
	void OnSampleColorReleased() override;
	bool ShouldClearSelectionWhenEquipped() const override { return true; }
	//~ End UZenbTool Interface

private: // Functions

	void TrackColorChange(class AZenbBrickActor* InActor, FLinearColor InColor);

private: // Properties

	/** Whether actors under the cursor should be changed to active color during tick. */
	UPROPERTY(Transient)
	uint8 bIsPainting:1;
	
	UPROPERTY(Transient)
	uint8 bIsSamplingColors:1;

	UPROPERTY(Transient)
	TArray<FZenbPaintDelta> PendingPaintChanges;
};
