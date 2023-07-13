// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbSelectTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "ZenbBrickActor.h"
#include "Transactions/ZenbSelectionTransaction.h"
#include "ZenbTransactionManager.h"

// Public Interfaces:

//~ Begin UZenbTool Interface
void UZenbSelectTool::Tick()
{
	Super::Tick();

	if (!bIsSelecting)
		return;

	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	bool bIsRemovingFromSelection = PC->IsInputKeyDown(EKeys::LeftShift);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, MouseRayLocation, MouseRayLocation + MouseRayDirection * 10000.f, ECC_Visibility))
	{
		AZenbBrickActor* Actor = Cast<AZenbBrickActor>(Hit.GetActor());
		if (Actor && Actor->CanBeSelected())
		{
			if (bIsRemovingFromSelection)
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
			else
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
		}
	}
}

void UZenbSelectTool::OnLeftMousePressed()
{
	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector MouseRayLocation;
	FVector MouseRayDirection;
	if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
		return;

	FHitResult MouseDownHit;
	if (!GetWorld()->LineTraceSingleByChannel(MouseDownHit, MouseRayLocation, MouseRayLocation + MouseRayDirection * 10000.f, ECC_Visibility))
	{
		// Clearing selection when clicking on the sky is expected.
		Pawn->RequestClearSelection();
		return;
	}

	AZenbBrickActor* MouseDownActor = Cast<AZenbBrickActor>(MouseDownHit.GetActor());
	if (!MouseDownActor || !MouseDownActor->CanBeSelected())
	{
		// Clearing selection when clicking on baseplate is expected.
		Pawn->RequestClearSelection();
		MouseDownActor = nullptr;
		return;
	}

	InitialSelection = Pawn->GetSelection();
	InitialActiveActor = Pawn->GetActiveActor();
	bIsSelecting = true;
}

void UZenbSelectTool::OnLeftMouseReleased()
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

bool UZenbSelectTool::ShouldHoverActor(AZenbBrickActor* InActor) const
{
	return InActor->CanBeSelected();
}
//~ End UZenbTool Interface
