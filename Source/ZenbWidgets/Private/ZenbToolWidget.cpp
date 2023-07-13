// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbToolWidget.h"
#include "Components/Button.h"
#include "ZenbPawn.h"
#include "Components/TextBlock.h"
#include "Tools/ZenbTool.h"

// Public Functions:

void UZenbToolWidget::SetTool(UZenbTool* InTool)
{
	Tool = InTool;

	if (DisplayNameLabel)
	{
		DisplayNameLabel->SetText(Tool->DisplayNameText);
	}
	if (HotkeyLabel)
	{
		HotkeyLabel->SetText(Tool->HotkeyText);
	}
}

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbToolWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->OnActiveToolChanged.AddDynamic(this, &UZenbToolWidget::OnActiveToolChanged);

		if (UZenbTool* FoundTool = Pawn->FindToolByClass(ToolClass))
		{
			SetTool(FoundTool);
		}

		OnActiveToolChanged();
	}
}

void UZenbToolWidget::NativeDestruct()
{
	Super::NativeConstruct();

	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->OnActiveToolChanged.RemoveAll(this);

		if (UZenbTool* FoundTool = Pawn->FindToolByClass(ToolClass))
		{
			SetTool(FoundTool);
		}
	}
}

void UZenbToolWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UZenbToolWidget::OnButtonClicked);
	}
}
//~ End UUserWidget Interface

// Private Functions:

void UZenbToolWidget::OnButtonClicked()
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->RequestChangeTool(Tool);
	}
}

void UZenbToolWidget::OnActiveToolChanged()
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		bool bNewIsActive = Pawn->GetActiveTool() == Tool;
		if (bIsActive != bNewIsActive)
		{
			bIsActive = bNewIsActive;
			if (bIsActive)
			{
				PlayAnimationForward(EquipAnim);
			}
			else
			{
				PlayAnimationReverse(EquipAnim);
			}
		}
	}
}
