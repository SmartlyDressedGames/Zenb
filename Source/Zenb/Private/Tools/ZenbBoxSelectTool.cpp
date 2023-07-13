// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbBoxSelectTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"
#include "Transactions/ZenbSelectionTransaction.h"
#include "EngineUtils.h"
#include "ZenbTransactionManager.h"

enum class EZenbBoxSelectToolMode : uint8
{
	/** Default mode replaces initial selection with actors inside the box. */
	OverrideSelection,
	/** Holding SHIFT adds actors inside the box to the initial selection. */
	AddToSelection,
	/** Holding ALT removes actors inside the box from the initial selection. */
	RemoveFromSelection,
};

// Public Interfaces:

//~ Begin UZenbTool Interface
void UZenbBoxSelectTool::Tick()
{
	Super::Tick();

	if (!bIsSelecting)
		return;

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector2D CurrentMousePosition;
	PC->GetMousePosition(CurrentMousePosition.X, CurrentMousePosition.Y);

	FVector2D SelectMin(FMath::Min(MouseDownScreenPosition.X, CurrentMousePosition.X), FMath::Min(MouseDownScreenPosition.Y, CurrentMousePosition.Y));
	FVector2D SelectMax(FMath::Max(MouseDownScreenPosition.X, CurrentMousePosition.X), FMath::Max(MouseDownScreenPosition.Y, CurrentMousePosition.Y));

	EZenbBoxSelectToolMode Mode = EZenbBoxSelectToolMode::OverrideSelection;
	if (PC->IsInputKeyDown(EKeys::LeftShift))
	{
		Mode = EZenbBoxSelectToolMode::AddToSelection;
	}
	else if (PC->IsInputKeyDown(EKeys::LeftAlt))
	{
		Mode = EZenbBoxSelectToolMode::RemoveFromSelection;
	}
	
	if (Mode == EZenbBoxSelectToolMode::AddToSelection)
	{
		// If tool was previously in remove or override mode then we need to re-select the deselected actors.
		for (AZenbBrickActor* Actor : RemovedActors)
		{
			Pawn->SelectActor(Actor);
		}
		RemovedActors.Reset();
	}
	else if (Mode == EZenbBoxSelectToolMode::RemoveFromSelection)
	{
		// If tool was previously in add or override mode then we need to de-select the selected actors.
		for (AZenbBrickActor* Actor : AddedActors)
		{
			Pawn->DeselectActor(Actor);
		}
		AddedActors.Reset();
	}

	for (TActorIterator<AZenbBrickActor> Iter(GetWorld()); Iter; ++Iter)
	{
		AZenbBrickActor* Actor = *Iter;
		check(Actor);
		if (!Actor->CanBeSelected())
			continue;

		if (Mode == EZenbBoxSelectToolMode::AddToSelection)
		{
			if (InitialSelection.Contains(Actor))
			{ 
				continue;
			}
		}
		else if (Mode == EZenbBoxSelectToolMode::RemoveFromSelection)
		{
			if (!InitialSelection.Contains(Actor))
			{
				continue;
			}
		}

		bool bWithinBox = false;

		FVector2D ScreenPosition;
		if (PC->ProjectWorldLocationToScreen(Actor->GetActorLocation(), ScreenPosition, true))
		{
			if (ScreenPosition.X >= SelectMin.X && ScreenPosition.X <= SelectMax.X && ScreenPosition.Y >= SelectMin.Y && ScreenPosition.Y <= SelectMax.Y)
			{
				bWithinBox = true;
			}
		}

		if (Mode == EZenbBoxSelectToolMode::AddToSelection)
		{
			if (bWithinBox)
			{
				if (!Actor->ZenbIsSelected())
				{
					bool bWasAlreadyInSet;
					AddedActors.Add(Actor, &bWasAlreadyInSet);
					check(!bWasAlreadyInSet);
					Pawn->SelectActor(Actor);
				}
			}
			else
			{
				if (Actor->ZenbIsSelected())
				{
					int32 NumRemoved = AddedActors.Remove(Actor);
					check(NumRemoved == 1);
					Pawn->DeselectActor(Actor);
				}
			}
		}
		else if (Mode == EZenbBoxSelectToolMode::RemoveFromSelection)
		{
			if (bWithinBox)
			{
				if (Actor->ZenbIsSelected())
				{
					bool bWasAlreadyInSet;
					RemovedActors.Add(Actor, &bWasAlreadyInSet);
					check(!bWasAlreadyInSet);
					Pawn->DeselectActor(Actor);
				}
			}
			else
			{
				if (!Actor->ZenbIsSelected())
				{
					int32 NumRemoved = RemovedActors.Remove(Actor);
					check(NumRemoved == 1);
					Pawn->SelectActor(Actor);
				}
			}
		}
		else
		{
			if (bWithinBox)
			{
				if (!Actor->ZenbIsSelected())
				{
					if (InitialSelection.Contains(Actor))
					{
						// Was initially selected, so cancel the deselection.
						int32 NumRemoved = RemovedActors.Remove(Actor);
						check(NumRemoved == 1);
					}
					else
					{
						// Was not initially selected, so record selecting as a change.
						bool bWasAlreadyInSet;
						AddedActors.Add(Actor, &bWasAlreadyInSet);
						check(!bWasAlreadyInSet);
					}

					Pawn->SelectActor(Actor);
				}
			}
			else
			{
				if (Actor->ZenbIsSelected())
				{
					if (InitialSelection.Contains(Actor))
					{
						// Was initially selected, so record deselecting as a change.
						bool bWasAlreadyInSet;
						RemovedActors.Add(Actor, &bWasAlreadyInSet);
						check(!bWasAlreadyInSet);
					}
					else
					{
						// Was not initially selected, so cancel the selection.
						int32 NumRemoved = AddedActors.Remove(Actor);
						check(NumRemoved == 1);
					}

					Pawn->DeselectActor(Actor);
				}
			}
		}
	}
}

void UZenbBoxSelectTool::OnLeftMousePressed()
{
	InitialSelection = Pawn->GetSelection();
	InitialActiveActor = Pawn->GetActiveActor();
	bIsSelecting = true;
	Pawn->GetController<APlayerController>()->GetMousePosition(MouseDownScreenPosition.X, MouseDownScreenPosition.Y);
}

void UZenbBoxSelectTool::OnLeftMouseReleased()
{
	if (AddedActors.Num() + RemovedActors.Num() > 0)
	{
		UZenbSelectionTransaction* Transaction = Pawn->GetTransactionManager()->NewTransaction<UZenbSelectionTransaction>();
		Transaction->RemovedActors = RemovedActors;
		Transaction->AddedActors = AddedActors;
		Transaction->OldActiveActor;
		Transaction->NewActiveActor = Pawn->GetActiveActor();
		RemovedActors.Reset();
		AddedActors.Reset();
		Pawn->GetTransactionManager()->AddTransaction(Transaction);
	}

	bIsSelecting = false;
}
//~ End UZenbTool Interface
