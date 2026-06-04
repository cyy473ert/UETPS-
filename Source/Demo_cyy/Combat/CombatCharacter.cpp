// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/DamageEvents.h"

ACombatCharacter::ACombatCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ACombatCharacter::OnHandleDeath);
	}
}

float ACombatCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HealthComponent)
	{
		return 0.0f;
	}

	const float OldHealth = HealthComponent->GetCurrentHealth();
	HealthComponent->ApplyDamage(DamageAmount, DamageCauser);
	return OldHealth - HealthComponent->GetCurrentHealth();
}

float ACombatCharacter::ReceivePointDamage_Implementation(float Damage, AController* InstigatorController, AActor* DamageCauser)
{
	FDamageEvent DamageEvent;
	return TakeDamage(Damage, DamageEvent, InstigatorController, DamageCauser);
}

void ACombatCharacter::OnHandleDeath(AActor* DeadActor)
{
	HandleDeath();
}

void ACombatCharacter::HandleDeath()
{
	if (bDeathHandled)
	{
		return;
	}
	bDeathHandled = true;

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->DisableMovement();
	}

	if (DeathMontage)
	{
		if (UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInst->Montage_Play(DeathMontage);
			UE_LOG(LogTemp, Warning, TEXT("death"));
		}
	}

	if (DeathDestroyDelay > 0.0f)
	{
		SetLifeSpan(DeathDestroyDelay);
	}
}
