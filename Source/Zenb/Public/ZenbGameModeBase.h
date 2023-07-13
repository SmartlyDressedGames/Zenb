// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZenbGameModeBase.generated.h"

UCLASS()
class ZENB_API AZenbGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AZenbGameModeBase();

public: // Functions

	/** Initially saved world to load later, but duplicating actors preserves undo history.*/
	void BeginExplosionMode();
	void EndExplosionMode();
	void SetDirtyForAutosave();
	void RequestSave();
	void RequestSave(const FGuid& InGuid);
	const TArray<TObjectPtr<class AZenbBrickActor>>& GetExplosionActors() const { return ExplosionActors; }

public: // Interfaces

	//~ Begin AActor Interface
	void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin AGameModeBase Interface
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	//~ End AGameModeBase Interface

private: // Functions

	bool LoadWorldFromMemory(const TArray<uint8>& InSaveData);
	void SaveWorldToMemory(TArray<uint8>& SaveData);
	void OnAutosaveTimerFired();

private: // Properties

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AZenbBrickActor> BrickClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AZenbBrickActor> BaseplateClass;

	UPROPERTY(Transient)
	uint8 bInExplosionMode:1;

	UPROPERTY(Transient)
	TArray<TObjectPtr<class AZenbBrickActor>> SourceActors;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<class AZenbBrickActor>> ExplosionActors;

	UPROPERTY(Transient)
	FTimerHandle AutosaveTimer;

	UPROPERTY(Transient)
	uint8 bAreBricksDirty:1;

	UPROPERTY()
	TObjectPtr<class APlayerStart> LoadedPlayerStart;

	UPROPERTY(Transient)
	FGuid CurrentSaveGuid;
};
