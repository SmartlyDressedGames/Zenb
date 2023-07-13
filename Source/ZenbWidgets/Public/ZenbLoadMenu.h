// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbActivatableWidget.h"
#include "ZenbLoadMenu.generated.h"

UCLASS(Abstract)
class UZenbLoadMenu : public UZenbActivatableWidget
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UCommonActivatableWidget Interface
	TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End UCommonActivatableWidget Interface

protected: // Functions

	UFUNCTION(BlueprintImplementableEvent)
	void AddLoadWidget(UWidget* InWidget);
	
	UFUNCTION(BlueprintImplementableEvent)
	void ClearLoadWidgets();

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UCommonActivatableWidget Interface
	void NativeOnActivated() override;
	UWidget* NativeGetDesiredFocusTarget() const override;
	//~ End UCommonActivatableWidget Interface

private: // Functions

	void OnBackClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UCommonButtonBase* BackButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UZenbKnownSaveButton> KnownSaveButtonClass;
};
