// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbNotificationWidget.h"
#include "TimerManager.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(ExpiryTimerHandle, this, &UZenbNotificationWidget::OnNotificationExpired, 2.0f);
}
//~ End UUserWidget Interface

// Private Functions:

void UZenbNotificationWidget::OnNotificationExpired()
{
	RemoveFromParent();
}
