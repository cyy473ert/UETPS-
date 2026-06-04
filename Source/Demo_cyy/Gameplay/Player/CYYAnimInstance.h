// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Types/CYYPlayerState.h"
#include "CYYAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_CYY_API UCYYAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class ACYYCharacterFather* CharacterFather;

	// ö������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerState, meta = (AllowPrivateAccess = "True"))
	E_Weapon CurrentWeaponType = E_Weapon::NoWeapon;
	//��ɫ�ٶ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;
	// ���������ٶȣ�����ʱ��ѹΪ0����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float AnimSpeed;
	// �Ƿ����ڿ������ڶ���״̬�л�����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsFiring = false;
	//�Ƿ��ڿ�����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAir;
	//�Ƿ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	//�Ƿ���׼
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAiming;

public:
	// WSAD �������� 2D ����ֵ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "True"))
	float AxisVectorX;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "True"))
	float AxisVectorY;
};

