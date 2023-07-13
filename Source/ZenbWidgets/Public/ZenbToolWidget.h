// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbUserWidget.h"
#include "ZenbToolWidget.generated.h"

UCLASS(Meta = (DisableNativeTick))
class UZenbToolWidget : public UZenbUserWidget
{
	GENERATED_BODY()

public: // Functions

	void SetTool(class UZenbTool* InTool);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	void NativeDestruct() override;
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

	UFUNCTION()
	void OnActiveToolChanged();

private: // Properties

	UPROPERTY()
	TObjectPtr<class UZenbTool> Tool;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UZenbTool> ToolClass;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* DisplayNameLabel;
	
	UPROPERTY(Meta = (BindWidget))
	class UTextBlock* HotkeyLabel;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	class UWidgetAnimation* EquipAnim;

	UPROPERTY(Transient)
	uint8 bIsActive:1;
};
