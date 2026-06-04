#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MinimapTrackingSubsystem.generated.h"

class AEnemyBase;

UCLASS()
class DEMO_CYY_API UMinimapTrackingSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterEnemy(AEnemyBase* Enemy);
	void UnregisterEnemy(AEnemyBase* Enemy);
	const TArray<TWeakObjectPtr<AEnemyBase>>& GetActiveEnemies();

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<AEnemyBase>> ActiveEnemies;

	int32 CleanupCounter = 0;
	static constexpr int32 CleanupInterval = 60;
};

