// Copyright Smartly Dressed Games Ltd. Provided under the MIT license.

#pragma once

#include "ZenbTool.h"
#include "ZenbExplosionTool.generated.h"

UCLASS()
class ZENB_API UZenbExplosionTool : public UZenbTool
{
	GENERATED_BODY()

public: // Interfaces

	//~ Begin UZenbTool Interface
	void OnEquip() override;
	void OnDequip() override;
	void OnLeftMousePressed() override;
	bool ShouldHoverActorUnderMouse() const { return false; }
	//~ End UZenbTool Interface
};
