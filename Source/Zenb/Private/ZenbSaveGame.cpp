// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbSaveGame.h"

// Public Functions:

void FZenbSaveGame::Read(FMemoryReader& MemoryReader)
{
	int32 FileTypeTag;
	MemoryReader << FileTypeTag;

	int32 FileVersionNumber;
	MemoryReader << FileVersionNumber;
	FileVersion = static_cast<EZenbSaveGameFileVersion>(FileVersionNumber);

	if (FileVersion >= EZenbSaveGameFileVersion::AddedViewLocationAndRotation)
	{
		MemoryReader << bHasViewState;
		MemoryReader << ViewLocation;
		MemoryReader << ViewRotation;
	}
	else
	{
		bHasViewState = false;
		ViewLocation = FVector::ZeroVector;
		ViewRotation = FRotator::ZeroRotator;
	}

	int32 NumBricks;
	MemoryReader << NumBricks;
	Bricks.Reset(NumBricks);
	for (int32 Index = 0; Index < NumBricks; ++Index)
	{
		FZenbSavedBrick& Brick = Bricks.AddDefaulted_GetRef();
	
		if (FileVersion >= EZenbSaveGameFileVersion::AddedBaseplateClass)
		{
			uint8 BrickClass;
			MemoryReader << BrickClass;
			Brick.Class = static_cast<EZenbSavedBrickClass>(BrickClass);
		}
		else
		{
			Brick.Class = EZenbSavedBrickClass::Brick;
		}

		MemoryReader << Brick.Location;
		MemoryReader << Brick.Rotation;
		MemoryReader << Brick.Dimensions;
		MemoryReader << Brick.Color;
	}
}

void FZenbSaveGame::Write(FMemoryWriter& MemoryWriter)
{
	int32 FileTypeTag = ('z' << 24) | ('e' << 16) | ('n' << 8) | 'b';
	MemoryWriter << FileTypeTag;
	int32 FileVersionNumber = static_cast<int32>(EZenbSaveGameFileVersion::LatestVersion);
	MemoryWriter << FileVersionNumber;

	MemoryWriter << bHasViewState;
	MemoryWriter << ViewLocation;
	MemoryWriter << ViewRotation;

	int32 NumBricks = Bricks.Num();
	MemoryWriter << NumBricks;
	for (FZenbSavedBrick& Brick : Bricks)
	{
		uint8 BrickClass = static_cast<uint8>(Brick.Class);
		MemoryWriter << BrickClass;
		MemoryWriter << Brick.Location;
		MemoryWriter << Brick.Rotation;
		MemoryWriter << Brick.Dimensions;
		MemoryWriter << Brick.Color;
	}
}
