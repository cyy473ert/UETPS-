#include "AI/Enemy/RangedEnemyCharacter.h"

#include "AI/Enemy/EnemyController.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DataAssets/RangedMonsterData.h"
#include "GameFramework/DamageType.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ARangedEnemyCharacter::ARangedEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(GetRootComponent());

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ARangedEnemyCharacter::BeginPlay()
{
	if (bBeginPlayInitialized || bBeginPlayInitializing)
	{
		return;
	}
	bBeginPlayInitializing = true;

	if (!Monster)
	{
		Monster = NewObject<URangedMonsterData>(this);
	}

	Super::BeginPlay();

	EnemyController = Cast<AEnemyController>(GetController());
	DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &ARangedEnemyCharacter::OnDetectOverlap);
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ARangedEnemyCharacter::OnCombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ARangedEnemyCharacter::OnCombatRangeEndOverlap);

	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		const FVector WorldPatrolPoint1 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint1);
		const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(GetActorTransform(), PatrolPoint2);
		UBlackboardComponent* BB = EnemyController->GetBlackboardComponent();
		BB->SetValueAsBool(TEXT("CanAttack"), true);
		BB->SetValueAsBool(TEXT("IsAiming"), false);
		BB->SetValueAsBool(TEXT("TooClose"), false);
		BB->SetValueAsBool(TEXT("HasTarget"), false);
		BB->SetValueAsBool(TEXT("InAttackRange"), false);
		BB->SetValueAsVector(TEXT("PatrolPoint1"), WorldPatrolPoint1);
		BB->SetValueAsVector(TEXT("PatrolPoint2"), WorldPatrolPoint2);
		EnemyController->RunBehaviorTree(BehaviorTree);
	}

	UE_LOG(LogTemp, Warning, TEXT("DetectSphere: Radius=%f, Enabled=%d, ObjType=%d, PawnResponse=%d"),
		DetectSphere ? DetectSphere->GetScaledSphereRadius() : -1.0f,
		DetectSphere ? (int32)DetectSphere->GetCollisionEnabled() : -1,
		DetectSphere ? (int32)DetectSphere->GetCollisionObjectType() : -1,
		DetectSphere ? (int32)DetectSphere->GetCollisionResponseToChannel(ECC_Pawn) : -1);

	bBeginPlayInitialized = true;
	bBeginPlayInitializing = false;
}

void ARangedEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRetreating)
	{
		ApplyRetreatMovement(DeltaTime);
	}

	if (bIsCombatStrafing)
	{
		AddMovementInput(StrafeDirection, StrafeSpeed / 600.f);
		StrafeTimeRemaining -= DeltaTime;
		if (StrafeTimeRemaining <= 0.f)
		{
			bIsCombatStrafing = false;
		}
	}

	if (!EnemyController || !EnemyController->GetBlackboardComponent() || !TargetActor)
	{
		return;
	}

	DistanceCheckAccumulator += DeltaTime;
	if (DistanceCheckAccumulator < 0.15f)
	{
		return;
	}
	DistanceCheckAccumulator = 0.0f;

	const float Distance = FVector::Distance(GetActorLocation(), TargetActor->GetActorLocation());
	const bool bTooClose = Monster && Distance < Monster->PreferDistanceMin;
	const bool bInRange = Monster && Distance <= Monster->AttackRange;

	UBlackboardComponent* BB = EnemyController->GetBlackboardComponent();
	BB->SetValueAsBool(TEXT("TooClose"), bTooClose);
	BB->SetValueAsBool(TEXT("InAttackRange"), bInRange);
}

