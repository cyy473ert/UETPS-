// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_CYY_API AEnemyController : public AAIController
{
	GENERATED_BODY()
public:
	AEnemyController();
	virtual void OnPossess(APawn* InPawn) override;
private:
	//黑板组件
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBlackboardComponent* BlackboardComponent;

	//行为树组件
	UPROPERTY(BlueprintReadWrite, Category = "AI Behavior", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

public:

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }
	
};
