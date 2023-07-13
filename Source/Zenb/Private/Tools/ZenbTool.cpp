// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "Tools/ZenbTool.h"
#include "ZenbBrickActor.h"
#include "GameFramework/PlayerController.h"
#include "ZenbPawn.h"
#include "Engine/World.h"

// Public Functions:

void UZenbTool::Tick()
{
	if (ShouldHoverActorUnderMouse())
	{
		APlayerController* PC = Pawn->GetController<APlayerController>();
		if (!PC)
			return;

		FVector MouseRayLocation;
		FVector MouseRayDirection;
		if (!PC->DeprojectMousePositionToWorld(MouseRayLocation, MouseRayDirection))
			return;

		AZenbBrickActor* NewHoverActor = nullptr;
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, MouseRayLocation, MouseRayLocation + MouseRayDirection * 10000.f, ECC_Visibility))
		{
			NewHoverActor = Cast<AZenbBrickActor>(Hit.GetActor());
		}

		if (NewHoverActor != nullptr && !ShouldHoverActor(NewHoverActor))
		{
			NewHoverActor = nullptr;
		}

		Pawn->SetHoveredActor(NewHoverActor);
	}
	else
	{
		Pawn->SetHoveredActor(nullptr);
	}
}
