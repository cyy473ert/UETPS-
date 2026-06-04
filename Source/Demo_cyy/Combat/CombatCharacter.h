#pragma once

#include "CoreMinimal.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "Interaction/Damageable.h"
#include "CombatCharacter.generated.h"

class UAnimMontage;
class UHealthComponent;

UCLASS()
class DEMO_CYY_API ACombatCharacter : public ACharacter, public IDamageable
{
	GENERATED_BODY()

public:
	ACombatCharacter();

protected:
	virtual void BeginPlay() override;
	//¼Ì³Ðdeath
	virtual void HandleDeath();
	//º¯Êýdeath
	UFUNCTION()
	void OnHandleDeath(AActor* DeadActor);

public:
	virtual float TakeDamage(
		float DamageAmount,
		const FDamageEvent& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	virtual float ReceivePointDamage_Implementation(float Damage, AController* InstigatorController, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category="Health")
	UHealthComponent* GetHealthComponent() const { return HealthComponent; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	UHealthComponent* HealthComponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	UAnimMontage* DeathMontage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat", meta=(ClampMin="0.0"))
	float DeathDestroyDelay =0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	bool bDeathHandled = false;
};

