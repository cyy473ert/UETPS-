// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

class AController;
class AActor;

UINTERFACE(MinimalAPI, BlueprintType)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class DEMO_CYY_API IDamageable
{
	GENERATED_BODY()

public:
	// 统一受击入口：返回实际应用的伤害值。
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	float ReceivePointDamage(float Damage, AController* InstigatorController, AActor* DamageCauser);

	// 命中事件：用于命中特效、命中反馈和受击反应动画。
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	void Hit(const FHitResult& HitResult);
};

