// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbPauseMenu.h"
#include "CommonButtonBase.h"
#include "HAL/PlatformMisc.h"
#include "ZenbGameModeBase.h"

// Public Interfaces:

//~ Begin UCommonActivatableWidget Interface
TOptional<FUIInputConfig> UZenbPauseMenu::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture, false);
}
//~ End UCommonActivatableWidget Interface

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbPauseMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ResumeButton)
	{
		ResumeButton->OnClicked().AddUObject(this, &UZenbPauseMenu::OnResumeClicked);
	}

	if (NewMapButton)
	{
		NewMapButton->OnClicked().AddUObject(this, &UZenbPauseMenu::OnNewMapClicked);
	}

	if (SaveButton)
	{
		SaveButton->OnClicked().AddUObject(this, &UZenbPauseMenu::OnSaveClicked);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked().AddUObject(this, &UZenbPauseMenu::OnQuitClicked);
	}
}
//~ End UUserWidget Interface

//~ Begin UCommonActivatableWidget Interface
UWidget* UZenbPauseMenu::NativeGetDesiredFocusTarget() const
{
	return ResumeButton;
}
//~ End UCommonActivatableWidget Interface

// Private Functions:

void UZenbPauseMenu::OnResumeClicked()
{
	DeactivateWidget();
}

void UZenbPauseMenu::OnNewMapClicked()
{
	GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("/Game/Default.Default?LoadSlotGuid=%s"), *FGuid().ToString()), TRAVEL_Absolute);
}

void UZenbPauseMenu::OnSaveClicked()
{
	GetWorld()->GetAuthGameMode<AZenbGameModeBase>()->RequestSave();
}

void UZenbPauseMenu::OnQuitClicked()
{
	FPlatformMisc::RequestExit(/*Force*/ false);
}
