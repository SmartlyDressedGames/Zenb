// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbMainWidget.h"
#include "ZenbAxisHandleWidget.h"
#include "ZenbPawn.h"
#include "ZenbColorSwatchWidget.h"
#include "Components/PanelWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "ZenbBrickActor.h"
#include "Engine/LocalPlayer.h"
#include "ZenbToolWidget.h"
#include "Tools/ZenbTool.h"
#include "Input/CommonInputMode.h"
#include "ZenbPawn.h"
#include "Widgets/CommonActivatableWidgetContainer.h"

// Public Interfaces:

//~ Begin UCommonActivatableWidget Interface
TOptional<FUIInputConfig> UZenbMainWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::CaptureDuringMouseDown, false);
}
//~ End UCommonActivatableWidget Interface

// Protected Interfaces:

//~ Begin UUserWidget Interface
void UZenbMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->OnActiveToolChanged.AddDynamic(this, &UZenbMainWidget::OnActiveToolChanged);
		OnActiveToolChanged();
	}
}

void UZenbMainWidget::NativeDestruct()
{
	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		Pawn->OnActiveToolChanged.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UZenbMainWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>();
	if (Pawn == nullptr)
		return;

	UZenbTool* Tool = Pawn->GetActiveTool();
	AZenbBrickActor* ActiveBrick = Pawn->GetActiveActor();
	if (ActiveBrick == nullptr || Tool == nullptr || !Tool->ShouldHandlesBeVisible())
	{
		DragForwardButton->SetVisibility(ESlateVisibility::Collapsed);
		DragBackwardButton->SetVisibility(ESlateVisibility::Collapsed);
		DragUpButton->SetVisibility(ESlateVisibility::Collapsed);
		DragDownButton->SetVisibility(ESlateVisibility::Collapsed);
		DragLeftButton->SetVisibility(ESlateVisibility::Collapsed);
		DragRightButton->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	FBox LocalBounds = ActiveBrick->CalculateComponentsBoundingBoxInLocalSpace(true, false);
	FVector LocalCenter;
	FVector LocalExtents;
	LocalBounds.GetCenterAndExtents(LocalCenter, LocalExtents);

	const FTransform& ActorToWorld = ActiveBrick->GetActorTransform();

	ULocalPlayer* LP = GetOwningLocalPlayer();
	if (LP == nullptr || LP->ViewportClient == nullptr)
		return;

	FSceneViewProjectionData ProjectionData;
	if (!LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
		return;

	const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();

	auto ProjectToAnchors = [&](FVector InWorldLocation, FVector2D& OutScreen)
	{
		FVector4 Result = ViewProjectionMatrix.TransformFVector4(FVector4(InWorldLocation, 1.f));
		if (Result.W > 0.0f)
		{
			// the result of this will be x and y coords in -1..1 projection space
			const float RHW = 1.0f / Result.W;
			FPlane PosInScreenSpace = FPlane(Result.X * RHW, Result.Y * RHW, Result.Z * RHW, Result.W);

			// Move from projection space to normalized 0..1 UI space
			OutScreen.X = (PosInScreenSpace.X / 2.f) + 0.5f;
			OutScreen.Y = 1.f - (PosInScreenSpace.Y / 2.f) - 0.5f;
			return true;
		}
		else
		{
			return false;
		}
	};

	auto ProjectWorldDirectionToScreen = [&](FVector InWorldDirection)
	{
		const FIntRect& ViewRect = ProjectionData.GetConstrainedViewRect();
		FVector4 Result = ViewProjectionMatrix.TransformFVector4(FVector4(InWorldDirection, 0.f));
		Result.X *= ViewRect.Width();
		Result.Y *= -ViewRect.Height();
		return Result;
	};

	FVector CameraDirection = ProjectionData.ViewRotationMatrix.InverseTransformVector(FVector(0.f, 0.f, 1.f));

	auto UpdateWidgetAnchor = [&](UZenbAxisHandleWidget* InWidget, FVector InLocalDirection, float InDistance)
	{
		UCanvasPanelSlot* ButtonCanvasSlot = Cast<UCanvasPanelSlot>(InWidget->Slot);
		if (ButtonCanvasSlot == nullptr)
			return;

		FVector HandlePosition = ActorToWorld.TransformPosition(InLocalDirection * InDistance);
		FVector2D NormalizedScreenPosition;
		if (ProjectToAnchors(FVector4(HandlePosition, 1.f), NormalizedScreenPosition))
		{
			FVector WorldDirection = ActorToWorld.TransformVector(InLocalDirection);
			float CameraAlignment = FMath::Abs(FVector::DotProduct(WorldDirection, CameraDirection));

			bool bIsHidden = CameraAlignment > 0.8f;
			InWidget->SetIsHidden(bIsHidden);
			InWidget->SetVisibility(bIsHidden ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Visible);

			FVector4 ScreenDirection = ProjectWorldDirectionToScreen(WorldDirection);
			float ViewAngle = FMath::Atan2(ScreenDirection.Y, ScreenDirection.X);
			InWidget->SetRenderTransformAngle(FMath::RadiansToDegrees(ViewAngle));
			ButtonCanvasSlot->SetAnchors(FAnchors(NormalizedScreenPosition.X, NormalizedScreenPosition.Y));
		}
		else
		{
			InWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	};

	UpdateWidgetAnchor(DragForwardButton, FVector::ForwardVector, LocalExtents.X + 20.f);
	UpdateWidgetAnchor(DragBackwardButton, FVector::BackwardVector, LocalExtents.X + 20.f);
	UpdateWidgetAnchor(DragUpButton, FVector::UpVector, LocalExtents.Z + 20.f);
	UpdateWidgetAnchor(DragDownButton, FVector::DownVector, LocalExtents.Z + 20.f);
	UpdateWidgetAnchor(DragLeftButton, FVector::LeftVector, LocalExtents.Y + 20.f);
	UpdateWidgetAnchor(DragRightButton, FVector::RightVector, LocalExtents.Y + 20.f);
}
//~ End UUserWidget Interface

//~ Begin UCommonActivatableWidget Interface
void UZenbMainWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
	}
}
//~ End UCommonActivatableWidget Interface

// Private Functions:

void UZenbMainWidget::OnActiveToolChanged()
{
	if (ToolWidgetInstance)
	{
		ToolWidgetInstance->RemoveFromParent();
		ToolWidgetInstance = nullptr;
	}

	if (AZenbPawn* Pawn = GetOwningPlayerPawn<AZenbPawn>())
	{
		if (UZenbTool* ActiveTool = Pawn->GetActiveTool())
		{
			if (ActiveTool->WidgetClass)
			{
				ToolWidgetInstance = CreateWidget<UUserWidget>(this, ActiveTool->WidgetClass);
				AddToolWidget(ToolWidgetInstance);
			}
		}
	}
}
