// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbLinkSubsystem.h"
#include "DrawDebugHelpers.h"
#include "ZenbBrickActor.h"
#include "ZenbBaseplate.h"
#include "EngineUtils.h"
#include "ZenbGameModeBase.h"

#if ENABLE_DRAW_DEBUG
static bool bDrawStudLinks = false;
static FAutoConsoleVariableRef CVarDrawStudLinks(TEXT("z.DrawStudLinks"), bDrawStudLinks, TEXT(""));
static bool bDrawActorLinks = false;
static FAutoConsoleVariableRef CVarDrawActorLinks(TEXT("z.DrawActorLinks"), bDrawActorLinks, TEXT(""));
static bool bDrawActorsWithConnectionsRecentlyLost = false;
static FAutoConsoleVariableRef CVarDrawActorsWithConnectionsRecentlyLost(TEXT("z.DrawActorsWithConnectionsRecentlyLost"), bDrawActorsWithConnectionsRecentlyLost, TEXT(""));
#endif // ENABLE_DRAW_DEBUG

// Public Functions:

void UZenbLinkSubsystem::Reset()
{
	Studs.Reset();
}

void UZenbLinkSubsystem::LinkStud(AZenbBrickActor* InActor, const FVector& InLocation, bool bAdd)
{
	int32 Index = IndexOfStud(InLocation);
	if (Index != INDEX_NONE)
	{
		FZenbStud& Stud = Studs[Index];
		if (bAdd)
		{
			for (AZenbBrickActor* OtherActor : Stud.Actors)
			{
				++InActor->StudTracker.FindOrAdd(OtherActor);
				++OtherActor->StudTracker.FindOrAdd(InActor);
			}

			Stud.Actors.Add(InActor);
		}
		else
		{
			Stud.Actors.RemoveSwap(InActor);
			
			ActorsWithStudConnectionsRecentlyLost.Add(InActor);
			for (AZenbBrickActor* OtherActor : Stud.Actors)
			{
				InActor->RemoveStudReference(OtherActor);
				OtherActor->RemoveStudReference(InActor);
				ActorsWithStudConnectionsRecentlyLost.Add(OtherActor);
			}
			
			if (Stud.Actors.Num() < 1)
			{
				Studs.RemoveAtSwap(Index);
			}
		}
	}
	else if (bAdd)
	{
		FZenbStud& NewLink = Studs.AddDefaulted_GetRef();
		NewLink.Location = InLocation;
		NewLink.Actors.Add(InActor);
	}
}

void UZenbLinkSubsystem::PostLinkAllStuds(AZenbGameModeBase* InGameMode)
{
	check(InGameMode);
	UWorld* World = GetWorld();

	TSet<AZenbBrickActor*> ActorsConnectedToBaseplate;
	TSet<AZenbBrickActor*> NextActorsToCheck;
	for (TActorIterator<AZenbBaseplate> Iter(World); Iter; ++Iter)
	{
		AZenbBaseplate* Baseplate = *Iter;
		ActorsConnectedToBaseplate.Add(Baseplate);
		NextActorsToCheck.Add(Baseplate);
	}

	TSet<AZenbBrickActor*> ActorsToCheck;
	while (NextActorsToCheck.Num() > 0)
	{
		ActorsToCheck.Reset();
		Swap(ActorsToCheck, NextActorsToCheck);

		for (AZenbBrickActor* Actor : ActorsToCheck)
		{
			for (const auto& Pair : Actor->StudTracker)
			{
				bool bWasAlreadyInSet;
				ActorsConnectedToBaseplate.Add(Pair.Key, &bWasAlreadyInSet);
				if (!bWasAlreadyInSet)
				{
					NextActorsToCheck.Add(Pair.Key);
				}
			}
		}
	}

	for (AZenbBrickActor* Actor : InGameMode->GetExplosionActors())
	{
		check(Actor);
		if (Actor->CanEverSimulatePhysics() && !ActorsConnectedToBaseplate.Contains(Actor))
		{
			Actor->DetachAndSimulatePhysics();
		}
	}
}

// Public Interfaces:

