// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTool.h"
#include "ZenbSelectTool.generated.h"

UCLASS()
class ZENB_API UZenbSelectTool : public UZenbTool
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTool Interface
	void Tick() override;
	void OnLeftMousePressed() override;
	void OnLeftMouseReleased() override;
	bool ShouldHoverActor(AZenbBrickActor* InActor) const override;
	//~ End UZenbTool Interface

private: // Properties

	UPROPERTY(Transient)
	uint8 bIsSelecting:1;

	/** Copy of selection prior to painting. */
	UPROPERTY(Transient)
	TSet<TObjectPtr<class AZenbBrickActor>> InitialSelection;

	/** Copy of active selection prior to painting. */
	UPROPERTY(Transient)
	TObjectPtr<class AZenbBrickActor> InitialActiveActor;

	/** Actors which were not present in InitialSelection. */
	UPROPERTY(Transient)
	TSet<TObjectPtr<class AZenbBrickActor>> AddedActors;
	
	/** Actors which were present in InitialSelection. */
	UPROPERTY(Transient)
	TSet<TObjectPtr<class AZenbBrickActor>> RemovedActors;
};
