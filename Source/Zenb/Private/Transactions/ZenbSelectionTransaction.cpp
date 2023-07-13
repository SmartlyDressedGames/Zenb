// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbSelectionTransaction.h"
#include "ZenbPawn.h"
#include "ZenbBrickActor.h"

// Public Interfaces:

//~ Begin UZenbTransaction Interface
FString UZenbSelectionTransaction::GetTransactionSummary() const
{
	if (AddedActors.Num() > 0 && RemovedActors.Num() < 1)
	{
		return FString::Printf(TEXT("Select %i actor(s)"), AddedActors.Num());
	}
	else if (AddedActors.Num() < 1 && RemovedActors.Num() > 0)
	{
		return FString::Printf(TEXT("Deselect %i actor(s)"), RemovedActors.Num());
	}
	else
	{
		return FString::Printf(TEXT("Select %i actor(s) and Deselect %i actor(s)"), AddedActors.Num(), RemovedActors.Num());
	}
}

void UZenbSelectionTransaction::Undo()
{
	if (AZenbPawn* Pawn = GetPawn())
	{
		for (AZenbBrickActor* Actor : RemovedActors)
		{
			Pawn->SelectActor(Actor);
		}

		for (AZenbBrickActor* Actor : AddedActors)
		{
			Pawn->DeselectActor(Actor);
		}

		Pawn->SetActiveActor(OldActiveActor);
	}
}

void UZenbSelectionTransaction::Redo()
{
	if (AZenbPawn* Pawn = GetPawn())
	{
		for (AZenbBrickActor* Actor : RemovedActors)
		{
			Pawn->DeselectActor(Actor);
		}

		for (AZenbBrickActor* Actor : AddedActors)
		{
			Pawn->SelectActor(Actor);
		}

		Pawn->SetActiveActor(NewActiveActor);
	}
}
//~ End UZenbTransaction Interface