void ARangedEnemyCharacter::StartRetreat()
{
	AActor* RetreatTarget = TargetActor;
	if (!RetreatTarget && EnemyController && EnemyController->GetBlackboardComponent())
	{
		RetreatTarget = Cast<AActor>(EnemyController->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
		TargetActor = RetreatTarget;
	}

	if (!RetreatTarget)
	{
		return;
	}

	const FVector AwayDir = (GetActorLocation() - RetreatTarget->GetActorLocation()).GetSafeNormal2D();
	if (AwayDir.IsNearlyZero())
	{
		return;
	}

	if (FMath::FRand() < RetreatStrafeChance)
	{
		const bool bLeft = FMath::RandBool();
		const float YawOffset = bLeft ? -60.0f : 60.0f;
		RetreatDirection = AwayDir.RotateAngleAxis(YawOffset, FVector::UpVector).GetSafeNormal2D();
	}
	else
	{
		RetreatDirection = AwayDir;
	}

	bIsRetreating = true;
	RetreatTimeRemaining = RetreatDuration;
}

void ARangedEnemyCharacter::ApplyRetreatMovement(float DeltaTime)
{
	RetreatTimeRemaining -= DeltaTime;
	if (RetreatTimeRemaining <= 0.0f)
	{
		StopRetreat();
		return;
	}

	const float InputScale = FMath::Clamp(RetreatSpeed / 600.0f, 0.0f, 1.0f);
	AddMovementInput(RetreatDirection, InputScale);
}

void ARangedEnemyCharacter::StopRetreat()
{
	bIsRetreating = false;
	RetreatDirection = FVector::ZeroVector;
	RetreatTimeRemaining = 0.0f;
}

void ARangedEnemyCharacter::StartCombatStrafe()
{
	const bool bLeft = FMath::RandBool();
	StrafeDirection = bLeft ? -GetActorRightVector() : GetActorRightVector();
	bIsCombatStrafing = true;
	StrafeTimeRemaining = StrafeDuration;
}

void ARangedEnemyCharacter::StartRangedAttack()
{
	if (!Monster || !bCanAttack || bIsAiming)
	{
		return;
	}

	bCanAttack = false;
	bIsAiming = true;

	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), false);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAiming"), true);
	}

	if (TargetActor)
	{
		const FVector ToTarget = (TargetActor->GetActorLocation() - GetActorLocation()).GetSafeNormal2D();
		const FRotator FaceRot = ToTarget.Rotation();
		SetActorRotation(FRotator(0.f, FaceRot.Yaw, 0.f));
	}

	const float AimDelay = FMath::Max(0.0f, Monster->CastTime);
	if (AimDelay <= KINDA_SMALL_NUMBER)
	{
		OnAimComplete();
	}
	else
	{
		GetWorldTimerManager().SetTimer(AimTimer, this, &ARangedEnemyCharacter::OnAimComplete, AimDelay, false);
	}
}

void ARangedEnemyCharacter::OnAimComplete()
{
	bIsAiming = false;
	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("IsAiming"), false);
	}

	if (AttackMnontage)
	{
		if (UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInst->Montage_Play(AttackMnontage, 1.0f);
		}
	}
	else
	{
		FireHitscan();
	}

	const float Cooldown = Monster ? FMath::Max(0.1f, Monster->AttackInterval) : 2.0f;
	GetWorldTimerManager().SetTimer(CooldownTimer, this, &ARangedEnemyCharacter::ResetAttackCooldown, Cooldown, false);
}

void ARangedEnemyCharacter::OnFireNotify()
{
	FireHitscan();
}

void ARangedEnemyCharacter::FireHitscan()
{
	if (!HasAuthority() || !Monster)
	{
		return;
	}

	const FVector Start = GetMesh() ? GetMesh()->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const float TraceRange = FMath::Max(100.f, Monster->HitscanRange);
	const int32 ShotCount = FMath::Max(1, Monster->ShotsPerAttack);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	for (int32 i = 0; i < ShotCount; ++i)
	{
		const FVector End = Start + GetFiringDirection() * TraceRange;
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, QueryParams))
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.f);
			DrawDebugPoint(GetWorld(), Hit.Location, 10.f, FColor::Green, false, 2.f);
			UE_LOG(LogTemp, Warning, TEXT("Hit: %s  Loc:%s"), *Hit.GetActor()->GetName(), *Hit.Location.ToString());
			AActor* HitActor = Hit.GetActor();
			if (Cast<ACharacter>(HitActor))
			{
				UGameplayStatics::ApplyDamage(HitActor, GetAttackDamage(), GetController(), this, UDamageType::StaticClass());
			}
			
		}
	}
}

