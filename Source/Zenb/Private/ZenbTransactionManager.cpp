// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbTransactionManager.h"
#include "ZenbLogChannels.h"
#include "ZenbPawn.h"
#include "Transactions/ZenbTransaction.h"
#include "ZenbPlayerController.h"

#define LOCTEXT_NAMESPACE "Zenb"

#if WITH_EDITOR
static int32 DebugTransactionUndoRedo = false;
static FAutoConsoleVariableRef CVarDebugTransactionUndoRedo(TEXT("z.DebugTransactionUndoRedo"), DebugTransactionUndoRedo,
	TEXT("Immediately undo and redo each transaction to ensure state is preserved.\n")
	TEXT("0: Disabled (default)\n")
	TEXT("1: Single\n")
	TEXT("2: All\n"));
#endif // WITH_EDITOR

static bool bLogTransactions = false;
static FAutoConsoleVariableRef CVarLogTransactions(TEXT("z.LogTransactions"), bLogTransactions,
	TEXT("Whether to log transaction add/remove/undo/redo."));

AZenbPawn* UZenbTransactionManager::GetPawn()
{
	return Cast<AZenbPawn>(GetOuter());
}

void UZenbTransactionManager::Undo()
{
	if (UndoStack.Num() > 0)
	{
		UZenbTransaction* Transaction = UndoStack.Pop(false);
		Transaction->Undo();
		Transaction->bInUndoStack = false;
		Transaction->bInRedoStack = true;
		RedoStack.Add(Transaction);
		UE_CLOG(bLogTransactions, LogZenb, Display, TEXT("Undo transaction %s (Undoable: %i Redoable: %i)"), *Transaction->GetTransactionSummary(), UndoStack.Num(), RedoStack.Num());
	}
	else
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot undo because queue is empty (Undoable: %i Redoable: %i)"), UndoStack.Num(), RedoStack.Num());
		GetPawn()->GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_UndoQueueEmpty", "Reached end of undo queue"));
	}
}

void UZenbTransactionManager::Redo()
{
	if (RedoStack.Num() > 0)
	{
		UZenbTransaction* Transaction = RedoStack.Pop(false);
		Transaction->Redo();
		Transaction->bInUndoStack = true;
		Transaction->bInRedoStack = false;
		UndoStack.Add(Transaction);
		UE_CLOG(bLogTransactions, LogZenb, Display, TEXT("Redo transaction %s (Undoable: %i Redoable: %i)"), *Transaction->GetTransactionSummary(), UndoStack.Num(), RedoStack.Num());
	}
	else
	{
		UE_LOG(LogZenb, Display, TEXT("Cannot redo because queue is empty (Undoable: %i Redoable: %i)"), UndoStack.Num(), RedoStack.Num());
		GetPawn()->GetZenbPlayerController()->PushTextNotification(LOCTEXT("Notification_RedoQueueEmpty", "Reached end of redo queue"));
	}
}

void UZenbTransactionManager::AddTransaction(class UZenbTransaction* InTransaction)
{
	if (!ensure(InTransaction))
		return;

	InTransaction->bInUndoStack = true;
	InTransaction->bInRedoStack = false;
	UndoStack.Add(InTransaction);
	UE_CLOG(bLogTransactions, LogZenb, Display, TEXT("Commit transaction %s"), *InTransaction->GetTransactionSummary());

	if (UndoStack.Num() > 50)
	{
		UZenbTransaction* OldestTransaction = UndoStack[0];
		UE_CLOG(bLogTransactions, LogZenb, Display, TEXT("Forgot oldest transaction %s"), *OldestTransaction->GetTransactionSummary());
		OldestTransaction->Forget();
		OldestTransaction->MarkAsGarbage();
		UndoStack.RemoveAt(0);
	}

	UE_CLOG(bLogTransactions && (RedoStack.Num() > 0), LogZenb, Display, TEXT("Forgetting %i redo transaction(s):"), RedoStack.Num());
	for (UZenbTransaction* Transaction : RedoStack)
	{
		UE_CLOG(bLogTransactions && (RedoStack.Num() > 0), LogZenb, Display, TEXT("Forgot redo transaction %s"), *Transaction->GetTransactionSummary());
		Transaction->Forget();
		Transaction->MarkAsGarbage();
	}
	RedoStack.Reset();

#if WITH_EDITOR
	if (DebugTransactionUndoRedo == 1)
	{
		Undo();
		Redo();
	}
	else if (DebugTransactionUndoRedo > 1)
	{
		UE_LOG(LogZenb, Warning, TEXT("Undoing/redoing %i transactions"), UndoStack.Num());
		while (UndoStack.Num() > 0)
		{
			Undo();
		}
		while (RedoStack.Num() > 0)
		{
			Redo();
		}
	}
#endif // WITH_EDITOR

	check(UndoStack.Num() > 0);
	check(RedoStack.Num() < 1);
}

#undef LOCTEXT_NAMESPACE
