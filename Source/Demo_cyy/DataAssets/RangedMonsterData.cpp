#include "DataAssets/RangedMonsterData.h"

URangedMonsterData::URangedMonsterData()
{
	Type = EMonsterType::Ranged;
	MaxHP = 40.0f;
	MoveSpeed = 260.0f;
	DetectRange = 800.0f;
	LoseTargetRange = 1200.0f;
	AttackRange = 500.0f;
	AttackInterval = 0.2f;
	Damage = 10.0f;
	CastTime = 1.0f;
	RetreatDistanceOnHit = 200.0f;
	HitscanRange = 800.0f;
	FireSpread = 0.0f;
	ShotsPerAttack = 1;
	PreferDistanceMin = 200.0f;
	PreferDistanceMax = 600.0f;
}
