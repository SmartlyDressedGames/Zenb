// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbTransformTransaction.h"
#include "ZenbBrickActor.h"
#include "ZenbPawn.h"

FZenbTransformDelta::FZenbTransformDelta()
{
	Actor = nullptr;
	OldLocation = FVector::ZeroVector;
	OldQuat = FQuat::Identity;
	NewLocation = FVector::ZeroVector;
	NewQuat = FQuat::Identity;
	OldDimensions = FIntVector::ZeroValue;
	NewDimensions = FIntVector::ZeroValue;
}

// Public Interfaces:

//~ Begin UZenbTransaction Interface
FString UZenbTransformTransaction::GetTransactionSummary() const
{
	return FString::Printf(TEXT("Transform %i actor(s)"), ModifiedBricks.Num());
}

void UZenbTransformTransaction::Undo()
{
	for (const FZenbTransformDelta& Item : ModifiedBricks)
	{
		Item.Actor->SetActorLocationAndRotation(Item.OldLocation, Item.OldQuat);
		Item.Actor->Dimensions = Item.OldDimensions;
		Item.Actor->UpdateMeshFromDimensions();
	}
	GetPawn()->SetWorldDirtyForAutosave();
}

void UZenbTransformTransaction::Redo()
{
	for (const FZenbTransformDelta& Item : ModifiedBricks)
	{
		Item.Actor->SetActorLocationAndRotation(Item.NewLocation, Item.NewQuat);
		Item.Actor->Dimensions = Item.NewDimensions;
		Item.Actor->UpdateMeshFromDimensions();
	}
	GetPawn()->SetWorldDirtyForAutosave();
}
//~ End UZenbTransaction Interface
