// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTransaction.h"
#include "ZenbSpawnTransaction.generated.h"

UCLASS()
class ZENB_API UZenbSpawnTransaction : public UZenbTransaction
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTransaction Interface
	FString GetTransactionSummary() const override;
	void Undo() override;
	void Redo() override;
	void Forget() override;
	//~ End UZenbTransaction Interface

public: // Properties

	UPROPERTY()
	TArray<TObjectPtr<class AZenbBrickActor>> SourceActors;

	UPROPERTY()
	TArray<TObjectPtr<class AZenbBrickActor>> Actors;

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> OldActiveActor;
	
	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> NewActiveActor;
};
