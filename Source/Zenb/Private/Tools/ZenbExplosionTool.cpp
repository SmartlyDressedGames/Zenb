// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbExplosionTool.h"
#include "ZenbPawn.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "ZenbGameModeBase.h"
#include "ZenbBrickActor.h"

//~ Begin UZenbTool Interface
void UZenbExplosionTool::OnEquip()
{
	if (AZenbGameModeBase* GM = GetWorld()->GetAuthGameMode<AZenbGameModeBase>())
	{
		GM->BeginExplosionMode();
	}
}

void UZenbExplosionTool::OnDequip()
{
	if (AZenbGameModeBase* GM = GetWorld()->GetAuthGameMode<AZenbGameModeBase>())
	{
		GM->EndExplosionMode();
	}
}

void UZenbExplosionTool::OnLeftMousePressed()
{
	APlayerController* PC = Pawn->GetController<APlayerController>();
	if (!PC)
		return;

	FVector WorldLocation;
	FVector WorldDirection;
	if (!PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		return;

	UWorld* World = GetWorld();

	FVector RayStart = WorldLocation;
	FVector RayEnd = RayStart + WorldDirection * 10000.0f;
	FHitResult Hit;
	if (!World->LineTraceSingleByChannel(Hit, RayStart, RayEnd, ECC_Visibility))
	{
		return;
	}

	FVector ExplosionLocation = Hit.Location;

	float ExplosionRadius = 200.f;
	float ExplosionForce = 2000.f;

	TArray<FOverlapResult> Overlaps;
	World->OverlapMultiByObjectType(Overlaps, ExplosionLocation, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(ExplosionRadius));

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AZenbBrickActor* OverlapActor = Cast<AZenbBrickActor>(Overlap.OverlapObjectHandle.FetchActor());

		if (OverlapActor &&
			OverlapActor->CanEverSimulatePhysics() &&
			Overlap.Component.IsValid())
		{
			if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Overlap.Component.Get()))
			{
				FVector CenterOfMass = Prim->GetCenterOfMass();
				FVector RelativeToExplosion = CenterOfMass - ExplosionLocation;
				float DistanceFromExplosion = RelativeToExplosion.Size();
				if (DistanceFromExplosion < ExplosionRadius)
				{
					FVector ImpulseDirection = RelativeToExplosion / DistanceFromExplosion;
					float Scale = ExplosionForce * (1.f - DistanceFromExplosion / ExplosionRadius);
					FVector AngularImpulseDirection = FVector::CrossProduct(FVector::UpVector, ImpulseDirection);

					if (!OverlapActor->bIsExploded)
					{
						OverlapActor->DetachAndSimulatePhysics();
					}

					Prim->AddVelocityChangeImpulseAtLocation(ImpulseDirection * Scale, ExplosionLocation);
					Prim->AddAngularImpulseInRadians(AngularImpulseDirection * 5.f);
				}
			}
		}
	}
}
//~ End UZenbTool Interface
