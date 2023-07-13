// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTransaction.h"
#include "ZenbPaintTransaction.generated.h"

USTRUCT()
struct ZENB_API FZenbPaintDelta
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> Actor;

	UPROPERTY()
	FLinearColor OldColor;
	
	UPROPERTY()
	FLinearColor NewColor;

	FZenbPaintDelta();
};

UCLASS()
class ZENB_API UZenbPaintTransaction : public UZenbTransaction
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
	TArray<FZenbPaintDelta> ModifiedBricks;
};
