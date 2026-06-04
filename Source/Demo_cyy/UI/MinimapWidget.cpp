#include "UI/MinimapWidget.h"

#include "AI/Enemy/EnemyBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HealthComponent.h"
#include "Components/Image.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/MinimapTrackingSubsystem.h"
#include "TimerManager.h"

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	StartTracking();
}

void UMinimapWidget::NativeDestruct()
{
	StopTracking();
	Super::NativeDestruct();
}

void UMinimapWidget::StartTracking()
{
	if (UpdateInterval <= 0.f)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(UpdateTimer, this, &UMinimapWidget::RefreshMinimap, UpdateInterval, true);
	}
}

void UMinimapWidget::StopTracking()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(UpdateTimer);
	}
}

void UMinimapWidget::RefreshMinimap()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UMinimapTrackingSubsystem* Subsystem = World->GetSubsystem<UMinimapTrackingSubsystem>();
	if (!Subsystem || !EnemyLayer)
	{
		return;
	}

	const FVector PlayerPos = Pawn->GetActorLocation();
	const float PlayerYaw = PC->GetControlRotation().Yaw;
	UsedThisFrame.Reset();

	for (const TWeakObjectPtr<AEnemyBase>& WeakEnemy : Subsystem->GetActiveEnemies())
	{
		AEnemyBase* Enemy = WeakEnemy.Get();
		if (!Enemy)
		{
			continue;
		}

		UHealthComponent* Health = Enemy->GetHealthComponent();
		if (!Health || Health->IsDead())
		{
			continue;
		}

		EMonsterType Type = EMonsterType::Melee;
		if (const UMonster* MonsterData = Enemy->GetMonsterData())
		{
			Type = MonsterData->Type;
		}

		UUserWidget* Blip = nullptr;
		if (FBlipEntry* Existing = ActiveBlips.Find(WeakEnemy))
		{
			Blip = Existing->Widget;
		}
		else
		{
			Blip = GetOrCreateBlip(Type);
			if (!Blip)
			{
				continue;
			}
			ActiveBlips.Add(WeakEnemy, FBlipEntry{ Blip, Type });
		}

		PositionBlip(Blip, WorldToMinimap(Enemy->GetActorLocation(), PlayerPos, PlayerYaw));
		Blip->SetVisibility(ESlateVisibility::Visible);
		UsedThisFrame.Add(WeakEnemy);
	}

	RecycleUnusedBlips();
}

FVector2D UMinimapWidget::WorldToMinimap(FVector WorldPos, FVector PlayerPos, float PlayerYaw) const
{
	const float SafeExtent = FMath::Max(WorldExtent, 1.f);
	const FVector Delta = WorldPos - PlayerPos;
	const float NX = Delta.X / SafeExtent;
	const float NY = Delta.Y / SafeExtent;

	const float YawRad = FMath::DegreesToRadians(-PlayerYaw);
	float RX = NX * FMath::Cos(YawRad) - NY * FMath::Sin(YawRad);
	float RY = NX * FMath::Sin(YawRad) + NY * FMath::Cos(YawRad);

	const float DistSq = RX * RX + RY * RY;
	if (DistSq > 1.f)
	{
		const float Scale = FMath::InvSqrt(DistSq);
		RX *= Scale;
		RY *= Scale;
	}

	return FVector2D(RY * MinimapRadius, -RX * MinimapRadius);
}

UUserWidget* UMinimapWidget::GetOrCreateBlip(EMonsterType Type)
{
	TArray<FBlipEntry>& Pool = (Type == EMonsterType::Melee) ? MeleeBlipPool : RangedBlipPool;
	TSubclassOf<UUserWidget> BlipClass = (Type == EMonsterType::Melee) ? MeleeBlipClass : RangedBlipClass;

	UUserWidget* Blip = nullptr;
	if (Pool.Num() > 0)
	{
		Blip = Pool.Pop().Widget;
	}
	else if (BlipClass)
	{
		Blip = CreateWidget<UUserWidget>(this, BlipClass);
	}

	if (Blip && EnemyLayer && !Blip->GetParent())
	{
		if (UCanvasPanelSlot* CanvasSlot = EnemyLayer->AddChildToCanvas(Blip))
		{
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f));
			CanvasSlot->SetSize(FVector2D(12.f, 12.f));
		}
	}

	return Blip;
}

void UMinimapWidget::PositionBlip(UUserWidget* Blip, FVector2D CenterPos) const
{
	if (!Blip || !EnemyLayer)
	{
		return;
	}

	FVector2D LayerSize = EnemyLayer->GetCachedGeometry().GetLocalSize();
	if (LayerSize.IsNearlyZero())
	{
		if (const UCanvasPanelSlot* EnemyLayerSlot = Cast<UCanvasPanelSlot>(EnemyLayer->Slot))
		{
			LayerSize = EnemyLayerSlot->GetSize();
		}
	}
	const FVector2D LayerCenter = LayerSize * 0.5f;

	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Blip->Slot))
	{
		CanvasSlot->SetPosition(CenterPos + LayerCenter);
	}
}

void UMinimapWidget::RecycleUnusedBlips()
{
	TArray<TWeakObjectPtr<AEnemyBase>> KeysToRemove;
	for (const TPair<TWeakObjectPtr<AEnemyBase>, FBlipEntry>& Pair : ActiveBlips)
	{
		if (UsedThisFrame.Contains(Pair.Key))
		{
			continue;
		}

		FBlipEntry Entry = Pair.Value;
		if (Entry.Widget)
		{
			Entry.Widget->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (Entry.Type == EMonsterType::Melee)
		{
			MeleeBlipPool.Add(Entry);
		}
		else
		{
			RangedBlipPool.Add(Entry);
		}

		KeysToRemove.Add(Pair.Key);
	}

	for (const TWeakObjectPtr<AEnemyBase>& Key : KeysToRemove)
	{
		ActiveBlips.Remove(Key);
	}
}
