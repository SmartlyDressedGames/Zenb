// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbBoxSelectToolWidget.h"
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
#include "Tools/ZenbBoxSelectTool.h"
#include "GameFramework/PlayerController.h"

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbBoxSelectToolWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>();
	if (Pawn == nullptr)
		return;

	UZenbBoxSelectTool* Tool = Cast<UZenbBoxSelectTool>(Pawn->GetActiveTool());
	if (Tool == nullptr)
		return;

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	ULocalPlayer* LP = PC->GetLocalPlayer();
	if (!LP || !LP->ViewportClient)
		return;

	FSceneViewProjectionData ProjectionData;
	if (!LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
		return;

	FVector2D CurrentMousePosition;
	PC->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);

	if (Tool->bIsSelecting)
	{
		UCanvasPanelSlot* BoxSlot = Cast<UCanvasPanelSlot>(BoxWidget->Slot);
		FVector2D SelectMin(FMath::Min(Tool->MouseDownScreenPosition.X, CurrentMousePosition.X), FMath::Min(Tool->MouseDownScreenPosition.Y, CurrentMousePosition.Y));
		FVector2D SelectMax(FMath::Max(Tool->MouseDownScreenPosition.X, CurrentMousePosition.X), FMath::Max(Tool->MouseDownScreenPosition.Y, CurrentMousePosition.Y));

		const FIntRect& ViewRect = ProjectionData.GetConstrainedViewRect();
		FVector2D NormalizedMin((SelectMin.X - ViewRect.Min.X) / ViewRect.Width(), (SelectMin.Y - ViewRect.Min.Y) / ViewRect.Height());
		FVector2D NormalizedMax((SelectMax.X - ViewRect.Min.X) / ViewRect.Width(), (SelectMax.Y - ViewRect.Min.Y) / ViewRect.Height());

		BoxSlot->SetAnchors(FAnchors(NormalizedMin.X, NormalizedMin.Y, NormalizedMax.X, NormalizedMax.Y));

		BoxWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		BoxWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
//~ End UUserWidget Interface
