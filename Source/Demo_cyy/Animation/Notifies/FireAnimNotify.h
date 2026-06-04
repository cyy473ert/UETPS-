// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FireAnimNotify.generated.h"

class USkeletalMeshComponent;
class UAnimSequenceBase;
struct FAnimNotifyEventReference;

/**
 * 
 */
UCLASS()
class DEMO_CYY_API UFireAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	// 使用新版Notify签名，兼容后续引擎版本。
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
