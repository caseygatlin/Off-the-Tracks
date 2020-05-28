#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/UObjectGlobals.h"
#include "E_CustomEnums.generated.h"

UENUM(BlueprintType)
enum class E_CartLeanState: uint8
{
	LEFT,
	CENTER,
	RIGHT
};
UENUM(BlueprintType)
enum class E_CartSpeedState: uint8
{
	SLOW,
	MEDIUM,
	FAST
};
UENUM(BlueprintType)
enum class E_CactusFireState : uint8
{
	RANDOM,
	AIMING
};

UENUM(BlueprintType)
enum class E_CactusFireLane : uint8
{
	LEFT,
	RIGHT
};