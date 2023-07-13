// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbBrickActor.h"
#include "Components/StaticMeshComponent.h"
#include "ZenbLinkSubsystem.h"

AZenbBrickActor::AZenbBrickActor() : Super()
{
	bCanEverSimulatePhysics = true;
	bCanBeSelected = true;
	GetStaticMeshComponent()->bRenderCustomDepth = false;
}

// Public Functions:

FVector AZenbBrickActor::GetNearestTopStud(FVector InLocation) const
{
	FVector RelativeLocation = GetActorTransform().InverseTransformPosition(InLocation);
	int StudsX = Dimensions.X / 20;
	int StudsY = Dimensions.Y / 20;
	int HitStudX = FMath::FloorToInt((RelativeLocation.X + StudsX * 10.f) / 20.f);
	HitStudX = FMath::Clamp(HitStudX, 0, StudsX - 1);
	int HitStudY = FMath::FloorToInt((RelativeLocation.Y + StudsY * 10.f) / 20.f);
	HitStudY = FMath::Clamp(HitStudY, 0, StudsY - 1);
	RelativeLocation.X = StudsX * -10.f + HitStudX * 20.f + 10.f;
	RelativeLocation.Y = StudsY * -10.f + HitStudY * 20.f + 10.f;
	RelativeLocation.Z = Dimensions.Z * 0.5f;
	return RelativeLocation;
}

FVector AZenbBrickActor::GetNearestTopStudWorldSpace(FVector InWorldLocation) const
{
	return GetActorTransform().TransformPosition(GetNearestTopStud(InWorldLocation));
}

FVector AZenbBrickActor::GetNearestBottomStud(FVector InLocation) const
{
	FVector RelativeLocation = GetActorTransform().InverseTransformPosition(InLocation);
	int StudsX = Dimensions.X / 20;
	int StudsY = Dimensions.Y / 20;
	int HitStudX = FMath::FloorToInt((RelativeLocation.X + StudsX * 10.f) / 20.f);
	HitStudX = FMath::Clamp(HitStudX, 0, StudsX - 1);
	int HitStudY = FMath::FloorToInt((RelativeLocation.Y + StudsY * 10.f) / 20.f);
	HitStudY = FMath::Clamp(HitStudY, 0, StudsY - 1);
	RelativeLocation.X = StudsX * -10.f + HitStudX * 20.f + 10.f;
	RelativeLocation.Y = StudsY * -10.f + HitStudY * 20.f + 10.f;
	RelativeLocation.Z = Dimensions.Z * -0.5f;
	return RelativeLocation;
}

FVector AZenbBrickActor::GetNearestBottomStudWorldSpace(FVector InWorldLocation) const
{
	return GetActorTransform().TransformPosition(GetNearestBottomStud(InWorldLocation));
}

void AZenbBrickActor::SetIsHovered(bool bInHovered)
{
	if (bIsHovered != bInHovered)
	{
		bIsHovered = bInHovered;
		UpdateStencilValue();
	}
}

void AZenbBrickActor::SetIsSelected(bool bInSelected)
{
	if (bIsSelected != bInSelected)
	{
		bIsSelected = bInSelected;
		UpdateStencilValue();
	}
}

void AZenbBrickActor::SetIsActive(bool bInActive)
{
	if (bIsActive != bInActive)
	{
		bIsActive = bInActive;
		UpdateStencilValue();
	}
}

FLinearColor AZenbBrickActor::GetColor() const
{
	return BrickColor;
}

void AZenbBrickActor::SetColor(FLinearColor InColor)
{
	BrickColor = InColor;
	BrickColor.A = 1.0f;
	GetStaticMeshComponent()->SetCustomPrimitiveDataVector3(0, FVector(BrickColor));
}

void AZenbBrickActor::UpdateLinks(bool bAdd)
{
	UZenbLinkSubsystem* Subsystem = GetWorld()->GetSubsystem<UZenbLinkSubsystem>();
	const FTransform& Transform = GetActorTransform();
	for (int32 StudX = 0; StudX < Dimensions.X; StudX += 20)
	{
		for (int32 StudY = 0; StudY < Dimensions.Y; StudY += 20)
		{
			FVector TopLocalLocation = FVector(StudX - Dimensions.X * 0.5f + 10.f, StudY - Dimensions.Y * 0.5f + 10.f, Dimensions.Z * 0.5f);
			FVector TopWorldLocation = Transform.TransformPositionNoScale(TopLocalLocation);
			FVector BottomLocalLocation = FVector(StudX - Dimensions.X * 0.5f + 10.f, StudY - Dimensions.Y * 0.5f + 10.f, Dimensions.Z * -0.5f);
			FVector BottomWorldLocation = Transform.TransformPositionNoScale(BottomLocalLocation);
			Subsystem->LinkStud(this, TopWorldLocation, bAdd);
			Subsystem->LinkStud(this, BottomWorldLocation, bAdd);
		}
	}
}

int32 AZenbBrickActor::GetNumStuds() const
{
	return (Dimensions.X / 20) * (Dimensions.Y / 20) * 2;
}

void AZenbBrickActor::UpdateMeshFromDimensions()
{
	if (TObjectPtr<class UStaticMesh>* NewMesh = Meshes.Find(Dimensions))
	{
		GetStaticMeshComponent()->SetStaticMesh(*NewMesh);
	}
}

void AZenbBrickActor::DetachAndSimulatePhysics()
{
	check(CanEverSimulatePhysics());
	if (bIsExploded)
		return;
	bIsExploded = true;

	if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(GetRootComponent()))
	{
		UpdateLinks(false);
		Prim->SetSimulatePhysics(true);
		SetLifeSpan(10.f);
	}
}

void AZenbBrickActor::RemoveStudReference(class AZenbBrickActor* InActor)
{
	check(InActor);
	if (int32* NumStuds = StudTracker.Find(InActor))
	{
		if (*NumStuds > 1)
		{
			--(*NumStuds);
		}
		else
		{
			int32 NumRemoved = StudTracker.Remove(InActor);
			check(NumRemoved == 1);
		}
	}
}

// Public Interfaces:

//~ Begin UObject Interface
#if WITH_EDITOR
void AZenbBrickActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.MemberProperty != nullptr && PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AZenbBrickActor, Dimensions))
	{
		UpdateMeshFromDimensions();
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR
//~ End UObject Interface

// Protected Functions:

void AZenbBrickActor::UpdateStencilValue()
{
	GetStaticMeshComponent()->SetCustomDepthStencilValue(bIsActive ? 4 : (bIsSelected ? 2 : (bIsHovered ? 1 : 0)));
	GetStaticMeshComponent()->SetCustomPrimitiveDataFloat(4, bIsSelected ? 1.0f : 0.0f);
	GetStaticMeshComponent()->SetRenderCustomDepth(bIsActive | bIsSelected | bIsHovered);
}
