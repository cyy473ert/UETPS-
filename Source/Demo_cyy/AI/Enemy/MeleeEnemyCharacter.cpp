#include "AI/Enemy/MeleeEnemyCharacter.h"
#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAssets/MeleeMonsterData.h"
#include "GameFramework/DamageType.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Enemy/EnemyController.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TimerManager.h"
AMeleeEnemyCharacter::AMeleeEnemyCharacter()
	: RAttack(TEXT("RAttack"))
	, LAttack(TEXT("LAttack"))
	, RAttack03(TEXT("RAttack03"))
	, RAttack02(TEXT("RAttack02"))
{
	Monster = UMeleeMonsterData::StaticClass()->GetDefaultObject<UMeleeMonsterData>();
	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
	ZombiesSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZombiesSphere"));
	ZombiesSphere->SetupAttachment(GetRootComponent());
	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBox"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("hand_l_socket"));
	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandBox"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("hand_r_socket"));

}
void AMeleeEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	EnemyController = Cast<AEnemyController>(GetController());
	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);

	ZombiesSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemyCharacter::ZombiesSphereOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemyCharacter::CombatRangeSphereOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AMeleeEnemyCharacter::CombatRangeEndOverlap);
	LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemyCharacter::OnLeftHandOverlap);
	RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemyCharacter::OnRightHandOverlap);

	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionObjectType(ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECC_WorldDynamic);
	RightHandCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint"), WorldPatrolPoint);
		EnemyController->GetBlackboardComponent()->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}
}

void AMeleeEnemyCharacter::PerformAttack()
{
	bHasDealtDamageThisAttack = false;
	if (!bCanAttack)
	{
		return;
	}

	bCanAttack = false;
	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), bCanAttack);
	}
	const float AttackWaitTime = Monster ? FMath::Max(0.1f, Monster->AttackInterval) : 2.0f;
	GetWorldTimerManager().SetTimer(AttackWaitTimer, this, &AMeleeEnemyCharacter::ResetMeleeCanAttack, AttackWaitTime, false);
	PlayAttackMnontage(GetAttackSectionName(), 1.0f);

	if (MeleeAttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MeleeAttackSound, GetActorLocation());
	}
}
void AMeleeEnemyCharacter::PlayAttackMnontage(FName MontageSection, float PlayRate)
{
	if (AttackMnontage)
	{
		if (UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInst->Montage_Play(AttackMnontage, PlayRate);
			AnimInst->Montage_JumpToSection(MontageSection, AttackMnontage);
		}
	}
}
FName AMeleeEnemyCharacter::GetAttackSectionName()
{
	switch (FMath::RandRange(1, 4))
	{
	case 1: return RAttack;
	case 2: return LAttack;
	case 3: return RAttack03;
	case 4: return RAttack02;
	default: return RAttack;
	}
}

void AMeleeEnemyCharacter::Hit_Implementation(const FHitResult& HitResult)
{
	FString HitBoneName = HitResult.BoneName.ToString();
	HitBoneName.ToLowerInline();
	if (HitBoneName.Contains(TEXT("head")))
	{
		HitReactionSection = FName("HitReaction");
	}
	else if (HitBoneName.Contains(TEXT("upperarm_l")) || HitBoneName.Contains(TEXT("lowerarm_l")) || HitBoneName.Contains(TEXT("hand_l")))
	{
		HitReactionSection = FName("HitReaction_LeftArm");
	}
	else if (HitBoneName.Contains(TEXT("upperarm_r")) || HitBoneName.Contains(TEXT("lowerarm_r")) || HitBoneName.Contains(TEXT("hand_r")))
	{
		HitReactionSection = FName("HitReaction_RightArm");
	}
	else
	{
		HitReactionSection = FName("HitReaction");
	}

	PlayHitReactionMontage(HitReactionSection, 1.0f);
}

void AMeleeEnemyCharacter::ZombiesSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACYYCharacterFather* Character = Cast<ACYYCharacterFather>(OtherActor))
	{
		TargetActor = Character;
		if (EnemyController && EnemyController->GetBlackboardComponent())
		{
			EnemyController->GetBlackboardComponent()->SetValueAsObject(TEXT("Target"), Character);
		}
	}
}

void AMeleeEnemyCharacter::CombatRangeSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACYYCharacterFather>(OtherActor))
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}
void AMeleeEnemyCharacter::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ACYYCharacterFather>(OtherActor))
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}
void AMeleeEnemyCharacter::OnLeftHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor || OtherActor == this)
	{
		return;
	}

	if (bHasDealtDamageThisAttack)
	{
		return;
	}

	if (Cast<ACYYCharacterFather>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, GetAttackDamage(), GetController(), this, UDamageType::StaticClass());
		bHasDealtDamageThisAttack = true;
	}
}

void AMeleeEnemyCharacter::OnRightHandOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor || OtherActor == this)
	{
		return;
	}

	if (bHasDealtDamageThisAttack)
	{
		return;
	}

	if (Cast<ACYYCharacterFather>(OtherActor))
	{
		UGameplayStatics::ApplyDamage(OtherActor, GetAttackDamage(), GetController(), this, UDamageType::StaticClass());
		bHasDealtDamageThisAttack = true;
	}
}

void AMeleeEnemyCharacter::ActivateLeftHand()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMeleeEnemyCharacter::DeactivateLeftHand()
{
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bHasDealtDamageThisAttack = false;
}

void AMeleeEnemyCharacter::ActivateRightHand()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AMeleeEnemyCharacter::DeactivateRightHand()
{
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bHasDealtDamageThisAttack = false;
}

void AMeleeEnemyCharacter::PlayHitReactionMontage(FName MontageSection, float PlayRate)
{
	if (!bCanHitReact)
	{
		return;
	}

	if (HitReactionMnontage)
	{
		if (UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInst->Montage_Play(HitReactionMnontage, PlayRate);
			AnimInst->Montage_JumpToSection(MontageSection, HitReactionMnontage);
		}
	}

	bCanHitReact = false;
	const float HitReactTime = FMath::FRandRange(HitReactTimeMin, HitReactTimeMax);
	GetWorldTimerManager().SetTimer(HitReactTimer, this, &AMeleeEnemyCharacter::ResetHitReactTimer, HitReactTime);
}

void AMeleeEnemyCharacter::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AMeleeEnemyCharacter::ResetMeleeCanAttack()
{
	bCanAttack = true;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(
			FName("CanAttack"),
			true);
	}
}
