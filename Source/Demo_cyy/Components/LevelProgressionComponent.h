#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "LevelProgressionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUpSignature, int32, NewLevel, int32, OldLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLevelUpClientSignature, int32, NewLevel, int32, OldLevel);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMO_CYY_API ULevelProgressionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULevelProgressionComponent();

	// 当前等级（服务端复制到客户端，客户端 OnRep 弹 Toast）
	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel, BlueprintReadOnly, Category = "Progression")
	int32 CurrentLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 CurrentXP = 0;

	// 升级阈值：Lv1=0, Lv2=200, Lv3=400, Lv4=700, Lv5=1100
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<int32> LevelThresholds = { 0, 0, 200, 400, 700, 1100 };

	// 每级 HP 倍率 (index 0 = Lv1)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<float> HPPerLevel = { 1.0f, 1.0f, 1.2f, 1.45f, 1.75f, 2.1f };

	// 每级伤害倍率
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<float> DamagePerLevel = { 1.0f, 1.0f, 1.1f, 1.2f, 1.35f, 1.5f };

	void AddXP(int32 Amount);

	float GetHPMultiplier() const;
	float GetDamageMultiplier() const;

	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FOnLevelUpSignature OnLevelUp;

	// 客户端升级事件（OnRep 触发，UI 用它弹 Toast）
	UPROPERTY(BlueprintAssignable, Category = "Progression")
	FOnLevelUpClientSignature OnLevelUpClient;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentLevel();

private:
	void CheckLevelUp();

	// 服务端记录升级前的等级，供客户端 OnRep 使用
	int32 OldLevelCache = 1;
};
