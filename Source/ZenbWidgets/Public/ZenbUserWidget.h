// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CommonUserWidget.h"
#include "ZenbUserWidget.generated.h"

UCLASS(Meta = (DisableNativeTick))
class UZenbUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UWidget Interface
#if WITH_EDITOR
	const FText GetPaletteCategory() override;
#endif // WITH_EDITOR
	//~ End UWidget Interface
};
