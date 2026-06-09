// Fill out your copyright notice in the Description page of Project Settings.


#include "CYYGameModeBase.h"
#include "Combat/CombatCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/UIManagerComponent.h"
#include "Gameplay/Player/CYYPlayerController.h"
#include "AI/Enemy/EnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Save/CYYSaveManager.h"
#include "Save/CYYSaveGame.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Components/LevelProgressionComponent.h"
#include "Engine/World.h"

ACYYGameModeBase::ACYYGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = ACYYPlayerController::StaticClass();
}

void ACYYGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	bGameEnded = false;
	KillCount = 0;
	LevelStartTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

	// 自动从地图名推断关卡编号
	{
		const FString LevelName = UGameplayStatics::GetCurrentLevelName(this, true);
		if (LevelName.Contains(TEXT("Level2")))      { LevelNumber = 2; }
		else if (LevelName.Contains(TEXT("Level1"))) { LevelNumber = 1; }
	}

	BindPlayerDeath();
	RefreshAliveEnemyCount();

	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), EnemyActors);
	for (AActor* EnemyActor : EnemyActors)
	{
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(EnemyActor))
		{
			if (UHealthComponent* EnemyHealth = Enemy->GetHealthComponent())
			{
				EnemyHealth->OnDeath.AddUniqueDynamic(this, &ACYYGameModeBase::OnEnemyDeath);
			}
		}
	}
}

void ACYYGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bGameEnded)
	{
		return;
	}

	const int32 PrevAlive = AliveEnemyCount;
	RefreshAliveEnemyCount();
	if (LevelNumber == 2 && PrevAlive > 0 && AliveEnemyCount == 0)
	{
		HandleGameOver(true, EGameEndReason::AllEnemiesCleared);
	}
}

void ACYYGameModeBase::BindPlayerDeath()
{
	ACombatCharacter* PlayerCharacter = Cast<ACombatCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!PlayerCharacter)
	{
		return;
	}

	if (UHealthComponent* Health = PlayerCharacter->GetHealthComponent())
	{
		Health->OnDeath.AddUniqueDynamic(this, &ACYYGameModeBase::OnPlayerDeath);
	}
}

void ACYYGameModeBase::RefreshAliveEnemyCount()
{
	int32 NewAliveCount = 0;
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemyBase::StaticClass(), EnemyActors);
	for (AActor* EnemyActor : EnemyActors)
	{
		const AEnemyBase* Enemy = Cast<AEnemyBase>(EnemyActor);
		if (!Enemy)
		{
			continue;
		}

		const UHealthComponent* EnemyHealth = Enemy->GetHealthComponent();
		if (!EnemyHealth || !EnemyHealth->IsDead())
		{
			++NewAliveCount;
		}
	}
	AliveEnemyCount = NewAliveCount;
}

void ACYYGameModeBase::OnPlayerDeath(AActor* DeadActor)
{
	HandleGameOver(false, EGameEndReason::PlayerDead);
}

void ACYYGameModeBase::OnEnemyDeath(AActor* DeadActor)
{
	if (bGameEnded)
	{
		return;
	}

	const int32 PrevAlive = AliveEnemyCount;
	++KillCount;

	// 击杀给经验
	if (AEnemyBase* DeadEnemy = Cast<AEnemyBase>(DeadActor))
	{
		if (ACYYCharacterFather* Player = Cast<ACYYCharacterFather>(
			UGameplayStatics::GetPlayerCharacter(this, 0)))
		{
			if (ULevelProgressionComponent* LPC = Player->GetLevelComponent())
			{
				LPC->AddXP(DeadEnemy->XPValue);
			}
		}
	}

	RefreshAliveEnemyCount();
	if (LevelNumber == 2 && PrevAlive > 0 && AliveEnemyCount == 0)
	{
		HandleGameOver(true, EGameEndReason::AllEnemiesCleared);
	}
}

void ACYYGameModeBase::HandleGameOver(bool bPlayerWon, EGameEndReason EndReason)
{
	if (bGameEnded)
	{
		return;
	}
	bGameEnded = true;

	// ── 胜利时自动存档 ──
	if (bPlayerWon)
	{
		SaveGameProgress();
	}

	FGameResultData ResultData;
	ResultData.ResultType = bPlayerWon ? EGameResultType::Win : EGameResultType::Lose;
	ResultData.KillCount = KillCount;
	ResultData.ElapsedTime = GetWorld() ? (GetWorld()->GetTimeSeconds() - LevelStartTimeSeconds) : 0.0f;
	ResultData.EndReason = EndReason;
	ResultData.Timestamp = FDateTime::UtcNow();

	UE_LOG(LogTemp, Log, TEXT("GameOver: %s kills=%d elapsed=%.2f reason=%d ts=%s"),
		bPlayerWon ? TEXT("Win") : TEXT("Lose"),
		ResultData.KillCount,
		ResultData.ElapsedTime,
		static_cast<int32>(ResultData.EndReason),
		*ResultData.Timestamp.ToIso8601());
	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			UIManager->ShowGameResult(ResultData);
		}
	}
}

void ACYYGameModeBase::SaveGameProgress()
{
	// 1. 加载或创建存档
	UCYYSaveGame* Save = FCYYSaveManager::LoadOrCreate();
	if (!Save) return;

	// 2. 收集玩家数据
	if (ACombatCharacter* PlayerChar = Cast<ACombatCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		if (ACYYCharacterFather* Father = Cast<ACYYCharacterFather>(PlayerChar))
		{
			FCYYSaveManager::CollectFromPlayer(Save, Father);
		}
	}

	// 3. 标记关卡完成、解锁下一关、更新当前关卡
	Save->bHasStartedGame = true;
	Save->MarkLevelCompleted(LevelNumber);
	Save->CurrentLevel = LevelNumber + 1;  // 继续游戏时跳到下一关

	// 4. 写入磁盘
	FCYYSaveManager::SaveGame(Save);

	UE_LOG(LogTemp, Log, TEXT("[GameMode] 关卡 %d 完成，存档已更新"), LevelNumber);
}

void ACYYGameModeBase::TriggerVictory()
{
	HandleGameOver(true, EGameEndReason::AllEnemiesCleared);
}
