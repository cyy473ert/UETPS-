// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Player/CYYAnimInstance.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCYYAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (CharacterFather == nullptr)
	{
		CharacterFather = Cast<ACYYCharacterFather>(TryGetPawnOwner());
	}
	if (CharacterFather)
	{
		//��ȡ�ٶȵķ���
		FVector Velocity{ CharacterFather->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();
		
		//�Ƿ��ڿ���
		bIsAir = CharacterFather-> GetCharacterMovement()->IsFalling();

		//���ٶ�
		if (CharacterFather->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f) 
		{
			bIsAccelerating = true;
		}
		else 
		{
			bIsAccelerating = false;
		}
		//��ֵ 
		AxisVectorX = CharacterFather ->AxisVectorX;
		AxisVectorY = CharacterFather ->AxisVectorY;

		E_PlayerState State = CharacterFather->CurrentPlayerState;
		CurrentWeaponType = CharacterFather->CurrentWeaponType;
		bIsAiming = CharacterFather->bIsAiming;
		bIsFiring = CharacterFather->bIsFiring;

		// �����߼�����ǹʱ �� �����ٶ�=0
		AnimSpeed = bIsFiring ? 0.0f : Speed;
	}
}

void UCYYAnimInstance::NativeInitializeAnimation()
{
	CharacterFather = Cast<ACYYCharacterFather>(TryGetPawnOwner());
	
}



void UCYYAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	UpdateAnimationProperties(DeltaSeconds);
}

