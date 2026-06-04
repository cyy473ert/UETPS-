// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy/EnemyAnimInstance.h"
#include "AI/Enemy/EnemyBase.h"
#include "AI/Enemy/RangedEnemyCharacter.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	EnemyCharacter = Cast<AEnemyBase>(TryGetPawnOwner());
}

void UEnemyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (EnemyCharacter == nullptr)
	{
		EnemyCharacter = Cast<AEnemyBase>(TryGetPawnOwner());
	}

	if (EnemyCharacter)
	{
		FVector Velocity{ EnemyCharacter->GetVelocity() };
		Velocity.Z = 0.f;
		Speed = Velocity.Size();

		// Relative movement direction in degrees:
		// 0 = forward, -90 = left, 90 = right, +-180 = backward.
		const FVector Forward2D = EnemyCharacter->GetActorForwardVector().GetSafeNormal2D();
		const FVector VelDir = Velocity.GetSafeNormal();

		if (!VelDir.IsNearlyZero())
		{
			const float Dot = FMath::Clamp(FVector::DotProduct(Forward2D, VelDir), -1.f, 1.f);
			const float Sign = FVector::CrossProduct(Forward2D, VelDir).Z >= 0.f ? 1.f : -1.f;
			Direction = FMath::RadiansToDegrees(FMath::Acos(Dot)) * Sign;
		}
		else
		{
			Direction = 0.f;
		}

		if (const ARangedEnemyCharacter* RangedEnemy = Cast<ARangedEnemyCharacter>(EnemyCharacter))
		{
			bIsAiming = RangedEnemy->IsAiming();
		}
		else
		{
			bIsAiming = false;
		}
	}
	else
	{
		Speed = 0.0f;
		Direction = 0.0f;
		bIsAiming = false;
	}
}

