// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CommonUserWidget.h"
#include "ZenbMenuContainerWidgetBase.generated.h"

UCLASS(Abstract)
class ZENB_API UZenbMenuContainerWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()

public: // Functions

	virtual void PushMenu(TSubclassOf<class UCommonActivatableWidget> InWidgetClass) {}
	virtual void PushTextNotification(FText InText) {}
};
