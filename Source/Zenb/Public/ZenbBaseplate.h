// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbBrickActor.h"
#include "ZenbBaseplate.generated.h"

UCLASS()
class ZENB_API AZenbBaseplate : public AZenbBrickActor
{
	GENERATED_BODY()

public:

	AZenbBaseplate();

public: // Interfaces

	//~ Begin AActor Interface
	void BeginPlay() override;
	//~ End AActor Interface

	//~ Begin AZenbBrickActor Interface
	FVector GetNearestTopStud(FVector InWorldLocation) const override;
	FVector GetNearestTopStudWorldSpace(FVector InWorldLocation) const override;
	FVector GetNearestBottomStud(FVector InWorldLocation) const override;
	FVector GetNearestBottomStudWorldSpace(FVector InWorldLocation) const override;
	void SetColor(FLinearColor InColor) override;
	void UpdateLinks(bool bAdd) override;
	int32 GetNumStuds() const override;
	//~ End AZenbBrickActor Interface

protected: // Interfaces

	//~ Begin AZenbBrickActor Interface
	void UpdateStencilValue() override;
	//~ End AZenbBrickActor Interface

private: // Properties

	UPROPERTY(EditDefaultsOnly)
	int32 GridResolution;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UInstancedStaticMeshComponent> StudsComponent;
};
