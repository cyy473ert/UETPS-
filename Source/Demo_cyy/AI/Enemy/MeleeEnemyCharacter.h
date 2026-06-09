#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/EnemyBase.h"
#include "MeleeEnemyCharacter.generated.h"

class UBoxComponent;
class UAnimMontage;
class USphereComponent;
class AEnemyController;
class USoundBase;
UCLASS()
class DEMO_CYY_API AMeleeEnemyCharacter : public AEnemyBase
{
	GENERATED_BODY()

public:
	AMeleeEnemyCharacter();

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void PerformAttack();
	UFUNCTION(BlueprintCallable)
	void PlayAttackMnontage(FName MontageSection, float PlayRate);
	virtual void Hit_Implementation(const FHitResult& HitResult) override;
	UFUNCTION()
	void ZombiesSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnLeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();
	UFUNCTION()
	void OnRightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(BlueprintCallable)
	void ActivateLeftHand();

	UFUNCTION(BlueprintCallable)
	void DeactivateLeftHand();

	UFUNCTION(BlueprintCallable)
	void ActivateRightHand();

	UFUNCTION(BlueprintCallable)
	void DeactivateRightHand();

private:
	FName RAttack;
	FName LAttack;
	FName RAttack03;
	FName RAttack02;
	void PlayHitReactionMontage(FName MontageSection, float PlayRate);
	void ResetHitReactTimer();
	void ResetMeleeCanAttack();
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint;
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* ZombiesSphere = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere = nullptr;
	bool bHasDealtDamageThisAttack = false;
	bool bCanHitReact = true;
	FName HitReactionSection = NAME_None;
	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactionMnontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMax = 3.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	UBoxComponent* LeftHandCollision = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	UBoxComponent* RightHandCollision = nullptr;
	AEnemyController* EnemyController = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange = false;
	FTimerHandle AttackWaitTimer;
	bool bCanAttack = true;

	// ── 音效 ──
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* MeleeAttackSound = nullptr;


public:
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BehaviorTree = nullptr;
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
};
