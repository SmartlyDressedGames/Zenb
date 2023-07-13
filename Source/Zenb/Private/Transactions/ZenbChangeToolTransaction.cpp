// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbChangeToolTransaction.h"
#include "ZenbPawn.h"
#include "Tools/ZenbTool.h"

// Public Interfaces:

//~ Begin UZenbTransaction Interface
FString UZenbChangeToolTransaction::GetTransactionSummary() const
{
	return FString::Printf(TEXT("ChangeTool from %s to %s"), *GetNameSafe(OldTool.Get()), *GetNameSafe(NewTool.Get()));
}

void UZenbChangeToolTransaction::Undo()
{
	if (AZenbPawn* Pawn = GetPawn())
	{
		Pawn->SetActiveTool(OldTool);

		for (AZenbBrickActor* Actor : RemovedActors)
		{
			Pawn->SelectActor(Actor);
		}

		Pawn->SetActiveActor(OldActiveActor);
	}
}

void UZenbChangeToolTransaction::Redo()
{
	if (AZenbPawn* Pawn = GetPawn())
	{
		for (AZenbBrickActor* Actor : RemovedActors)
		{
			Pawn->DeselectActor(Actor);
		}

		Pawn->SetActiveTool(NewTool);
	}
}
//~ End UZenbTransaction Interface
