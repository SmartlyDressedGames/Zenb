// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTransaction.h"
#include "ZenbDestroyTransaction.generated.h"

UCLASS()
class ZENB_API UZenbDestroyTransaction : public UZenbTransaction
{
	GENERATED_BODY()

public: // Functions

	void AddActor(class AZenbBrickActor* InActor);

public: // Interfaces

	//~ Begin UZenbTransaction Interface
	FString GetTransactionSummary() const override;
	void Undo() override;
	void Redo() override;
	void Forget() override;
	//~ End UZenbTransaction Interface

public: // Properties

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> OldActiveActor;

private: // Properties

	UPROPERTY()
	TArray<TObjectPtr<class AZenbBrickActor>> Actors;
};
