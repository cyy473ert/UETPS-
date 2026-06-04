#include "AI/Enemy/EnemyBase.h"

#include "AI/Enemy/EnemyController.h"
#include "Animation/AnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Kismet/KismetMathLibrary.h"
#include "Subsystems/MinimapTrackingSubsystem.h"
#include "UI/EnemyHealthWidget.h"
#include "HealthComponent.h"
#include "Gameplay/Pickups/HealPickup.h"
AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	OverheadHealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadHealthBar"));
	OverheadHealthBar->SetupAttachment(GetMesh());
	OverheadHealthBar->SetWidgetSpace(EWidgetSpace::Screen);
	OverheadHealthBar->SetDrawSize(FVector2D(200.f, 20.f));
	OverheadHealthBar->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	EnemyController = Cast<AEnemyController>(GetController());
	SpawnOrigin = GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Actor=%s, OverheadHealthBar=%s, OverheadHealthBarClass=%s"),
		*GetName(),
		OverheadHealthBar ? TEXT("Valid") : TEXT("NULL"),
		OverheadHealthBarClass ? *OverheadHealthBarClass->GetName() : TEXT("NULL"));

	if (!OverheadHealthBar)
	{
		UE_LOG(LogTemp, Error, TEXT("OverheadHealthBar component is NULL - C++ class may not match BP"));
		return;
	}
	if (!OverheadHealthBarClass)
	{
		UE_LOG(LogTemp, Error, TEXT("OverheadHealthBarClass is NULL - BP Class Defaults not saved or level override"));
		return;
	}

	OverheadHealthBar->SetWidgetClass(OverheadHealthBarClass);
	OverheadHealthBar->InitWidget();

	if (UUserWidget* Widget = OverheadHealthBar->GetWidget())
	{
		Widget->SetVisibility(ESlateVisibility::Visible);

		if (UEnemyHealthWidget* HW = Cast<UEnemyHealthWidget>(Widget))
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast OK, PB_Health=%s, HealthPercent=%f"),
				HW->PB_Health ? TEXT("Valid") : TEXT("NULL"),
				HealthComponent ? HealthComponent->GetHealthPercent() : -1.f);
			HW->SetHealthPercent(HealthComponent->GetHealthPercent());
		}

		UE_LOG(LogTemp, Warning, TEXT("OverheadHealthBar widget created: Widget=%s, Class=%s"),
			*Widget->GetName(), *OverheadHealthBarClass->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OverheadHealthBar InitWidget failed - GetWidget is null"));
	}

	OverheadHealthBar->SetVisibility(false);

	if (HealthComponent)
	{
		HealthComponent->OnHealthChanged.AddDynamic(this, &AEnemyBase::OnOverheadHealthChanged);
	}

	if (UWorld* World = GetWorld())
	{
		if (UMinimapTrackingSubsystem* Subsys = World->GetSubsystem<UMinimapTrackingSubsystem>())
		{
			Subsys->RegisterEnemy(this);
		}
	}
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UMinimapTrackingSubsystem* Subsys = World->GetSubsystem<UMinimapTrackingSubsystem>())
		{
			Subsys->UnregisterEnemy(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OverheadVisibilityAccumulator += DeltaTime;
	if (OverheadVisibilityAccumulator >= 0.3f)
	{
		OverheadVisibilityAccumulator = 0.f;
		UpdateOverheadHealthBar();
	}

	if (!HasAuthority())
	{
		return;
	}


}
void AEnemyBase::HandleDeath()
{
	Super::HandleDeath();
	if (OverheadHealthBar)
	{
		OverheadHealthBar->SetVisibility(false);
	}

	if (HasAuthority() && HealPickupClass && FMath::FRand() <= DropChance)
	{
		const FVector SpawnLoc = GetActorLocation() + FVector(0.0f, 0.0f, 20.0f);
		const FRotator SpawnRot = FRotator::ZeroRotator;
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		GetWorld()->SpawnActor<AHealPickup>(HealPickupClass, SpawnLoc, SpawnRot, Params);
	}
}
float AEnemyBase::GetAttackDamage() const
{
	return Monster ? Monster->Damage : 15.0f;
}

float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	const float AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!TargetActor && EventInstigator)
	{
		APawn* AttackerPawn = EventInstigator->GetPawn();
		if (AttackerPawn)
		{
			TargetActor = AttackerPawn;

			UBlackboardComponent* BB = EnemyController ? EnemyController->GetBlackboardComponent() : nullptr;
			if (BB)
			{
				BB->SetValueAsObject(TEXT("Target"), AttackerPawn);
				BB->SetValueAsObject(TEXT("TargetActor"), AttackerPawn);
				BB->SetValueAsBool(TEXT("HasTarget"), true);
			}
		}
	}

	return AppliedDamage;
}

void AEnemyBase::UpdateOverheadHealthBar()
{
	if (!OverheadHealthBar) return;

	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) { OverheadHealthBar->SetVisibility(false); return; }
	const APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn) { OverheadHealthBar->SetVisibility(false); return; }

	const float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
	OverheadHealthBar->SetVisibility(Dist <= OverheadHealthBarDistance);
}

void AEnemyBase::OnOverheadHealthChanged(UHealthComponent* HC, float NewHealth, float Delta, AActor* InstigatorActor)
{
	if (!OverheadHealthBar) return;
	UUserWidget* Widget = OverheadHealthBar->GetWidget();
	UE_LOG(LogTemp, Warning, TEXT("HealthChanged: NewHealth=%.1f, Percent=%.2f"),
		NewHealth, HC->GetHealthPercent());
	if (UEnemyHealthWidget* HealthWidget = Cast<UEnemyHealthWidget>(Widget))
	{
		HealthWidget->SetHealthPercent(HC->GetHealthPercent());
	}
}

