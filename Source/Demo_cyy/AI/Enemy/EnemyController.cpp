// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Enemy/EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/Enemy/MeleeEnemyCharacter.h"
#include "AI/Enemy/RangedEnemyCharacter.h"

AEnemyController::AEnemyController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn == nullptr) return;
	AMeleeEnemyCharacter* Enemy = Cast<AMeleeEnemyCharacter>(InPawn);
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree() && Enemy->GetBehaviorTree()->BlackboardAsset)
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}
	} // 远程
    else if (ARangedEnemyCharacter* RangedEnemy = Cast<ARangedEnemyCharacter>(InPawn))
    {
        if (RangedEnemy->GetBehaviorTree() && RangedEnemy->GetBehaviorTree()->BlackboardAsset)
            BlackboardComponent->InitializeBlackboard(*(RangedEnemy->GetBehaviorTree()->BlackboardAsset));
    }
}

