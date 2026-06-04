// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/Notifies/FireAnimNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "Gameplay/Player/CYYCharacterFather.h"

void UFireAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	// 统一走角色通知入口：同帧触发粒子与伤害判定。
	if (ACYYCharacterFather* Character = Cast<ACYYCharacterFather>(MeshComp->GetOwner()))
	{
		Character->TriggerFireByAnimNotify();
	}
}

