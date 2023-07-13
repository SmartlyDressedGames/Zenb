// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbUserWidget.h"
#include "ZenbAxisHandleWidget.generated.h"

UCLASS(Meta = (DisableNativeTick))
class UZenbAxisHandleWidget : public UZenbUserWidget
{
	GENERATED_BODY()

public: // Functions

	void SetIsHidden(bool bInHidden);

public: // Properties

	UPROPERTY(EditAnywhere)
	FVector LocalDirection;

	UPROPERTY(Meta = (BindWidget))
	class UButton* Button;

	UPROPERTY(Transient, Meta = (BindWidgetAnim))
	class UWidgetAnimation* HideAnim;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

private: // Functions

	UFUNCTION()
	void OnButtonClicked();

private: // Properties

	UPROPERTY(Transient)
	uint8 bIsHidden:1;
};
