#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Monster.generated.h"

UENUM(BlueprintType)
enum class EMonsterType : uint8 
    { 
        Melee,
         Ranged 
    };   // Melee: 近战类型, Ranged: 远程类型

// 可在蓝图中继承和编辑
UCLASS(BlueprintType, Blueprintable)
class DEMO_CYY_API UMonster : public UDataAsset
{
    GENERATED_BODY()
public:
    // 怪物类型（近战/远程），决定使用哪一组战斗参数
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EMonsterType Type = EMonsterType::Melee;

    // 最大生命值（HP）
    UPROPERTY(EditAnywhere, BlueprintReadOnly) 
    float MaxHP = 100.f;

    // 移动速度（单位/秒）
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float MoveSpeed = 300.f;

    // 探测玩家的范围（单位），超出此范围不会进入战斗
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float DetectRange = 1800.f;

    // 丢失目标的范围（单位），超过此距离会放弃追击
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float LoseTargetRange = 2200.f;

    // 攻击范围（单位），在此距离内可发动攻击
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float AttackRange = 180.f;

    // 攻击间隔（秒），两次攻击之间的冷却时间
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float AttackInterval = 2.f;

    // 单次攻击造成的伤害值
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Damage = 20.f;

    // ---------- 近战专属参数 ----------
    // 近战攻击的命中半径（单位），用于检测攻击是否击中玩家
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float MeleeHitRadius = 90.f;

    // ---------- 远程专属参数 ----------
    // 攻击前摇（瞄准/蓄力时间），秒
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float CastTime = 0.7f;

    // 射线检测最大距离
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float HitscanRange = 2000.f;

    // 射击散布（度），0 = 完全精准
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float FireSpread = 1.5f;

    // 每次攻击发射几发
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"))
    int32 ShotsPerAttack = 1;

    // 远程怪物期望与玩家保持的最小距离（单位），过近时会后退
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float PreferDistanceMin = 700.f;

    // 远程怪物期望与玩家保持的最大距离（单位），过远时会靠近
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float PreferDistanceMax = 1200.f;

    // 受击后后撤距离（单位）
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float RetreatDistanceOnHit = 200.f;

   
};
