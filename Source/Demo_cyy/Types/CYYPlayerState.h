#pragma once

#include "CoreMinimal.h"
#include "CYYPlayerState.generated.h"

UENUM(BlueprintType)
enum class E_PlayerState : uint8
{
	Idle,
	Walk,
	Run,
	Aim,
	Jump
};

UENUM(BlueprintType)
enum class E_Weapon: uint8
{
	NoWeapon,
	RU74,
	Beretta,
};

