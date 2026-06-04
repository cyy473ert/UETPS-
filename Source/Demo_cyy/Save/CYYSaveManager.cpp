#include "CYYSaveManager.h"
#include "CYYSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Components/InventoryComponent.h"
#include "Components/LevelProgressionComponent.h"
#include "Components/HealthComponent.h"
#include "Weapons/Item.h"
#include "DataAssets/Weapon.h"

UCYYSaveGame* FCYYSaveManager::LoadOrCreate()
{
	UCYYSaveGame* Save = Cast<UCYYSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UCYYSaveGame::SaveSlotName, 0));

	if (!Save)
	{
		// 无存档 → 创建默认存档并写入磁盘
		Save = NewObject<UCYYSaveGame>();
		if (Save)
		{
			UE_LOG(LogTemp, Log, TEXT("[CYYSaveManager] 未找到存档，已创建默认存档"));
			UGameplayStatics::SaveGameToSlot(Save, UCYYSaveGame::SaveSlotName, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[CYYSaveManager] 已加载存档: %s"), *UCYYSaveGame::SaveSlotName);
	}

	return Save;
}

bool FCYYSaveManager::SaveGame(UCYYSaveGame* Save)
{
	if (!Save)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CYYSaveManager] SaveGame 失败: Save 为空"));
		return false;
	}

	Save->SaveTimestamp = FDateTime::UtcNow();

	if (UGameplayStatics::SaveGameToSlot(Save, UCYYSaveGame::SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[CYYSaveManager] 存档成功写入: %s"), *UCYYSaveGame::SaveSlotName);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CYYSaveManager] 存档写入失败: %s"), *UCYYSaveGame::SaveSlotName);
	return false;
}

bool FCYYSaveManager::DeleteSave()
{
	if (UGameplayStatics::DeleteGameInSlot(UCYYSaveGame::SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[CYYSaveManager] 存档已删除: %s"), *UCYYSaveGame::SaveSlotName);
		return true;
	}
	return false;
}

bool FCYYSaveManager::HasSaveFile()
{
	return UGameplayStatics::DoesSaveGameExist(UCYYSaveGame::SaveSlotName, 0);
}

void FCYYSaveManager::CollectFromPlayer(UCYYSaveGame* Save, ACYYCharacterFather* Player)
{
	if (!Save || !Player)
	{
		return;
	}

	// 1. 收集医疗包数量
	if (UInventoryComponent* Inventory = Player->GetInventoryComponent())
	{
		Save->MedkitCount = Inventory->GetMedkitCount();
	}

	// 2. 收集武器槽中的武器类型（索引 0, 1）
	Save->EquippedWeaponTypes.Empty();
	const TArray<FWeaponSlot>& Slots = Player->GetWeaponSlots();
	for (const FWeaponSlot& Slot : Slots)
	{
		if (Slot.WeaponActor && Slot.WeaponActor->WeaponDataAsset)
		{
			const E_Weapon WeaponType = Slot.WeaponActor->WeaponDataAsset->CurrentWeapon;
			if (WeaponType != E_Weapon::NoWeapon)
			{
				Save->EquippedWeaponTypes.Add(WeaponType);
			}
		}
	}

	// 3. 收集背包武器类型列表
	Save->BackpackWeaponTypes.Empty();
	const TArray<FWeaponSlot>& Backpack = Player->GetBackpackWeapons();
	for (const FWeaponSlot& Slot : Backpack)
	{
		if (Slot.WeaponActor && Slot.WeaponActor->WeaponDataAsset)
		{
			const E_Weapon WeaponType = Slot.WeaponActor->WeaponDataAsset->CurrentWeapon;
			if (WeaponType != E_Weapon::NoWeapon)
			{
				Save->BackpackWeaponTypes.Add(WeaponType);
			}
		}
	}

	// 4. 收集等级数据
	if (ULevelProgressionComponent* LPC = Player->GetLevelComponent())
	{
		Save->PlayerLevel = LPC->CurrentLevel;
		Save->PlayerXP = LPC->CurrentXP;
	}
}

void FCYYSaveManager::ApplyToPlayer(const UCYYSaveGame* Save, ACYYCharacterFather* Player)
{
	if (!Save || !Player)
	{
		return;
	}

	// 1. 恢复医疗包数量
	if (UInventoryComponent* Inventory = Player->GetInventoryComponent())
	{
		Inventory->ClearInventory();
		Inventory->AddMedkit(Save->MedkitCount);
	}

	// 2. 恢复武器
	Player->RestoreWeaponsFromSave(Save->EquippedWeaponTypes, Save->BackpackWeaponTypes);

	// 3. 恢复等级数据
	if (ULevelProgressionComponent* LPC = Player->GetLevelComponent())
	{
		LPC->CurrentLevel = Save->PlayerLevel;
		LPC->CurrentXP = Save->PlayerXP;

		// 恢复 HP 倍率并回满血
		if (UHealthComponent* HC = Player->GetHealthComponent())
		{
			HC->SetMaxHealthMultiplier(LPC->GetHPMultiplier());
			HC->ResetHealth();
		}
	}
}
