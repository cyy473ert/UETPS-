#include "CYYSaveManager.h"

#include "CYYSaveGame.h"
#include "Components/HealthComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/LevelProgressionComponent.h"
#include "DataAssets/Weapon.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Kismet/GameplayStatics.h"
#include "Weapons/Item.h"

UCYYSaveGame* FCYYSaveManager::LoadOrCreate()
{
	UCYYSaveGame* Save = LoadExisting();
	if (!Save)
	{
		Save = CreateNew();
		if (Save)
		{
			UGameplayStatics::SaveGameToSlot(Save, UCYYSaveGame::SaveSlotName, 0);
		}
	}
	return Save;
}

UCYYSaveGame* FCYYSaveManager::LoadExisting()
{
	return Cast<UCYYSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UCYYSaveGame::SaveSlotName, 0));
}

UCYYSaveGame* FCYYSaveManager::CreateNew()
{
	return NewObject<UCYYSaveGame>();
}

bool FCYYSaveManager::SaveGame(UCYYSaveGame* Save)
{
	if (!Save)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CYYSaveManager] SaveGame failed: Save is null"));
		return false;
	}

	Save->SaveTimestamp = FDateTime::UtcNow();

	if (UGameplayStatics::SaveGameToSlot(Save, UCYYSaveGame::SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[CYYSaveManager] Save written: %s"), *UCYYSaveGame::SaveSlotName);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("[CYYSaveManager] Save write failed: %s"), *UCYYSaveGame::SaveSlotName);
	return false;
}

bool FCYYSaveManager::DeleteSave()
{
	if (UGameplayStatics::DeleteGameInSlot(UCYYSaveGame::SaveSlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("[CYYSaveManager] Save deleted: %s"), *UCYYSaveGame::SaveSlotName);
		return true;
	}
	return false;
}

bool FCYYSaveManager::HasSaveFile()
{
	const UCYYSaveGame* Save = LoadExisting();
	return Save && Save->bHasStartedGame && Save->bHasPlayerSnapshot;
}

void FCYYSaveManager::CollectFromPlayer(UCYYSaveGame* Save, ACYYCharacterFather* Player)
{
	if (!Save || !Player)
	{
		return;
	}

	Save->bHasPlayerSnapshot = true;

	if (UInventoryComponent* Inventory = Player->GetInventoryComponent())
	{
		Save->MedkitCount = Inventory->GetMedkitCount();
	}

	Save->EquippedWeaponTypes.Empty();
	const TArray<FWeaponSlot>& Slots = Player->GetWeaponSlots();
	for (const FWeaponSlot& Slot : Slots)
	{
		if (Slot.WeaponActor)
		{
			const UWeapon* WeaponData = Slot.WeaponActor->DataAsset_Implementation();
			if (WeaponData && WeaponData->CurrentWeapon != E_Weapon::NoWeapon)
			{
				Save->EquippedWeaponTypes.Add(WeaponData->CurrentWeapon);
			}
		}
	}

	Save->BackpackWeaponTypes.Empty();
	const TArray<FWeaponSlot>& Backpack = Player->GetBackpackWeapons();
	for (const FWeaponSlot& Slot : Backpack)
	{
		if (Slot.WeaponActor)
		{
			const UWeapon* WeaponData = Slot.WeaponActor->DataAsset_Implementation();
			if (WeaponData && WeaponData->CurrentWeapon != E_Weapon::NoWeapon)
			{
				Save->BackpackWeaponTypes.Add(WeaponData->CurrentWeapon);
			}
		}
	}

	if (ULevelProgressionComponent* LPC = Player->GetLevelComponent())
	{
		Save->PlayerLevel = LPC->CurrentLevel;
		Save->PlayerXP = LPC->CurrentXP;
	}

	UE_LOG(LogTemp, Log, TEXT("[SAVE] Equipped=%d Backpack=%d Medkits=%d Level=%d XP=%d"),
		Save->EquippedWeaponTypes.Num(),
		Save->BackpackWeaponTypes.Num(),
		Save->MedkitCount,
		Save->PlayerLevel,
		Save->PlayerXP);
}

void FCYYSaveManager::ApplyToPlayer(const UCYYSaveGame* Save, ACYYCharacterFather* Player)
{
	if (!Save || !Player)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[LOAD] Equipped=%d Backpack=%d Medkits=%d Level=%d"),
		Save->EquippedWeaponTypes.Num(),
		Save->BackpackWeaponTypes.Num(),
		Save->MedkitCount,
		Save->PlayerLevel);

	if (UInventoryComponent* Inventory = Player->GetInventoryComponent())
	{
		Inventory->ClearInventory();
		Inventory->AddMedkit(Save->MedkitCount);
	}

	Player->RestoreWeaponsFromSave(Save->EquippedWeaponTypes, Save->BackpackWeaponTypes);

	if (ULevelProgressionComponent* LPC = Player->GetLevelComponent())
	{
		LPC->CurrentLevel = Save->PlayerLevel;
		LPC->CurrentXP = Save->PlayerXP;

		if (UHealthComponent* HC = Player->GetHealthComponent())
		{
			HC->SetMaxHealthMultiplier(LPC->GetHPMultiplier());
			HC->ResetHealth();
		}
	}
}
