// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#include "ZenbPawnMovement.h"

// Protected Interfaces:

//~ Begin UFloatingPawnMovement Interface
void UZenbPawnMovement::ApplyControlInputToVelocity(float DeltaTime)
{
	Velocity = GetPendingInputVector() * GetMaxSpeed();
	ConsumeInputVector();
}
//~ End UFloatingPawnMovement Interface
