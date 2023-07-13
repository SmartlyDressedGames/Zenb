// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbUserSettings.h"

// Public Functions:

UZenbUserSettings* UZenbUserSettings::GetZenbUserSettings()
{
	return CastChecked<UZenbUserSettings>(GetGameUserSettings());
}

FGuid UZenbUserSettings::AddKnownSave(const FText& InCustomName)
{
	FZenbKnownSave& KnownSave = KnownSaves.AddDefaulted_GetRef();
	KnownSave.Guid = FGuid::NewGuid();
	KnownSave.CustomName = InCustomName;
	KnownSave.SavedTimestampUtc = FDateTime::UtcNow();
	MostRecentSaveGuid = KnownSave.Guid;
	SaveSettings();
	return KnownSave.Guid;
}
