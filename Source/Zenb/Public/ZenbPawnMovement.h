// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "GameFramework/FloatingPawnMovement.h"
#include "ZenbPawnMovement.generated.h"

UCLASS()
class ZENB_API UZenbPawnMovement : public UFloatingPawnMovement
{
	GENERATED_BODY()

protected: // Interfaces

	//~ Begin UFloatingPawnMovement Interface
	void ApplyControlInputToVelocity(float DeltaTime) override;
	//~ End UFloatingPawnMovement Interface
};
