// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "ZenbTransactionManager.generated.h"

UCLASS()
class ZENB_API UZenbTransactionManager : public UObject
{
	GENERATED_BODY()

public: // Functions
	
	class AZenbPawn* GetPawn();

	void Undo();
	void Redo();

	template<class T>
	T* NewTransaction()
	{
		return NewObject<T>(this, NAME_None, EObjectFlags::RF_Transient);
	};
	void AddTransaction(class UZenbTransaction* InTransaction);

public: // Properties

	UPROPERTY()
	TArray<TObjectPtr<class UZenbTransaction>> UndoStack;
	
	UPROPERTY()
	TArray<TObjectPtr<class UZenbTransaction>> RedoStack;
};
