#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ShelterGameInstance.generated.h"

UCLASS()
class DEMO_CYY_API UShelterGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Shelter")
	bool bPowerRestored = false;

	UPROPERTY(BlueprintReadWrite, Category = "Shelter")
	bool bHasKeyCard = false;
};
