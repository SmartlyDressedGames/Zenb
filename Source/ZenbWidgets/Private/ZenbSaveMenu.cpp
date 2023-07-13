// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbSaveMenu.h"
#include "CommonButtonBase.h"
#include "ZenbGameModeBase.h"
#include "Components/EditableTextBox.h"
#include "ZenbUserSettings.h"

// Public Interfaces:

//~ Begin UCommonActivatableWidget Interface
TOptional<FUIInputConfig> UZenbSaveMenu::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, false);
}
//~ End UCommonActivatableWidget Interface

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbSaveMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked().AddUObject(this, &UZenbSaveMenu::OnBackClicked);
	}

	if (SaveButton)
	{
		SaveButton->OnClicked().AddUObject(this, &UZenbSaveMenu::OnSaveClicked);
	}
}
//~ End UUserWidget Interface

//~ Begin UCommonActivatableWidget Interface
UWidget* UZenbSaveMenu::NativeGetDesiredFocusTarget() const
{
	return BackButton;
}
//~ End UCommonActivatableWidget Interface

// Private Functions:

void UZenbSaveMenu::OnBackClicked()
{
	DeactivateWidget();
}

void UZenbSaveMenu::OnSaveClicked()
{
	UZenbUserSettings* Settings = UZenbUserSettings::GetZenbUserSettings();
	FGuid Save = Settings->AddKnownSave(NameTextBox->GetText());
	GetWorld()->GetAuthGameMode<AZenbGameModeBase>()->RequestSave(Save);
}
