// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTransaction.h"
#include "ZenbChangeToolTransaction.generated.h"

UCLASS()
class ZENB_API UZenbChangeToolTransaction : public UZenbTransaction
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTransaction Interface
	FString GetTransactionSummary() const override;
	void Undo() override;
	void Redo() override;
	//~ End UZenbTransaction Interface

public: // Properties

	UPROPERTY()
	TObjectPtr<class UZenbTool> OldTool;

	UPROPERTY()
	TObjectPtr<class UZenbTool> NewTool;

	/** Actors removed from the selection by this transaction. */
	UPROPERTY()
	TSet<TObjectPtr<class AZenbBrickActor>> RemovedActors;

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> OldActiveActor;
};
