// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbAxisHandleWidget.h"
#include "Components/Button.h"
#include "Tools/ZenbTool.h"
#include "ZenbPawn.h"

// Public Functions:

void UZenbAxisHandleWidget::SetIsHidden(bool bInHidden)
{
	if (bIsHidden != bInHidden)
	{
		bIsHidden = bInHidden;
		if (bInHidden)
		{
			PlayAnimationForward(HideAnim);
		}
		else
		{
			PlayAnimationReverse(HideAnim);
		}
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbAxisHandleWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UZenbAxisHandleWidget::OnButtonClicked);
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UZenbAxisHandleWidget::OnButtonClicked()
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->GetActiveTool()->BeginDraggingHandle(LocalDirection);
	}
}
