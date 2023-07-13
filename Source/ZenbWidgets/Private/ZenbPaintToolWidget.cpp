// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbPaintToolWidget.h"
#include "Components/SpinBox.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "ZenbPawn.h"
#include "ZenbColorSwatchWidget.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "ZenbBrickActor.h"
#include "Engine/LocalPlayer.h"
#include "ZenbToolWidget.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbPaintToolWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (RedSpinBox)
	{
		RedSpinBox->OnValueChanged.AddDynamic(this, &UZenbPaintToolWidget::OnRedChanged);
	}

	if (GreenSpinBox)
	{
		GreenSpinBox->OnValueChanged.AddDynamic(this, &UZenbPaintToolWidget::OnGreenChanged);
	}

	if (BlueSpinBox)
	{
		BlueSpinBox->OnValueChanged.AddDynamic(this, &UZenbPaintToolWidget::OnBlueChanged);
	}

	for (const FLinearColor& SwatchColor : SwatchColors)
	{
		UZenbColorSwatchWidget* SwatchWidget = CreateWidget<UZenbColorSwatchWidget>(this, SwatchClass);
		if (SwatchWidget)
		{
			SwatchPanel->AddChild(SwatchWidget);
			SwatchWidget->SetColor(SwatchColor);
			SwatchWidget->OnClicked.AddDynamic(this, &UZenbPaintToolWidget::OnSwatchClicked);
		}
	}
}

void UZenbPaintToolWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->OnColorSampled.AddDynamic(this, &UZenbPaintToolWidget::OnColorSampled);
	}
}

void UZenbPaintToolWidget::NativeDestruct()
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->OnColorSampled.RemoveAll(this);
	}

	Super::NativeDestruct();
}
//~ End UUserWidget Interface

// Private Functions:

void UZenbPaintToolWidget::OnColorSampled()
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		SelectedColor = FLinearColor(Pawn->ActiveColor.R, Pawn->ActiveColor.G, Pawn->ActiveColor.B, 1.0f);
		FColor sRGB = SelectedColor.ToFColorSRGB();
		ColorImage->SetColorAndOpacity(SelectedColor);
		RedSpinBox->SetValue(sRGB.R);
		GreenSpinBox->SetValue(sRGB.G);
		BlueSpinBox->SetValue(sRGB.B);
	}
}

void UZenbPaintToolWidget::OnSwatchClicked(FLinearColor InColor)
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->ActiveColor = InColor;
		OnColorSampled();
	}
}

void UZenbPaintToolWidget::OnRedChanged(float InValue)
{
	FLinearColor Linear = FLinearColor::FromSRGBColor(FColor(FMath::RoundToInt(InValue), 0.f, 0.f));
	SelectedColor.R = Linear.R;
	ColorImage->SetColorAndOpacity(SelectedColor);
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
		Pawn->ActiveColor = SelectedColor;
}

void UZenbPaintToolWidget::OnGreenChanged(float InValue)
{
	FLinearColor Linear = FLinearColor::FromSRGBColor(FColor(0.f, FMath::RoundToInt(InValue), 0.f));
	SelectedColor.G = Linear.G;
	ColorImage->SetColorAndOpacity(SelectedColor);
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
		Pawn->ActiveColor = SelectedColor;
}

void UZenbPaintToolWidget::OnBlueChanged(float InValue)
{
	FLinearColor Linear = FLinearColor::FromSRGBColor(FColor(0.f, 0.f, FMath::RoundToInt(InValue)));
	SelectedColor.B = Linear.B;
	ColorImage->SetColorAndOpacity(SelectedColor);
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
		Pawn->ActiveColor = SelectedColor;
}
