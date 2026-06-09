#pragma once

#include "CoreMinimal.h"
#include "Combat/CombatCharacter.h"
#include "DataAssets/Monster.h"
#include "EnemyBase.generated.h"

class USphereComponent;
class AEnemyController;
class UWidgetComponent;
class UHealthComponent;
class UUserWidget;
class AHealPickup;
class USoundBase;

UCLASS()
class DEMO_CYY_API AEnemyBase : public ACombatCharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void HandleDeath() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

protected:

	float GetAttackDamage() const;
	virtual bool IsRangedEnemy() const { return false; }
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMnontage = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category="Data")
	UMonster* Monster = nullptr;
	FVector SpawnOrigin = FVector::ZeroVector;
	AActor* TargetActor = nullptr;

	AEnemyController* EnemyController = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverheadHealthBar = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> OverheadHealthBarClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	float OverheadHealthBarDistance = 1500.f;
	float OverheadVisibilityAccumulator = 0.f;

	void UpdateOverheadHealthBar();
	UFUNCTION()
	void OnOverheadHealthChanged(UHealthComponent* HC, float NewHealth, float Delta, AActor* InstigatorActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Drop", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AHealPickup> HealPickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Drop", meta=(AllowPrivateAccess = "true", ClampMin="0.0", ClampMax="1.0"))
	float DropChance = 0.25f;

	// ── 每只怪的实例覆盖参数（在关卡里单独调）──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	float OverrideHP = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true", ClampMin = "50"))
	float OverrideSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true", ClampMin = "1"))
	float OverrideDamage = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true", ClampMin = "0.5"))
	float OverrideAttackInterval = 2.0f;

	// 覆盖开关（不勾就用 Monster DataAsset 默认值）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bOverrideHP = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bOverrideSpeed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bOverrideDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Override", meta = (AllowPrivateAccess = "true"))
	bool bOverrideAttackInterval = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 XPValue = 50;

	// ── 音效 ──
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* AlertSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* HurtSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* DeathSound = nullptr;

	AActor* GetTargetActor() const { return TargetActor; }
	UMonster* GetMonsterData() const { return Monster; }
};
