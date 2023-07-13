// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbGameModeBase.h"
#include "ZenbLogChannels.h"
#include "EngineUtils.h"
#include "ZenbBrickActor.h"
#include "ZenbSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "ZenbBaseplate.h"
#include "GameFramework/PlayerStart.h"
#include "ZenbUserSettings.h"
#include "ZenbPlayerController.h"
#include "ZenbLinkSubsystem.h"

AZenbGameModeBase::AZenbGameModeBase() : Super()
{
	CurrentSaveGuid.Invalidate();
}

// Public Functions:

void AZenbGameModeBase::BeginExplosionMode()
{
	if (bInExplosionMode)
		return;
	bInExplosionMode = true;

	UZenbLinkSubsystem* Subsystem = GetWorld()->GetSubsystem<UZenbLinkSubsystem>();
	Subsystem->Reset();

	for (auto Iter = TActorIterator<AZenbBrickActor>(GetWorld()); Iter; ++Iter)
	{
		if (Iter->IsHidden())
		{
			// Hidden by undo/redo spawn/destroy, so should not be duplicated.
			continue;
		}

		SourceActors.Add(*Iter);
	}

	for (AZenbBrickActor* SourceActor : SourceActors)
	{
		check(!SourceActor->bIsExplosionModeDuplicate);
		check(!SourceActor->bIsHiddenForDeletion);
		check(!SourceActor->bIsHiddenForSpawn);
		FActorSpawnParameters SpawnParams;
		SpawnParams.Template = SourceActor;
		SpawnParams.ObjectFlags |= EObjectFlags::RF_Transient;
		AZenbBrickActor* DuplicateActor = GetWorld()->SpawnActor<AZenbBrickActor>(SourceActor->GetClass(), SpawnParams);
		DuplicateActor->SetIsActive(false);
		DuplicateActor->SetIsHovered(false);
		DuplicateActor->SetIsSelected(false);
		DuplicateActor->bIsExplosionModeDuplicate = true;
		DuplicateActor->UpdateLinks(true);
		ExplosionActors.Add(DuplicateActor);
		SourceActor->SetActorHiddenInGame(true);
		SourceActor->SetActorEnableCollision(false);
	}

	Subsystem->PostLinkAllStuds(this);
}

void AZenbGameModeBase::EndExplosionMode()
{
	if (!bInExplosionMode)
		return;
	bInExplosionMode = false;

	for (AZenbBrickActor* Actor : ExplosionActors)
	{
		if (!Actor)
		{
			// May have been destroyed by timer or fell out of map.
			continue;
		}

		check(Actor->bIsExplosionModeDuplicate);
		Actor->Destroy();
	}

	for (AZenbBrickActor* Actor : SourceActors)
	{
		check(!Actor->bIsExplosionModeDuplicate);
		check(!Actor->bIsHiddenForDeletion);
		check(!Actor->bIsHiddenForSpawn);
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
	}

	SourceActors.Reset();
	ExplosionActors.Reset();
}

void AZenbGameModeBase::SetDirtyForAutosave()
{
	if (!bAreBricksDirty)
	{
		bAreBricksDirty = true;
		UE_LOG(LogZenb, Display, TEXT("Marked Zenb world dirty for autosave"));
	}
}

void AZenbGameModeBase::RequestSave()
{
	if (!CurrentSaveGuid.IsValid())
	{
		if (AZenbPlayerController* PC = Cast<AZenbPlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			PC->PushTextNotification(NSLOCTEXT("Zenb", "Notification_UntitledSave", "Please enter a save name in the Save As menu"));
		}
		return;
	}

	RequestSave(CurrentSaveGuid);
}

void AZenbGameModeBase::RequestSave(const FGuid& InGuid)
{
	if (bInExplosionMode)
		return;

	if (!ensure(InGuid.IsValid()))
		return;

	CurrentSaveGuid = InGuid;
	const FString SlotName = CurrentSaveGuid.ToString();
	UE_LOG(LogZenb, Display, TEXT("Saving... (%s)"), *SlotName);
	TArray<uint8> SaveData;
	SaveWorldToMemory(SaveData);
	if (SaveData.Num() > 0)
	{
		UGameplayStatics::SaveDataToSlot(SaveData, SlotName, 0);
	}
	UE_LOG(LogZenb, Display, TEXT("Saved! (%s)"), *SlotName);
	if (AZenbPlayerController* PC = Cast<AZenbPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->PushTextNotification(NSLOCTEXT("Zenb", "Notification_Saved", "Saved!"));
	}
}

// Public Interfaces:

//~ Begin AActor Interface
void AZenbGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(AutosaveTimer, this, &AZenbGameModeBase::OnAutosaveTimerFired, 60.f, true);
}
//~ End AActor Interface

