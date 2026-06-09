// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Types/UIFlowTypes.h"
#include "CYYGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_CYY_API ACYYGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ACYYGameModeBase();
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void BindPlayerDeath();
	void RefreshAliveEnemyCount();

	UFUNCTION()
	void OnPlayerDeath(AActor* DeadActor);

	UFUNCTION()
	void OnEnemyDeath(AActor* DeadActor);

	void HandleGameOver(bool bPlayerWon, EGameEndReason EndReason);
	void SaveGameProgress();

	// ── 关卡存档 ──
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level", meta = (AllowPrivateAccess = "true"))
	int32 LevelNumber = 1;

public:
	UFUNCTION(BlueprintPure, Category = "Level")
	int32 GetLevelNumber() const { return LevelNumber; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	void TriggerVictory();

private:
	UPROPERTY(Transient)
	bool bGameEnded = false;

	UPROPERTY(Transient)
	int32 AliveEnemyCount = 0;

	UPROPERTY(Transient)
	int32 KillCount = 0;

	UPROPERTY(Transient)
	double LevelStartTimeSeconds = 0.0;

};
