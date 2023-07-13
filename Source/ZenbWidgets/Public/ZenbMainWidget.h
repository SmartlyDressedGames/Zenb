// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbActivatableWidget.h"
#include "ZenbMainWidget.generated.h"

UCLASS()
class UZenbMainWidget : public UZenbActivatableWidget
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UCommonActivatableWidget Interface
	TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~ End UCommonActivatableWidget Interface

protected: // Functions

	UFUNCTION(BlueprintImplementableEvent)
	void AddToolWidget(class UWidget* InWidget);

protected: // Interfaces

	//~ Begin UUserWidget Interface
	void NativeConstruct() override;
	void NativeDestruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	//~ End UUserWidget Interface

	//~ Begin UCommonActivatableWidget Interface
	void NativeOnActivated() override;
	//~ End UCommonActivatableWidget Interface

private: // Functions

	UFUNCTION()
	void OnActiveToolChanged();

private: // Properties

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UZenbAxisHandleWidget> DragForwardButton;

	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UZenbAxisHandleWidget> DragBackwardButton;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UZenbAxisHandleWidget> DragUpButton;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UZenbAxisHandleWidget> DragDownButton;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UZenbAxisHandleWidget> DragLeftButton;
	
	UPROPERTY(Meta = (BindWidget))
	TObjectPtr<class UZenbAxisHandleWidget> DragRightButton;

	UPROPERTY(Transient)
	TObjectPtr<class UUserWidget> ToolWidgetInstance; 
};
