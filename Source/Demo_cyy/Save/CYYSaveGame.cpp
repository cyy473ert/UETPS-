#include "CYYSaveGame.h"

const FString UCYYSaveGame::SaveSlotName = TEXT("ShelterSave01");

bool UCYYSaveGame::IsLevelCompleted(int32 Level)
{
	switch (Level)
	{
	case 1: return bLevel1Completed;
	case 2: return bLevel2Completed;
	default: return false;
	}
}

void UCYYSaveGame::MarkLevelCompleted(int32 Level)
{
	switch (Level)
	{
	case 1: bLevel1Completed = true; break;
	case 2: bLevel2Completed = true; break;
	}
	UnlockLevel(Level + 1);
}

bool UCYYSaveGame::IsLevelUnlocked(int32 Level)
{
	return Level <= MaxUnlockedLevel;
}

void UCYYSaveGame::UnlockLevel(int32 Level)
{
	MaxUnlockedLevel = FMath::Max(MaxUnlockedLevel, Level);
}
