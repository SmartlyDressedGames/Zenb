// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbPaintTransaction.h"
#include "ZenbBrickActor.h"
#include "ZenbPawn.h"

FZenbPaintDelta::FZenbPaintDelta()
{
	Actor = nullptr;
	OldColor = FLinearColor::Black;
	NewColor = FLinearColor::Black;
}

// Public Interfaces:

//~ Begin UZenbTransaction Interface
FString UZenbPaintTransaction::GetTransactionSummary() const
{
	return FString::Printf(TEXT("Paint %i actor(s)"), ModifiedBricks.Num());
}

void UZenbPaintTransaction::Undo()
{
	for (const FZenbPaintDelta& Item : ModifiedBricks)
	{
		Item.Actor->SetColor(Item.OldColor);
	}
	GetPawn()->SetWorldDirtyForAutosave();
}

void UZenbPaintTransaction::Redo()
{
	for (const FZenbPaintDelta& Item : ModifiedBricks)
	{
		Item.Actor->SetColor(Item.NewColor);
	}
	GetPawn()->SetWorldDirtyForAutosave();
}
//~ End UZenbTransaction Interface
