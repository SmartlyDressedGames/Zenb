// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "ZenbTool.generated.h"

UCLASS(Blueprintable)
class ZENB_API UZenbTool : public UObject
{
	GENERATED_BODY()

public: // Functions

	virtual void OnEquip() {}
	virtual void OnDequip() {}
	virtual void Tick();
	virtual void OnLeftMousePressed() {}
	virtual void OnLeftMouseReleased() {}
	virtual void OnRotateClockwisePressed() {}
	virtual void OnRotateCounterClockwisePressed() {}
	virtual void OnSampleColorPressed() {}
	virtual void OnSampleColorReleased() {}
	virtual bool ShouldHoverActorUnderMouse() const { return true; }
	virtual bool ShouldHoverActor(class AZenbBrickActor* InActor) const { return true; }
	virtual void BeginDraggingHandle(FVector InLocalVector) {}
	virtual void EndDraggingHandle() {}
	virtual bool ShouldHandlesBeVisible() const { return false; }
	virtual bool ShouldClearSelectionWhenEquipped() const { return false; }

public: // Properties

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> WidgetClass;

	UPROPERTY(EditDefaultsOnly)
	FText DisplayNameText;
	
	/** temporary */
	UPROPERTY(EditDefaultsOnly)
	FText HotkeyText;

	UPROPERTY()
	TObjectPtr<class AZenbPawn> Pawn;
};
