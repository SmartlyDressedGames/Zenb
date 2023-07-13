// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Transactions/ZenbTransaction.h"
#include "ZenbPawn.h"

AZenbPawn* UZenbTransaction::GetPawn() const
{
	return Cast<AZenbPawn>(GetOuter()->GetOuter());
}

FString UZenbTransaction::GetTransactionSummary() const
{
	return TEXT("Unknown");
}
