#pragma once

#include "CoreMinimal.h"
#include "UIFlowTypes.generated.h"

UENUM(BlueprintType)
enum class EFlowState : uint8
{
	MainMenu UMETA(DisplayName="Main Menu"),
	InGame UMETA(DisplayName="In Game"),
	Paused UMETA(DisplayName="Paused"),
	Result UMETA(DisplayName="Result"),
	Transition UMETA(DisplayName="Transition")
};

UENUM(BlueprintType)
enum class EMedkitUseResult : uint8
{
	Success UMETA(DisplayName="Success"),
	NoItem UMETA(DisplayName="No Item"),
	FullHealth UMETA(DisplayName="Full Health"),
	FlowBlocked UMETA(DisplayName="Flow Blocked")
};

// Game result types (moved from UIManagerComponent.h to break circular include)

UENUM(BlueprintType)
enum class EGameResultType : uint8
{
	Win UMETA(DisplayName="Win"),
	Lose UMETA(DisplayName="Lose")
};

UENUM(BlueprintType)
enum class EGameEndReason : uint8
{
	AllEnemiesCleared UMETA(DisplayName="All Enemies Cleared"),
	PlayerDead UMETA(DisplayName="Player Dead"),
	ForceEnd UMETA(DisplayName="Force End")
};

USTRUCT(BlueprintType)
struct FGameResultData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow")
	EGameResultType ResultType = EGameResultType::Lose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow")
	int32 KillCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow")
	float ElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow")
	EGameEndReason EndReason = EGameEndReason::ForceEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flow")
	FDateTime Timestamp = FDateTime::UtcNow();
};
