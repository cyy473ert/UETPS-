// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/CYYPlayerState.h"
#include "Weapon.generated.h"

class UAnimMontage;
class UTexture2D;

UCLASS(BlueprintType)
class DEMO_CYY_API UWeapon : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	E_Weapon CurrentWeapon = E_Weapon::NoWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 CurrentAmmo = 30;

	// 总弹药容量（备用弹药池上限）
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 MaxAmmo = 180;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMesh* SkeletonMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UStaticMesh* StaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (MinValue = 0.0f))
	float Damage = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (MinValue = 0.1f))
	float FireRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (MinValue = 1))
	int32 MagazineSize = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* FireMontage = nullptr;

	// ADS开火动画
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* ADSFireMontage = nullptr;

	// 换弹动画蒙太奇
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Animation")
	UAnimMontage* ReloadMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|UI")
	UTexture2D* WeaponIcon = nullptr;
};
