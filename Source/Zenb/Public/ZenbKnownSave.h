// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "ZenbKnownSave.generated.h"

USTRUCT()
struct ZENB_API FZenbKnownSave
{
	GENERATED_BODY()

	/**
	 * File name uses guid rather than custom name.
	 * This avoids problems with restricted characters in file names, and allows the item to be renamed easily.
	 */
	UPROPERTY()
	FGuid Guid;

	/** Player-specified name shown in the user interface. */
	UPROPERTY()
	FText CustomName;

	/** UTC timestamp when the file was last written. */
	UPROPERTY()
	FDateTime SavedTimestampUtc;
};
