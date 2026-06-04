#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Types/CYYPlayerState.h"
#include "CYYSaveGame.generated.h"

UCLASS()
class DEMO_CYY_API UCYYSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// ── 关卡进度 ──
	UPROPERTY()
	int32 MaxUnlockedLevel = 1;

	UPROPERTY()
	int32 CurrentLevel = 1;

	UPROPERTY()
	bool bLevel1Completed = false;

	UPROPERTY()
	bool bLevel2Completed = false;

	// ── 玩家状态 ──
	UPROPERTY()
	int32 MedkitCount = 0;

	UPROPERTY()
	int32 PlayerLevel = 1;

	UPROPERTY()
	int32 PlayerXP = 0;

	// ── 武器 ──
	UPROPERTY()
	TArray<E_Weapon> EquippedWeaponTypes;

	UPROPERTY()
	TArray<E_Weapon> BackpackWeaponTypes;

	// ── 元数据 ──
	UPROPERTY()
	FDateTime SaveTimestamp;

	// ── 槽位名称常量 ──
	static const FString SaveSlotName;

	// ── 辅助方法（纯 C++，非 UFUNCTION） ──
	bool IsLevelCompleted(int32 Level);
	void MarkLevelCompleted(int32 Level);
	bool IsLevelUnlocked(int32 Level);
	void UnlockLevel(int32 Level);
};
