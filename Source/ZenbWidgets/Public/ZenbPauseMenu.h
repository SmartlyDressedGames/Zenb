// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbActivatableWidget.h"
#include "ZenbPauseMenu.generated.h"

UCLASS(Abstract)
class UZenbPauseMenu : public UZenbActivatableWidget
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UCommonActivatableWidget Interface
	TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End UCommonActivatableWidget Interface

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	//~ Begin UCommonActivatableWidget Interface
	UWidget* NativeGetDesiredFocusTarget() const override;
	//~ End UCommonActivatableWidget Interface

private: // Functions

	void OnResumeClicked();
	void OnNewMapClicked();
	void OnSaveClicked();
	void OnQuitClicked();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UCommonButtonBase* ResumeButton;

	UPROPERTY(Meta = (BindWidget))
	class UCommonButtonBase* NewMapButton;

	UPROPERTY(Meta = (BindWidget))
	class UCommonButtonBase* SaveButton;

	UPROPERTY(Meta = (BindWidget))
	class UCommonButtonBase* QuitButton;
};
