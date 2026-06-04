#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UMGGameModeBase.generated.h"

UCLASS()
class DEMO_CYY_API AUMGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AUMGGameModeBase();

protected:
	virtual void BeginPlay() override;
};
