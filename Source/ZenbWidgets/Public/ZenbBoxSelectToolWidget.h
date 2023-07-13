// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbUserWidget.h"
#include "ZenbBoxSelectToolWidget.generated.h"

UCLASS()
class UZenbBoxSelectToolWidget : public UZenbUserWidget
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UWidget> BoxWidget;
};
