// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/DefaultPawn.h"
#include "ZenbPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FColorSampledDelegate);

USTRUCT()
struct ZENB_API FZenbActorCopyInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<class AZenbBrickActor> ActorClass;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FQuat Rotation;

	UPROPERTY()
	FIntVector Dimensions;

	UPROPERTY()
	FLinearColor Color;
};

USTRUCT()
struct ZENB_API FZenbCopyDetails
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FZenbActorCopyInfo> Actors;

	UPROPERTY()
	FVector BoundsOrigin;

	UPROPERTY()
	FVector BoundsExtent;
};

UCLASS()
class ZENB_API AZenbPawn : public ADefaultPawn
{
	GENERATED_BODY()

	AZenbPawn(const FObjectInitializer& ObjectInitializer);

public: // Functions

	class AZenbPlayerController* GetZenbPlayerController();
	class UZenbTool* GetActiveTool() const { return ActiveTool; };
	class UZenbTool* FindToolByClass(TSubclassOf<class UZenbTool> InClass);
	void SetActiveTool(class UZenbTool* InTool);
	void RequestChangeTool(class UZenbTool* InTool);

	class AZenbBrickActor* GetActiveActor() const { return PrivateActiveActor; }
	void SetActiveActor(class AZenbBrickActor* InActor);
	bool DoesSelectionContain(class AZenbBrickActor* InActor) const;
	void ToggleSelectActor(class AZenbBrickActor* InActor);
	void SelectActor(class AZenbBrickActor* InActor);
	void DeselectActor(class AZenbBrickActor* InActor);
	void ClearSelection();
	/** Undo-able version of clear selection. */
	void RequestClearSelection();
	const TSet<TObjectPtr<class AZenbBrickActor>>& GetSelection() const { return SelectedActors; }

	class UZenbTransactionManager* GetTransactionManager() const { return TransactionManager; }
	void SetWorldDirtyForAutosave();

	void SetHoveredActor(class AZenbBrickActor* NewHoveredActor);
	float ProjectRayOntoRay(FVector Origin0, FVector Direction0, FVector Origin1, FVector Direction1);

public: // Interfaces

	//~ Begin AActor Interface
	void Tick(float DeltaSeconds) override;
	void BecomeViewTarget(APlayerController* PC) override;
	//~ End AActor Interface

public: // Properties

	UPROPERTY(Transient)
	FLinearColor ActiveColor;

	UPROPERTY(Transient)
	FColorSampledDelegate OnColorSampled;

	UPROPERTY()
	FColorSampledDelegate OnActiveToolChanged;

protected: // Interfaces

	//~ Begin APawn Interface
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	//~ End APawn Interface

private: // Functions

	FVector GetBestAlignedLocalVector(FVector InControlVector);
	void MoveAlongLocalVector(FVector InLocalVector);
	void GrowAlongLocalVector(FVector InLocalVector);
	void ShrinkAlongLocalVector(FVector InLocalVector);
	void RotateGrabbed(float InYawDelta);

	void OnMoveForwardInput(float InAxisValue);
	void OnMoveRightInput(float InAxisValue);
	void OnMoveSpeedInput(float InAxisValue);
	void OnPitchInput(float InAxisValue);
	void OnYawInput(float InAxisValue);

	void OnMoveLeftPressed();
	void OnMoveRightPressed();
	void OnMoveUpPressed();
	void OnMoveDownPressed();
	void OnGrowLeftPressed();
	void OnGrowRightPressed();
	void OnGrowUpPressed();
	void OnGrowDownPressed();
	void OnShrinkLeftPressed();
	void OnShrinkRightPressed();
	void OnShrinkUpPressed();
	void OnShrinkDownPressed();

	void OnSelectPressed();
	void OnBoxSelectPressed();
	void OnMovePressed();
	void OnResizePressed();
	void OnPaintPressed();
	void OnSampleColorPressed();
	void OnSampleColorReleased();
	void OnCopyPressed();
	void OnPastePressed();
	void OnDuplicatePressed();
	void OnRotatePressed();
	void OnRotateCounterclockwisePressed();
	void OnDeletePressed();
	void OnSelectAllPressed();
	void OnExplodePressed();
	void OnLeftMousePressed();
	void OnLeftMouseReleased();
	void OnRightMousePressed();
	void OnRightMouseReleased();

	void OnSavePressed();
	void OnUndoPressed();
	void OnRedoPressed();

private: // Properties

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<class UZenbTool>> ToolClasses;
	
	UPROPERTY()
	TArray<TObjectPtr<class UZenbTool>> ToolInstances;

	UPROPERTY()
	TObjectPtr<class UUserWidget> ToolWidgetInstance;

	UPROPERTY()
	TObjectPtr<class UZenbTool> ActiveTool;

	UPROPERTY()
	TObjectPtr<class AZenbBrickActor> HoveredActor;

	/** Speed control while flying. */
	UPROPERTY()
	float MoveSpeedFactor;

	/** Whether right mouse button is held to fly around with FPS controls.  */
	UPROPERTY()
	uint8 bIsFlying:1;

	UPROPERTY(Transient)
	TObjectPtr<class UZenbTransactionManager> TransactionManager;

	UPROPERTY(Transient)
	TSet<TObjectPtr<class AZenbBrickActor>> SelectedActors;

	UPROPERTY(Transient)
	TObjectPtr<class AZenbBrickActor> PrivateActiveActor;

	UPROPERTY()
	FZenbCopyDetails CopyDetails;

	UPROPERTY(Transient)
	uint8 bIsLeftMouseHeld:1;
};
