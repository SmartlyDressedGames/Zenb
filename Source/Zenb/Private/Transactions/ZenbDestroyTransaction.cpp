// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbDestroyTransaction.h"
#include "ZenbBrickActor.h"
#include "ZenbPawn.h"

void UZenbDestroyTransaction::AddActor(class AZenbBrickActor* InActor)
{	
	check(InActor);
	check(!InActor->bIsHiddenForDeletion);
	GetPawn()->DeselectActor(InActor);
	Actors.Add(InActor);
	InActor->SetActorHiddenInGame(true);
	InActor->SetActorEnableCollision(false);
	InActor->bIsHiddenForDeletion = true;
}

FString UZenbDestroyTransaction::GetTransactionSummary() const
{
	return FString::Printf(TEXT("Destroy %i actor(s)"), Actors.Num());
}

void UZenbDestroyTransaction::Undo()
{
	for (AZenbBrickActor* Actor : Actors)
	{
		check(Actor);
		check(Actor->bIsHiddenForDeletion);
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
		Actor->bIsHiddenForDeletion = false;
		GetPawn()->SelectActor(Actor);
	}

	GetPawn()->SetActiveActor(OldActiveActor);
	GetPawn()->SetWorldDirtyForAutosave();
}

void UZenbDestroyTransaction::Redo()
{
	for (AZenbBrickActor* Actor : Actors)
	{
		check(Actor);
		check(!Actor->bIsHiddenForDeletion);
		GetPawn()->DeselectActor(Actor);
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->bIsHiddenForDeletion = true;
	}
	GetPawn()->SetWorldDirtyForAutosave();
}

void UZenbDestroyTransaction::Forget()
{
	if (bInUndoStack)
	{
		for (TObjectPtr<AZenbBrickActor>& Actor : Actors)
		{
			check(Actor);
			check(Actor->bIsHiddenForDeletion);
			Actor->Destroy();
			Actor = nullptr;
		}
	}
}
