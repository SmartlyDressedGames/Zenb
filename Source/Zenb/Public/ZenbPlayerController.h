// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/PlayerController.h"
#include "ZenbPlayerController.generated.h"

UCLASS()
class ZENB_API AZenbPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AZenbPlayerController();

public: // Functions

	UFUNCTION(BlueprintCallable)
	void PushMenu(TSubclassOf<class UCommonActivatableWidget> InWidgetClass);

	void PushTextNotification(FText InText);

public: // Interfaces

	//~ Begin AActor Interface
	void BeginPlay() override;
	//~ End AActor Interface

protected: // Interfaces

	//~ Begin APlayerController Interface
	void SetupInputComponent() override;
	//~ End APlayerController Interface

private: // Functions

	void OnEscapePressed();

private: // Properties

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UZenbMenuContainerWidgetBase> MenuContainerClass;

	UPROPERTY(Transient)
	class UZenbMenuContainerWidgetBase* MenuContainerInstance;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCommonActivatableWidget> PauseMenuClass;
};
