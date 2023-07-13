// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTool.h"
#include "ZenbBoxSelectTool.generated.h"

UCLASS()
class ZENB_API UZenbBoxSelectTool : public UZenbTool
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTool Interface
	void Tick() override;
	void OnLeftMousePressed() override;
	void OnLeftMouseReleased() override;
	bool ShouldHoverActorUnderMouse() const { return false; }
	//~ End UZenbTool Interface

public: // Properties

	UPROPERTY(Transient)
	uint8 bIsSelecting:1;

	UPROPERTY(Transient)
	FVector2D MouseDownScreenPosition;

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
