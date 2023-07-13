// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbSpawnTransaction.h"
#include "ZenbPawn.h"
#include "ZenbBrickActor.h"

// Public Interfaces:

//~ Begin UZenbTransaction Interface
FString UZenbSpawnTransaction::GetTransactionSummary() const
{
	return FString::Printf(TEXT("Spawn %i actor(s)"), Actors.Num());
}

void UZenbSpawnTransaction::Undo()
{
	GetPawn()->ClearSelection();
	for (AZenbBrickActor* Actor : SourceActors)
	{
		GetPawn()->SelectActor(Actor);
	}

	for (AZenbBrickActor* Actor : Actors)
	{
		check(Actor);
		check(!Actor->bIsHiddenForSpawn);
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->bIsHiddenForSpawn = true;
	}

	GetPawn()->SetActiveActor(OldActiveActor);
	GetPawn()->SetWorldDirtyForAutosave();
}

void UZenbSpawnTransaction::Redo()
{
	GetPawn()->ClearSelection();

	for (AZenbBrickActor* Actor : Actors)
	{
		check(Actor);
		check(Actor->bIsHiddenForSpawn);
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
		Actor->bIsHiddenForSpawn = false;
		GetPawn()->SelectActor(Actor);
	}

	GetPawn()->SetActiveActor(NewActiveActor);
	GetPawn()->SetWorldDirtyForAutosave();
}

void UZenbSpawnTransaction::Forget()
{
	if (bInRedoStack)
	{
		for (TObjectPtr<AZenbBrickActor>& Actor : Actors)
		{
			check(Actor);
			check(Actor->bIsHiddenForSpawn);
			Actor->Destroy();
			Actor = nullptr;
		}
	}
}

//~ End UZenbTransaction Interface
