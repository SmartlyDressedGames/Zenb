// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbKnownSaveButton.h"
#include "CommonTextBlock.h"
#include "ZenbUserSettings.h"

// Public Functions:

void UZenbKnownSaveButton::SetKnownSave(const FZenbKnownSave& InKnownSave)
{
	KnownSave = InKnownSave;

	if (ensure(CustomNameTextBlock))
	{
		CustomNameTextBlock->SetText(KnownSave.CustomName);
	}

	if (ensure(SavedTimestampTextBlock))
	{
		SavedTimestampTextBlock->SetText(FText::AsDateTime(KnownSave.SavedTimestampUtc));
	}
}

// Protected Interfaces:

//~ Begin UCommonButtonBase Interface
void UZenbKnownSaveButton::NativeOnClicked()
{
	Super::NativeOnClicked();

	UZenbUserSettings* Settings = UZenbUserSettings::GetZenbUserSettings();
	Settings->MostRecentSaveGuid = KnownSave.Guid;
	Settings->SaveSettings();

	GEngine->SetClientTravel(GetWorld(), *FString::Printf(TEXT("/Game/Default.Default?LoadSlotGuid=%s"), *KnownSave.Guid.ToString()), TRAVEL_Absolute);
}

void UZenbKnownSaveButton::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();

	if (CustomNameTextBlock)
	{
		CustomNameTextBlock->SetStyle(GetCurrentTextStyleClass());
	}

	if (SavedTimestampTextBlock)
	{
		SavedTimestampTextBlock->SetStyle(GetCurrentTextStyleClass());
	}
}
//~ End UCommonButtonBase Interface
