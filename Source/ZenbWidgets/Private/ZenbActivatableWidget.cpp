// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbActivatableWidget.h"
#include "ZenbWidgetPaletteCategories.h"

//~ Begin UWidget Interface
#if WITH_EDITOR
const FText UZenbActivatableWidget::GetPaletteCategory()
{
	return ZenbWidgetPaletteCategories::Default;
}
#endif // WITH_EDITOR
//~ End UWidget Interface
