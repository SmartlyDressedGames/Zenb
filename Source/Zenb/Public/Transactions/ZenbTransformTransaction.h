// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTransaction.h"
#include "ZenbTransformTransaction.generated.h"

USTRUCT()
struct ZENB_API FZenbTransformDelta
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> Actor;

	UPROPERTY()
	FVector OldLocation;
	
	UPROPERTY()
	FQuat OldQuat;
	
	UPROPERTY()
	FVector NewLocation;
	
	UPROPERTY()
	FQuat NewQuat;

	UPROPERTY()
	FIntVector OldDimensions;

	UPROPERTY()
	FIntVector NewDimensions;

	FZenbTransformDelta();
};

UCLASS()
class ZENB_API UZenbTransformTransaction : public UZenbTransaction
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
	TArray<FZenbTransformDelta> ModifiedBricks;
};
