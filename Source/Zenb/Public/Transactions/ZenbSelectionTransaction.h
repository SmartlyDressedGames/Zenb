// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTransaction.h"
#include "ZenbSelectionTransaction.generated.h"

UCLASS()
class ZENB_API UZenbSelectionTransaction : public UZenbTransaction
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTransaction Interface
	FString GetTransactionSummary() const override;
	void Undo() override;
	void Redo() override;
	//~ End UZenbTransaction Interface

public: // Properties

	/** Actors added to the selection by this transaction. */
	UPROPERTY()
	TSet<TObjectPtr<class AZenbBrickActor>> AddedActors;

	/** Actors removed from the selection by this transaction. */
	UPROPERTY()
	TSet<TObjectPtr<class AZenbBrickActor>> RemovedActors;

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> OldActiveActor;
	
	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> NewActiveActor;
};
