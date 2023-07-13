// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbUserWidget.h"
#include "ZenbColorSwatchWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FColorSwatchClickedEvent, FLinearColor, InColor);

UCLASS(Meta = (DisableNativeTick))
class UZenbColorSwatchWidget : public UZenbUserWidget
{
	GENERATED_BODY()

public: // Functions

	void SetColor(FLinearColor InColor);

public: // Properties

	UPROPERTY()
	FColorSwatchClickedEvent OnClicked;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY()
	FLinearColor Color;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;
	
	UPROPERTY(Meta = (BindWidget))
	class UImage* Image;
};