//~ Begin FTickableObjectBase Interface
void UZenbLinkSubsystem::Tick(float DeltaTime)
{
	UWorld* World = GetWorld();

#if ENABLE_DRAW_DEBUG
	if (bDrawStudLinks)
	{
		for (const FZenbStud& Stud : Studs)
		{
			DrawDebugPoint(World, Stud.Location, 8.0f, Stud.Actors.Num() > 1 ? FColor::Green : FColor::Red, /*bPersisent*/false, /*LifeTime*/ -1.f, SDPG_Foreground);
		}
	}

	if (bDrawActorLinks)
	{
		for (TActorIterator<AZenbBrickActor> Iter(World); Iter; ++Iter)
		{
			if (!Iter->bIsExplosionModeDuplicate)
				continue;

			DrawDebugPoint(World, Iter->GetActorLocation(), 8.0f, Iter->StudTracker.Num() > 0 ? FColor::Green : FColor::Red, /*bPersisent*/false, /*LifeTime*/ -1.f, SDPG_Foreground);
		}
	}
#endif // ENABLE_DRAW_DEBUG

	StabilityTimer += DeltaTime;
	if (StabilityTimer < 0.1f)
	{
		return;
	}
	StabilityTimer = 0.0f;

	if (ActorsWithStudConnectionsRecentlyLost.Num() > 0)
	{
		for (AZenbBrickActor* Actor : ActorsWithStudConnectionsRecentlyLost)
		{
			if (Actor == nullptr || Actor->bIsExploded || !Actor->CanEverSimulatePhysics())
			{
				// Maybe got destroyed?
				continue;
			}

#if ENABLE_DRAW_DEBUG
			if (bDrawActorsWithConnectionsRecentlyLost)
			{
				FBox Box = Actor->GetComponentsBoundingBox(false);
				DrawDebugBox(World, Box.GetCenter(), Box.GetExtent(), FColor::Red, /*bPersisent*/false, /*LifeTime*/ 5.f, SDPG_World);
			}
#endif // ENABLE_DRAW_DEBUG
			
			if (!IsActorConnectedToBaseplate(Actor))
			{
				UnstableActors.Add(Actor);
			}
		}

		ActorsWithStudConnectionsRecentlyLost.Reset();
	}

	if (UnstableActors.Num() > 0)
	{
		for (AZenbBrickActor* Actor : UnstableActors)
		{
			Actor->DetachAndSimulatePhysics();
			if (UPrimitiveComponent* RootComponent = Cast<UPrimitiveComponent>(Actor->GetRootComponent()))
			{
				RootComponent->AddAngularImpulseInRadians(FVector(FMath::RandRange(-15.f, 15.f), FMath::RandRange(-15.f, 15.f), FMath::RandRange(-15.f, 15.f)));
			}
		}
		UnstableActors.Reset();
	}
}

TStatId UZenbLinkSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UZenbLinkSubsystem, STATGROUP_Tickables);
}
//~ End FTickableObjectBase Interface

// Private Functions:

int32 UZenbLinkSubsystem::IndexOfStud(const FVector& InLocation)
{
	return Studs.IndexOfByPredicate([=](const FZenbStud& Link)
	{
		return Link.Location.Equals(InLocation);
	});
}

bool UZenbLinkSubsystem::IsActorConnectedToBaseplate(AZenbBrickActor* InActor)
{
	check(InActor);
	TSet<AZenbBrickActor*> CheckedActors;
	CheckedActors.Add(InActor);
	TSet<AZenbBrickActor*> NextActorsToCheck;
	NextActorsToCheck.Add(InActor);

	TSet<AZenbBrickActor*> ActorsToCheck;
	while (NextActorsToCheck.Num() > 0)
	{
		ActorsToCheck.Reset();
		Swap(ActorsToCheck, NextActorsToCheck);

		for (AZenbBrickActor* Actor : ActorsToCheck)
		{
			for (const auto& Pair : Actor->StudTracker)
			{
				if (Pair.Key->IsA<AZenbBaseplate>())
				{
					return true;
				}

				bool bWasAlreadyInSet;
				CheckedActors.Add(Pair.Key, &bWasAlreadyInSet);
				if (!bWasAlreadyInSet)
				{
					NextActorsToCheck.Add(Pair.Key);
				}
			}
		}
	}

	return false;
}
