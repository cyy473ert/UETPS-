#pragma once

#include "CoreMinimal.h"
#include "AI/Enemy/EnemyBase.h"
#include "RangedEnemyCharacter.generated.h"

class USphereComponent;
class UBehaviorTree;
class UAnimMontage;
class UPrimitiveComponent;
class UStaticMeshComponent;

UCLASS()
class DEMO_CYY_API ARangedEnemyCharacter : public AEnemyBase
{
	GENERATED_BODY()

public:
	ARangedEnemyCharacter();
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	FORCEINLINE bool IsAiming() const { return bIsAiming; }
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE bool IsRetreating() const { return bIsRetreating; }
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartRetreat();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StopRetreat();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombatStrafe();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE bool IsCombatStrafing() const { return bIsCombatStrafing; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsRangedEnemy() const override { return true; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartRangedAttack();
	void OnAimComplete();
	void FireHitscan();
	FVector GetFiringDirection() const;
	void ResetAttackCooldown();
	void ApplyRetreatMovement(float DeltaTime);

	UFUNCTION()
	void OnDetectOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCombatRangeOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCombatRangeEndOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
		UPrimitiveComponent* OtherComp, int32 BodyIndex);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnFireNotify();

	virtual void Hit_Implementation(const FHitResult& HitResult) override;
	void PlayHitReactionMontage(FName MontageSection, float PlayRate);
	void ResetHitReactTimer();
	void PerformRetreat();

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (MakeEditWidget = "true"))
	FVector PatrolPoint1 = FVector(300.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (MakeEditWidget = "true"))
	FVector PatrolPoint2 = FVector(-300.f, 0.f, 0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* DetectSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CombatRangeSphere = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* WeaponMesh = nullptr;

	FTimerHandle AimTimer;
	FTimerHandle CooldownTimer;
	FTimerHandle HitReactTimer;

	float DistanceCheckAccumulator = 0.0f;
	bool bCanAttack = true;
	bool bIsAiming = false;
	bool bInAttackRange = false;
	bool bCanHitReact = true;
	
	bool bIsRetreating = false;
	bool bBeginPlayInitializing = false;
	bool bBeginPlayInitialized = false;
	FVector RetreatDirection = FVector::ZeroVector;
	float RetreatTimeRemaining = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RetreatDuration = 1.2f;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RetreatSpeed = 300.0f;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float RetreatStrafeChance = 0.4f;

	bool bIsCombatStrafing = false;
	FVector StrafeDirection = FVector::ZeroVector;
	float StrafeTimeRemaining = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StrafeDuration = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float StrafeSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitReactionMnontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMax = 3.0f;
};
