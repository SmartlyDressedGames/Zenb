// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbMenuContainerWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"
#include "ZenbNotificationWidget.h"
#include "CommonTextBlock.h"
#include "Components/VerticalBox.h"

// Public Interfaces:

//~ Begin UZenbMenuContainerWidgetBase Interface
void UZenbMenuContainerWidget::PushMenu(TSubclassOf<UCommonActivatableWidget> InWidgetClass)
{
	MenuStack->AddWidget(InWidgetClass);
}

void UZenbMenuContainerWidget::PushTextNotification(FText InText)
{
	UZenbNotificationWidget* TextNotificationInstance = CreateWidget<UZenbNotificationWidget>(this, TextNotificationClass);
	if (TextNotificationInstance)
	{
		TextNotificationInstance->TextBlock->SetText(InText);
		NotificationsVerticalBox->AddChildToVerticalBox(TextNotificationInstance);
	}
}
//~ End UZenbMenuContainerWidgetBase Interface
