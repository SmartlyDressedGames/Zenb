// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbActivatableWidget.h"
#include "ZenbPaintToolWidget.generated.h"

UCLASS(Meta = (DisableNativeTick))
class UZenbPaintToolWidget : public UZenbActivatableWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	void NativeConstruct() override;
	void NativeDestruct() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnColorSampled();

	UFUNCTION()
	void OnSwatchClicked(FLinearColor InColor);

	UFUNCTION()
	void OnRedChanged(float InValue);
	
	UFUNCTION()
	void OnGreenChanged(float InValue);

	UFUNCTION()
	void OnBlueChanged(float InValue);

private: // Properties

	UPROPERTY(EditDefaultsOnly)
	FLinearColor SelectedColor;

	UPROPERTY(EditDefaultsOnly)
	TArray<FLinearColor> SwatchColors;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UZenbColorSwatchWidget> SwatchClass;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UPanelWidget> SwatchPanel;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UImage> ColorImage;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class USpinBox> RedSpinBox; 
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class USpinBox> GreenSpinBox; 
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class USpinBox> BlueSpinBox;
};
