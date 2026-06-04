#pragma once

#include "CoreMinimal.h"
#include "DataAssets/Monster.h"
#include "MeleeMonsterData.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DEMO_CYY_API UMeleeMonsterData : public UMonster
{
	GENERATED_BODY()

public:
	UMeleeMonsterData();
};
