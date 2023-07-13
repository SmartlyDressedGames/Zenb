// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CommonActivatableWidget.h"
#include "ZenbActivatableWidget.generated.h"

UCLASS(Abstract)
class UZenbActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UWidget Interface
#if WITH_EDITOR
	const FText GetPaletteCategory() override;
#endif // WITH_EDITOR
	//~ End UWidget Interface
};
