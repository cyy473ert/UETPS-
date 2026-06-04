// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interact.generated.h"

class ACYYCharacterFather;
class UWeapon;

// 禁止蓝图实现该接口，避免蓝图覆盖C++默认交互逻辑。
UINTERFACE(MinimalAPI)
class UInteract : public UInterface
{
	GENERATED_BODY()
};

class DEMO_CYY_API IInteract
{
	GENERATED_BODY()

public:
	// 显示交互提示
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void ShowInteractWidget();
	// 隐藏交互提示
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void HideInteractWidget();
	// 交互对象对应的数据资产
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	UWeapon* DataAsset();

	// 非武器交互（电闸、钥匙、安全门等）
	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void OnInteract(ACYYCharacterFather* Interactor);
};
