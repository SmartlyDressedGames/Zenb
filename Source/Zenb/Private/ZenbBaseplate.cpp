// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbBaseplate.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ZenbLinkSubsystem.h"

AZenbBaseplate::AZenbBaseplate() : Super()
{
	StudsComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Studs"));
	StudsComponent->SetAbsolute(true, true, true);
	StudsComponent->SetupAttachment(RootComponent);
	StudsComponent->bRenderCustomDepth = false;
	StudsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bCanEverSimulatePhysics = false;
	GridResolution = 50;
}

// Public Interfaces:

//~ Begin AActor Interface
void AZenbBaseplate::BeginPlay()
{
	Super::BeginPlay();

	StudsComponent->PreAllocateInstancesMemory(GridResolution * GridResolution);
	const float HalfGridResolution = GridResolution * 0.5f;
	for (int32 x = 0; x < GridResolution; ++x)
	{
		for (int32 y = 0; y < GridResolution; ++y)
		{
			FVector InstanceLocation((x + 0.5f - HalfGridResolution) * 200.0f, (y + 0.5f - HalfGridResolution) * 200.0f, 0.0f);
			FTransform InstanceTransform(InstanceLocation);
			StudsComponent->AddInstance(InstanceTransform, false);
		}
	}

	GetStaticMeshComponent()->SetRelativeScale3D(FVector(GridResolution, GridResolution, 1.f));
}
//~ End AActor Interface

//~ Begin AZenbBrickActor Interface
FVector AZenbBaseplate::GetNearestTopStud(FVector InWorldLocation) const
{
	FVector RelativeLocation = GetActorTransform().InverseTransformPositionNoScale(InWorldLocation);
	RelativeLocation.X = FMath::GridSnap(RelativeLocation.X - 10.f, 20.f) + 10.f;
	RelativeLocation.Y = FMath::GridSnap(RelativeLocation.Y - 10.f, 20.f) + 10.f;
	RelativeLocation.Z = 0.0f;
	return RelativeLocation;
}

FVector AZenbBaseplate::GetNearestTopStudWorldSpace(FVector InWorldLocation) const
{
	return GetActorTransform().TransformPositionNoScale(GetNearestTopStud(InWorldLocation));
}

FVector AZenbBaseplate::GetNearestBottomStud(FVector InWorldLocation) const
{
	FVector RelativeLocation = GetActorTransform().InverseTransformPositionNoScale(InWorldLocation);
	RelativeLocation.X = FMath::GridSnap(RelativeLocation.X - 10.f, 20.f) + 10.f;
	RelativeLocation.Y = FMath::GridSnap(RelativeLocation.Y - 10.f, 20.f) + 10.f;
	RelativeLocation.Z = 0.0f;
	return RelativeLocation;
}

FVector AZenbBaseplate::GetNearestBottomStudWorldSpace(FVector InWorldLocation) const
{
	return GetActorTransform().TransformPositionNoScale(GetNearestBottomStud(InWorldLocation));
}

void AZenbBaseplate::SetColor(FLinearColor InColor)
{
	Super::SetColor(InColor);
	StudsComponent->CreateDynamicMaterialInstance(0)->SetVectorParameterValue(TEXT("Color"), InColor);
}

void AZenbBaseplate::UpdateLinks(bool bAdd)
{
	UZenbLinkSubsystem* Subsystem = GetWorld()->GetSubsystem<UZenbLinkSubsystem>();
	const FTransform& Transform = GetActorTransform();
	for (int32 StudX = 0; StudX < GridResolution * 10; ++StudX)
	{
		for (int32 StudY = 0; StudY < GridResolution * 10; ++StudY)
		{
			FVector LocalLocation = FVector(StudX * 20.f - GridResolution * 100.f + 10.f, StudY * 20.f - GridResolution * 100.f + 10.f, 0.f);
			FVector WorldLocation = Transform.TransformPositionNoScale(LocalLocation);
			Subsystem->LinkStud(this, WorldLocation, bAdd);
		}
	}
}

int32 AZenbBaseplate::GetNumStuds() const
{
	return (GridResolution * 10) * (GridResolution * 10);
}
//~ End AZenbBrickActor Interface

// Protected Interfaces:

//~ Begin AZenbBrickActor Interface
void AZenbBaseplate::UpdateStencilValue()
{
	Super::UpdateStencilValue();
	StudsComponent->SetCustomDepthStencilValue(GetStaticMeshComponent()->CustomDepthStencilValue);
	StudsComponent->SetRenderCustomDepth(GetStaticMeshComponent()->bRenderCustomDepth);
}
//~ End AZenbBrickActor Interface
