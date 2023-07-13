// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbPlayerController.h"
#include "ZenbMenuContainerWidgetBase.h"
#include "CommonActivatableWidget.h"

AZenbPlayerController::AZenbPlayerController() : Super()
{
	bShowMouseCursor = true;
}

// Public Functions:

void AZenbPlayerController::PushMenu(TSubclassOf<UCommonActivatableWidget> InWidgetClass)
{
	if (MenuContainerInstance)
	{
		MenuContainerInstance->PushMenu(InWidgetClass);
	}
}

void AZenbPlayerController::PushTextNotification(FText InText)
{
	if (MenuContainerInstance)
	{
		MenuContainerInstance->PushTextNotification(InText);
	}
}

// Public Interfaces:

//~ Begin AActor Interface
void AZenbPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MenuContainerInstance = CreateWidget<UZenbMenuContainerWidgetBase>(this, MenuContainerClass);
	if (MenuContainerInstance)
	{
		MenuContainerInstance->AddToPlayerScreen(100);
	}
}
//~ End AActor Interface

// Protected Interfaces:

//~ Begin APlayerController Interface
void AZenbPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AZenbPlayerController::OnEscapePressed);
	}
}
//~ End APlayerController Interface

// Private Functions:

void AZenbPlayerController::OnEscapePressed()
{
	PushMenu(PauseMenuClass);
}
