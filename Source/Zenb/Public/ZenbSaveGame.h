// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

enum class EZenbSaveGameFileVersion : int32
{
	InitialVersion = 1,
	AddedBaseplateClass = 2,
	AddedViewLocationAndRotation = 3,

	VersionPlusOne,
	LatestVersion = VersionPlusOne - 1
};

enum class EZenbSavedBrickClass : uint8
{
	Brick,
	Baseplate,
};

struct ZENB_API FZenbSavedBrick
{
	EZenbSavedBrickClass Class;
	FVector Location;
	FQuat Rotation;
	FIntVector Dimensions;
	FLinearColor Color;
};

struct ZENB_API FZenbSaveGame
{
	void Read(class FMemoryReader& MemoryReader);
	void Write(class FMemoryWriter& MemoryWriter);

	EZenbSaveGameFileVersion FileVersion;
	bool bHasViewState;
	FVector ViewLocation;
	FRotator ViewRotation;

	TArray<FZenbSavedBrick> Bricks;
};
