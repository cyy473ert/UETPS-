#include "DataAssets/MeleeMonsterData.h"

UMeleeMonsterData::UMeleeMonsterData()
{
	Type = EMonsterType::Melee;
	MaxHP = 60.0f;
	MoveSpeed = 300.0f;
	DetectRange = 1800.0f;
	LoseTargetRange = 2200.0f;
	AttackRange = 180.0f;
	AttackInterval = 2.0f;
	Damage = 15.0f;
	MeleeHitRadius = 90.0f;
}