FVector ARangedEnemyCharacter::GetFiringDirection() const
{
	const FVector BaseDir = TargetActor
		? ((TargetActor->GetActorLocation() + FVector(0, 0, 50.f)) - GetActorLocation()).GetSafeNormal()
		: GetActorForwardVector();

	if (!Monster || Monster->FireSpread <= 0.0f)
	{
		return BaseDir;
	}

	return FMath::VRandCone(BaseDir, FMath::DegreesToRadians(Monster->FireSpread));
}

void ARangedEnemyCharacter::ResetAttackCooldown()
{
	bCanAttack = true;
	if (EnemyController && EnemyController->GetBlackboardComponent())
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("CanAttack"), true);
	}
}

void ARangedEnemyCharacter::OnDetectOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("!!! OnDetectOverlap FIRED, Other=%s"), Other ? *Other->GetName() : TEXT("None"));

	if (ACYYCharacterFather* Player = Cast<ACYYCharacterFather>(Other))
	{
		TargetActor = Player;
		UBlackboardComponent* BB = EnemyController ? EnemyController->GetBlackboardComponent() : nullptr;
		UE_LOG(LogTemp, Warning, TEXT("DetectOverlap: EnemyController=%s, Blackboard=%s"),
			EnemyController ? TEXT("Valid") : TEXT("Null"),
			BB ? TEXT("Valid") : TEXT("Null"));
		if (BB)
		{
			BB->SetValueAsObject(TEXT("TargetActor"), Player);
			BB->SetValueAsBool(TEXT("HasTarget"), true);
		}
	}
}

void ARangedEnemyCharacter::OnCombatRangeOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bSweep, const FHitResult& SweepResult)
{
	if (Cast<ACYYCharacterFather>(Other))
	{
		bInAttackRange = true;
		if (EnemyController && EnemyController->GetBlackboardComponent())
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}
}

void ARangedEnemyCharacter::OnCombatRangeEndOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 BodyIndex)
{
	if (Cast<ACYYCharacterFather>(Other))
	{
		bInAttackRange = false;
		if (EnemyController && EnemyController->GetBlackboardComponent())
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

void ARangedEnemyCharacter::PlayHitReactionMontage(FName MontageSection, float PlayRate)
{
	if (!bCanHitReact)
	{
		return;
	}

	if (HitReactionMnontage)
	{
		if (UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr)
		{
			AnimInst->Montage_Play(HitReactionMnontage, 0.7);
			AnimInst->Montage_JumpToSection(MontageSection, HitReactionMnontage);
		}
	}

	bCanHitReact = false;
	const float HitReactTime = FMath::FRandRange(HitReactTimeMin, HitReactTimeMax);
	GetWorldTimerManager().SetTimer(HitReactTimer, this, &ARangedEnemyCharacter::ResetHitReactTimer, HitReactTime);
}

void ARangedEnemyCharacter::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void ARangedEnemyCharacter::PerformRetreat()
{
	if (!Monster || !TargetActor)
	{
		return;
	}

	const FVector RetreatDir = (GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal2D();
	const FVector NewLocation = GetActorLocation() + RetreatDir * Monster->RetreatDistanceOnHit;
	SetActorLocation(NewLocation, true);
}

void ARangedEnemyCharacter::Hit_Implementation(const FHitResult& HitResult)
{
	PlayHitReactionMontage(FName("HitReaction"), 1.0f);
	StartRetreat();
}
