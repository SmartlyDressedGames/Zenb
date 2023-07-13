// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbPaintTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"
#include "Transactions/ZenbPaintTransaction.h"
#include "ZenbTransactionManager.h"

// Public Interfaces:

//~ Begin UZenbTool Interface
void UZenbPaintTool::Tick()
{
	Super::Tick();

	if (!bIsPainting)
		return;

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, MouseRayLocation, MouseRayLocation + MouseRayDirection * 10000.f, ECC_Visibility))
	{
		if (AZenbBrickActor* Part = Cast<AZenbBrickActor>(Hit.GetActor()))
		{
			TrackColorChange(Part, Pawn->ActiveColor);
			Part->SetColor(Pawn->ActiveColor);
		}
	}
}

void UZenbPaintTool::OnLeftMousePressed()
{
	if (bIsSamplingColors)
	{
		APlayerController* PC = Pawn->GetController<APlayerController>();
		if (!PC)
			return;

		FVector WorldLocation;
		FVector WorldDirection;
		if (!PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
			return;

		FHitResult MouseDownHit;
		if (GetWorld()->LineTraceSingleByChannel(MouseDownHit, WorldLocation, WorldLocation + WorldDirection * 10000.f, ECC_Visibility))
		{
			if (AZenbBrickActor* HitBrick = Cast<AZenbBrickActor>(MouseDownHit.GetActor()))
			{
				Pawn->ActiveColor = HitBrick->GetColor();
				Pawn->OnColorSampled.Broadcast();
			}
		}
	}
	else
	{
		bIsPainting = true;
		PendingPaintChanges.Reset();
	}
}

void UZenbPaintTool::OnLeftMouseReleased()
{
	if (PendingPaintChanges.Num() > 0)
	{
		UZenbPaintTransaction* Transaction = Pawn->GetTransactionManager()->NewTransaction<UZenbPaintTransaction>();
		Transaction->ModifiedBricks = PendingPaintChanges;
		PendingPaintChanges.Reset();
		Pawn->GetTransactionManager()->AddTransaction(Transaction);
		Pawn->SetWorldDirtyForAutosave();
	}

	bIsPainting = false;
}

void UZenbPaintTool::OnSampleColorPressed()
{
	bIsSamplingColors = true;
	if (APlayerController* PC = Pawn->GetController<APlayerController>())
	{
		PC->CurrentMouseCursor = EMouseCursor::EyeDropper;
	}
}

void UZenbPaintTool::OnSampleColorReleased()
{
	bIsSamplingColors = false;
	if (APlayerController* PC = Pawn->GetController<APlayerController>())
	{
		PC->CurrentMouseCursor = EMouseCursor::Default;
	}
}
//~ End UZenbTool Interface

// Private Functions:

void UZenbPaintTool::TrackColorChange(class AZenbBrickActor* InActor, FLinearColor InColor)
{
	for (const FZenbPaintDelta& Item : PendingPaintChanges)
	{
		if (Item.Actor == InActor)
			return;
	}

	FZenbPaintDelta& Item = PendingPaintChanges.AddDefaulted_GetRef();
	Item.Actor = InActor;
	Item.OldColor = InActor->GetColor();
	Item.NewColor = InColor;
}
