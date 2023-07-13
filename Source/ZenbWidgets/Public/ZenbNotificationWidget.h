// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CommonUserWidget.h"
#include "ZenbNotificationWidget.generated.h"

UCLASS(Abstract)
class UZenbNotificationWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY(Meta = (BindWidget))
	class UCommonTextBlock* TextBlock;

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	//~ End UUserWidget Interface

private: // Functions

	void OnNotificationExpired();

private: // Properties

	UPROPERTY(Transient)
	FTimerHandle ExpiryTimerHandle;
};
