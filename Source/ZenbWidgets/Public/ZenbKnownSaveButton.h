// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CommonButtonBase.h"
#include "ZenbKnownSave.h"
#include "ZenbKnownSaveButton.generated.h"

UCLASS(Abstract)
class UZenbKnownSaveButton : public UCommonButtonBase
{
	GENERATED_BODY()

public: // Functions

	void SetKnownSave(const FZenbKnownSave& InKnownSave);

protected: // Interfaces

	//~ Begin UCommonButtonBase Interface
	void NativeOnClicked() override;
	void NativeOnCurrentTextStyleChanged() override;
	//~ End UCommonButtonBase Interface

private: // Properties

	UPROPERTY(Transient)
	FZenbKnownSave KnownSave;

	UPROPERTY(Meta = (BindWidget))
	class UCommonTextBlock* CustomNameTextBlock;
	
	UPROPERTY(Meta = (BindWidget))
	class UCommonTextBlock* SavedTimestampTextBlock;
};
