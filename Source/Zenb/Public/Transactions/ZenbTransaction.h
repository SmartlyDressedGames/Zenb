// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "ZenbTransaction.generated.h"

UCLASS()
class ZENB_API UZenbTransaction : public UObject
{
	GENERATED_BODY()

public: // Functions

	class AZenbPawn* GetPawn() const;

	virtual FString GetTransactionSummary() const;
	virtual void Undo() {}
	virtual void Redo() {}
	virtual void Forget() {}

public: // Properties

	UPROPERTY(Transient)
	uint8 bInUndoStack:1;
	
	UPROPERTY(Transient)
	uint8 bInRedoStack:1;
};
