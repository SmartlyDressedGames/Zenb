// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "ZenbLinkSubsystem.generated.h"

USTRUCT()
struct FZenbStud
{
	GENERATED_BODY()

public: // Properties

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	TArray<TObjectPtr<class AZenbBrickActor>> Actors;
};

UCLASS()
class UZenbLinkSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public: // Functions

	void Reset();
	void LinkStud(class AZenbBrickActor* InActor, const FVector& InLocation, bool bAdd);
	
	/** Called when entering explosion mode and all bricks have been registered. */
	void PostLinkAllStuds(class AZenbGameModeBase* InGameMode);

public: // Interfaces

	//~ Begin FTickableObjectBase Interface
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override;
	//~ End FTickableObjectBase Interface

private: // Functions

	int32 IndexOfStud(const FVector& InLocation);
	bool IsActorConnectedToBaseplate(class AZenbBrickActor* InActor);

private: // Properties

	UPROPERTY(Transient)
	TArray<FZenbStud> Studs;

	UPROPERTY(Transient)
	TSet<TObjectPtr<class AZenbBrickActor>> ActorsWithStudConnectionsRecentlyLost;
	
	UPROPERTY(Transient)
	TSet<TObjectPtr<class AZenbBrickActor>> UnstableActors;

	UPROPERTY(Transient)
	float StabilityTimer;
};
