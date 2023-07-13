// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbLoadMenu.h"
#include "CommonButtonBase.h"
#include "Components/EditableTextBox.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "ZenbUserSettings.h"
#include "ZenbKnownSaveButton.h"

// Public Interfaces:

//~ Begin UCommonActivatableWidget Interface
TOptional<FUIInputConfig> UZenbLoadMenu::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, false);
}
//~ End UCommonActivatableWidget Interface

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbLoadMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked().AddUObject(this, &UZenbLoadMenu::OnBackClicked);
	}
}
//~ End UUserWidget Interface

//~ Begin UCommonActivatableWidget Interface
void UZenbLoadMenu::NativeOnActivated()
{
	Super::NativeOnActivated();

	ClearLoadWidgets();
	for (const FZenbKnownSave& Item : UZenbUserSettings::GetZenbUserSettings()->GetKnownSaves())
	{
		UZenbKnownSaveButton* Button = CreateWidget<UZenbKnownSaveButton>(this, KnownSaveButtonClass);
		Button->SetKnownSave(Item);
		AddLoadWidget(Button);
	}
}

UWidget* UZenbLoadMenu::NativeGetDesiredFocusTarget() const
{
	return BackButton;
}
//~ End UCommonActivatableWidget Interface

// Private Functions:

void UZenbLoadMenu::OnBackClicked()
{
	DeactivateWidget();
}
