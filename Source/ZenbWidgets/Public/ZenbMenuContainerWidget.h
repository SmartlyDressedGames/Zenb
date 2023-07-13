// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbMenuContainerWidgetBase.h"
#include "ZenbMenuContainerWidget.generated.h"

UCLASS(Abstract)
class UZenbMenuContainerWidget : public UZenbMenuContainerWidgetBase
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbMenuContainerWidgetBase Interface
	void PushMenu(TSubclassOf<UCommonActivatableWidget> InWidgetClass) override;
	void PushTextNotification(FText InText) override;
	//~ End UZenbMenuContainerWidgetBase Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UCommonActivatableWidgetStack* MenuStack;
	
	UPROPERTY(Meta = (BindWidget))
	class UVerticalBox* NotificationsVerticalBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UZenbNotificationWidget> TextNotificationClass;
};
