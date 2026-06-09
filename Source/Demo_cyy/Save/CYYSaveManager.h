#pragma once

#include "CoreMinimal.h"

class UCYYSaveGame;
class ACYYCharacterFather;

class DEMO_CYY_API FCYYSaveManager
{
public:
	static UCYYSaveGame* LoadOrCreate();
	static UCYYSaveGame* LoadExisting();
	static UCYYSaveGame* CreateNew();

	static bool SaveGame(UCYYSaveGame* Save);
	static bool DeleteSave();
	static bool HasSaveFile();

	static void CollectFromPlayer(UCYYSaveGame* Save, ACYYCharacterFather* Player);
	static void ApplyToPlayer(const UCYYSaveGame* Save, ACYYCharacterFather* Player);
};
