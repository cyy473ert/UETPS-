#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CYYSettingsSaveGame.generated.h"

UCLASS()
class DEMO_CYY_API UCYYSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float MasterVolume = 1.0f;

	static const FString SaveSlotName;
};
