#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChangedSignature, UHealthComponent*, HealthComponent, float, NewHealth, float, Delta, AActor*, InstigatorActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, DeadActor);

UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class DEMO_CYY_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

protected:
	virtual void BeginPlay() override;
	// 联机复制声明。
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health", meta=(AllowPrivateAccess="true", ClampMin="1.0"))
	float MaxHealth = 100.0f;

	// 当前血量：复制到客户端并触发 OnRep。
	UPROPERTY(ReplicatedUsing=OnRep_CurrentHealth, VisibleAnywhere, BlueprintReadOnly, Category="Health", meta=(AllowPrivateAccess="true"))
	float CurrentHealth = 100.0f;

	// 死亡标记：复制到客户端并触发 OnRep。
	UPROPERTY(ReplicatedUsing=OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly, Category="Health", meta=(AllowPrivateAccess="true"))
	bool bIsDead = false;

	// 客户端收到新血量后，补发统一事件给 UI/表现层。
	UFUNCTION()
	void OnRep_CurrentHealth(float OldHealth);

	// 客户端收到死亡标记后，补发死亡事件。
	UFUNCTION()
	void OnRep_IsDead();

public:
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintCallable, Category="Health")
	float ApplyDamage(float DamageAmount, AActor* InstigatorActor = nullptr);

	UFUNCTION(BlueprintCallable, Category="Health")
	float Heal(float HealAmount, AActor* InstigatorActor = nullptr);

	UFUNCTION(BlueprintCallable, Category="Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category="Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const { return MaxHealth * MaxHealthMultiplier; }

	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealthPercent() const { return GetMaxHealth() > 0.0f ? CurrentHealth / GetMaxHealth() : 0.0f; }

	void SetMaxHealthMultiplier(float Mult) { MaxHealthMultiplier = Mult; }
	float GetMaxHealthMultiplier() const { return MaxHealthMultiplier; }

	void SetBaseMaxHealth(float NewBase) { MaxHealth = NewBase; }

private:
	float MaxHealthMultiplier = 1.0f;
};