//~ Begin AGameModeBase Interface
void AZenbGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	
	FString SaveSlotOption = UGameplayStatics::ParseOption(Options, TEXT("LoadSlotGuid"));
	if (SaveSlotOption.IsEmpty())
	{
		const UZenbUserSettings* Settings = UZenbUserSettings::GetZenbUserSettings();
		CurrentSaveGuid = Settings->MostRecentSaveGuid;
	}
	else
	{
		if (!FGuid::Parse(SaveSlotOption, CurrentSaveGuid))
		{
			UE_LOG(LogZenb, Warning, TEXT("Unable to parse LoadSlotGuid \"%s\""), *SaveSlotOption);
		}
	}

	bool bHasAnyBricks = false;

	if (CurrentSaveGuid.IsValid())
	{
		TArray<uint8> SaveData;
		const FString SlotName = CurrentSaveGuid.ToString();
		if (UGameplayStatics::LoadDataFromSlot(SaveData, SlotName, 0) && SaveData.Num() > 0)
		{
			bHasAnyBricks = LoadWorldFromMemory(SaveData);
		}
	}

	if (!bHasAnyBricks)
	{
		FTransform DefaultSpawnTransform(FRotator::ZeroRotator, FVector(0.f, 0.f, 12.f));
		AZenbBrickActor* DefaultBrick = GetWorld()->SpawnActor<AZenbBrickActor>(BrickClass, DefaultSpawnTransform);
		if (DefaultBrick)
		{
			DefaultBrick->Dimensions = FIntVector(40, 40, 24);
			DefaultBrick->UpdateMeshFromDimensions();
		}

		GetWorld()->SpawnActor<AZenbBrickActor>(BaseplateClass);
	}
}

AActor* AZenbGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	if (LoadedPlayerStart)
	{
		return LoadedPlayerStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}
//~ End AGameModeBase Interface

// Private Functions:

bool AZenbGameModeBase::LoadWorldFromMemory(const TArray<uint8>& InSaveData)
{
	bool bHasAnyBricks = false;
	FMemoryReader MemoryReader(InSaveData, /*bIsPersistent*/ true);

	FZenbSaveGame SaveGame;
	SaveGame.Read(MemoryReader);
	UE_LOG(LogZenb, Display, TEXT("File version: %i"), SaveGame.FileVersion);

	if (SaveGame.bHasViewState)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.ObjectFlags |= RF_Transient;
		LoadedPlayerStart = GetWorld()->SpawnActor<APlayerStart>(SaveGame.ViewLocation, SaveGame.ViewRotation, SpawnParameters);
	}

	for (const FZenbSavedBrick& SavedBrick : SaveGame.Bricks)
	{
		TSubclassOf<AZenbBrickActor> SpawnClass;
		if (SavedBrick.Class == EZenbSavedBrickClass::Brick)
		{
			SpawnClass = BrickClass;
		}
		else
		{
			SpawnClass = BaseplateClass;
		}

		FTransform SpawnTransform(SavedBrick.Rotation, SavedBrick.Location);
		AZenbBrickActor* NewBrick = GetWorld()->SpawnActor<AZenbBrickActor>(SpawnClass, SpawnTransform);
		if (NewBrick)
		{
			NewBrick->SetColor(SavedBrick.Color);
			NewBrick->Dimensions = SavedBrick.Dimensions;
			NewBrick->UpdateMeshFromDimensions();

			bHasAnyBricks = true;
		}
	}

	return bHasAnyBricks;
}

void AZenbGameModeBase::SaveWorldToMemory(TArray<uint8>& InSaveData)
{
	FZenbSaveGame SaveGame;

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		SaveGame.bHasViewState = true;
		PC->GetActorEyesViewPoint(SaveGame.ViewLocation, SaveGame.ViewRotation);
	}
	else
	{
		SaveGame.bHasViewState = false;
	}

	for (auto Iter = TActorIterator<AZenbBrickActor>(GetWorld()); Iter; ++Iter)
	{
		if (Iter->ShouldExcludeFromSave())
		{
			// e.g. hidden by undo/redo spawn/destroy, so should not be saved.
			continue;
		}

		FZenbSavedBrick& SavedBrick = SaveGame.Bricks.AddDefaulted_GetRef();

		if (Iter->IsA<AZenbBaseplate>())
		{
			SavedBrick.Class = EZenbSavedBrickClass::Baseplate;
		}
		else
		{
			SavedBrick.Class = EZenbSavedBrickClass::Brick;
		}

		SavedBrick.Location = Iter->GetActorLocation();
		SavedBrick.Rotation = Iter->GetActorQuat();
		SavedBrick.Dimensions = Iter->Dimensions;
		SavedBrick.Color = Iter->GetColor();
	}

	FMemoryWriter MemoryWriter(InSaveData, /*bIsPersistent*/ true);
	SaveGame.Write(MemoryWriter);
}

void AZenbGameModeBase::OnAutosaveTimerFired()
{
	if (!bInExplosionMode && bAreBricksDirty)
	{
		UE_LOG(LogZenb, Display, TEXT("Autosaving..."));
		bAreBricksDirty = false;
		TArray<uint8> SaveData;
		SaveWorldToMemory(SaveData);
		if (SaveData.Num() > 0)
		{
			FString SlotName = FString::Printf(TEXT("Autosave-%s"), *FDateTime::UtcNow().ToString());
			UGameplayStatics::SaveDataToSlot(SaveData, SlotName, 0);
		}
		UE_LOG(LogZenb, Display, TEXT("Autosaved!"));
	}
}
