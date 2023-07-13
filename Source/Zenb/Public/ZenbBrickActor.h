// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "ZenbBrickActor.generated.h"

UCLASS()
class ZENB_API AZenbBrickActor : public AStaticMeshActor
{
	GENERATED_BODY()

public:

	AZenbBrickActor();

public: // Functions

	/** Get local space top stud location. */
	virtual FVector GetNearestTopStud(FVector InWorldLocation) const;
	virtual FVector GetNearestTopStudWorldSpace(FVector InWorldLocation) const;
	/** Get local space bottom stud location. */
	virtual FVector GetNearestBottomStud(FVector InWorldLocation) const;
	virtual FVector GetNearestBottomStudWorldSpace(FVector InWorldLocation) const;

	bool CanEverSimulatePhysics() const { return bCanEverSimulatePhysics && bIsExplosionModeDuplicate; }
	void SetIsHovered(bool bInHovered);
	bool CanBeSelected() const { return bCanBeSelected && !(bIsHiddenForDeletion || bIsHiddenForSpawn); }
	bool ZenbIsSelected() const { return bIsSelected; }
	void SetIsSelected(bool bInSelected);
	void SetIsActive(bool bInActive);

	FLinearColor GetColor() const;
	virtual void SetColor(FLinearColor InColor);
	virtual void UpdateLinks(bool bAdd);
	virtual int32 GetNumStuds() const;
	void UpdateMeshFromDimensions();
	
	bool ShouldExcludeFromSave() const { return bIsExplosionModeDuplicate || bIsHiddenForDeletion || bIsHiddenForSpawn; }
	
	/** Called when brick becomes unstable. */
	void DetachAndSimulatePhysics();

	void RemoveStudReference(class AZenbBrickActor* InActor);

public: // Interfaces

	//~ Begin UObject Interface
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif // WITH_EDITOR
	//~ End UObject Interface

public: // Properties

	UPROPERTY(EditAnywhere)
	FIntVector Dimensions;

	UPROPERTY(EditAnywhere)
	TMap<FIntVector, TObjectPtr<UStaticMesh>> Meshes;

	/**
	 * Flag for bricks duplicated into explosion mode.
	 * (Source actors are hidden in explosion mode.)
	 */
	UPROPERTY(Transient)
	uint8 bIsExplosionModeDuplicate:1;
	
	/**
	 * Flag for bricks that have been "deleted", but not destroyed until forgotten in undo history.
	 * Pending-deletion bricks are hidden.
	 */
	UPROPERTY(Transient)
	uint8 bIsHiddenForDeletion:1;
	
	/**
	 * Flag for bricks that have been spawned but the spawning was undone.
	 * Undo-spawned bricks are hidden.
	 */
	UPROPERTY(Transient)
	uint8 bIsHiddenForSpawn:1;

	/** True if brick has physics simulation enabled in explosion mode. */
	UPROPERTY(Transient)
	uint8 bIsExploded:1;

	UPROPERTY(Transient)
	TMap<TObjectPtr<class AZenbBrickActor>, int32> StudTracker;

protected: // Functions

	virtual void UpdateStencilValue();

protected: // Properties

	UPROPERTY(EditAnywhere)
	uint8 bCanEverSimulatePhysics:1;

private: // Properties

	UPROPERTY(EditAnywhere)
	uint8 bCanBeSelected:1;

	UPROPERTY(Transient)
	uint8 bIsHovered:1;
	
	UPROPERTY(Transient)
	uint8 bIsSelected:1;

	UPROPERTY(Transient)
	uint8 bIsActive:1;

	UPROPERTY(Transient)
	FLinearColor BrickColor;
};
