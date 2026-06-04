#pragma once

#include "CoreMinimal.h"

class UCYYSaveGame;
class ACYYCharacterFather;

class DEMO_CYY_API FCYYSaveManager
{
public:
	// 加载已有存档，或创建新存档并立即写入磁盘
	static UCYYSaveGame* LoadOrCreate();

	// 将存档对象写入磁盘
	static bool SaveGame(UCYYSaveGame* Save);

	// 删除存档文件
	static bool DeleteSave();

	// 检查是否存在存档文件
	static bool HasSaveFile();

	// 从玩家身上收集数据填入存档对象
	static void CollectFromPlayer(UCYYSaveGame* Save, ACYYCharacterFather* Player);

	// 从存档对象恢复数据到玩家
	static void ApplyToPlayer(const UCYYSaveGame* Save, ACYYCharacterFather* Player);
};
