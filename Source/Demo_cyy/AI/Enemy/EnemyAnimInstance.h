// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_CYY_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;
	/*virtual void NativeUpdateAnimation(float DeltaSeconds) override;*/

private:
	//speed 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Direction = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming = false;

	// Fallback speed calculation uses position delta when Velocity is not updated.
	FVector LastLocation = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AEnemyBase* EnemyCharacter;
	
};
