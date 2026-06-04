#pragma once

#include "CoreMinimal.h"
#include "Combat/CombatCharacter.h"
#include "DataAssets/Monster.h"
#include "EnemyBase.generated.h"

class USphereComponent;
class AEnemyController;
class UWidgetComponent;
class UHealthComponent;
class AHealPickup;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	UAnimMontage* AttackMnontage = nullptr;
	
	UPROPERTY(VisibleInstanceOnly, Category="Data")
	UMonster* Monster = nullptr;
	FVector SpawnOrigin = FVector::ZeroVector;
	AActor* TargetActor = nullptr;
	
	AEnemyController* EnemyController = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadHealthBar = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> OverheadHealthBarClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float OverheadHealthBarDistance = 1500.f;
	float OverheadVisibilityAccumulator = 0.f;

	void UpdateOverheadHealthBar();
	UFUNCTION()
	void OnOverheadHealthChanged(UHealthComponent* HC, float NewHealth, float Delta, AActor* InstigatorActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Drop")
	TSubclassOf<AHealPickup> HealPickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0", ClampMax="1.0"))
	float DropChance = 0.35f;

	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 XPValue = 50;
	AActor* GetTargetActor() const { return TargetActor; }
	UMonster* GetMonsterData() const { return Monster; }
};
