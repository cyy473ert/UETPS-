#include "Subsystems/MinimapTrackingSubsystem.h"

#include "AI/Enemy/EnemyBase.h"

void UMinimapTrackingSubsystem::RegisterEnemy(AEnemyBase* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	ActiveEnemies.AddUnique(Enemy);
}

void UMinimapTrackingSubsystem::UnregisterEnemy(AEnemyBase* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	ActiveEnemies.RemoveSingle(Enemy);
}

const TArray<TWeakObjectPtr<AEnemyBase>>& UMinimapTrackingSubsystem::GetActiveEnemies()
{
	if (++CleanupCounter >= CleanupInterval)
	{
		CleanupCounter = 0;
		ActiveEnemies.RemoveAll([](const TWeakObjectPtr<AEnemyBase>& Ptr)
		{
			return !Ptr.IsValid();
		});
	}

	return ActiveEnemies;
}

