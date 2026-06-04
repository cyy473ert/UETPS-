#include "LevelProgressionComponent.h"
#include "Net/UnrealNetwork.h"

ULevelProgressionComponent::ULevelProgressionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void ULevelProgressionComponent::AddXP(int32 Amount)
{
	CurrentXP += Amount;
	CheckLevelUp();
}

void ULevelProgressionComponent::CheckLevelUp()
{
	int32 OldLevel = CurrentLevel;
	OldLevelCache = CurrentLevel;
	// 从下一级往上比，找到 XP 对应的最高等级
	for (int32 i = CurrentLevel + 1; i < LevelThresholds.Num(); ++i)
	{
		if (CurrentXP >= LevelThresholds[i])
		{
			CurrentLevel = i;
		}
	}
	if (CurrentLevel != OldLevel)
	{
		OnLevelUp.Broadcast(CurrentLevel, OldLevel);
		CheckLevelUp(); // 递归，可能连升多级
	}
}

float ULevelProgressionComponent::GetHPMultiplier() const
{
	const int32 Idx = FMath::Clamp(CurrentLevel, 0, HPPerLevel.Num() - 1);
	return HPPerLevel[Idx];
}

float ULevelProgressionComponent::GetDamageMultiplier() const
{
	const int32 Idx = FMath::Clamp(CurrentLevel, 0, DamagePerLevel.Num() - 1);
	return DamagePerLevel[Idx];
}

void ULevelProgressionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULevelProgressionComponent, CurrentLevel);
}

void ULevelProgressionComponent::OnRep_CurrentLevel()
{
	if (CurrentLevel != OldLevelCache)
	{
		OnLevelUpClient.Broadcast(CurrentLevel, OldLevelCache);
		OldLevelCache = CurrentLevel;
	}
}
