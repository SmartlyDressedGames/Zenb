// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbColorSwatchWidget.h"
#include "Components/Image.h"
#include "Components/Button.h"

// Public Functions:

void UZenbColorSwatchWidget::SetColor(FLinearColor InColor)
{
	Color = InColor;
	Image->SetColorAndOpacity(InColor);
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbColorSwatchWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UZenbColorSwatchWidget::OnButtonClicked);
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UZenbColorSwatchWidget::OnButtonClicked()
{
	OnClicked.Broadcast(Color);
}
