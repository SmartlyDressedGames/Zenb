// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "ZenbKnownSave.h"
#include "ZenbUserSettings.generated.h"

UCLASS()
class ZENB_API UZenbUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public: // Functions

	static UZenbUserSettings* GetZenbUserSettings();

	FGuid AddKnownSave(const FText& InCustomName);
	const TArray<FZenbKnownSave>& GetKnownSaves() const { return KnownSaves; }

public: // Properties

	/** If set, and level load was not requested, this level will be loaded by default. */
	UPROPERTY(Config)
	FGuid MostRecentSaveGuid;

private: // Properties

	UPROPERTY(Config)
	TArray<FZenbKnownSave> KnownSaves;
};
