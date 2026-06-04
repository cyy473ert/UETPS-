#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataAssets/Monster.h"
#include "MinimapWidget.generated.h"

class AEnemyBase;
class UCanvasPanel;
class UImage;

USTRUCT()
struct FBlipEntry
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UUserWidget> Widget = nullptr;

	UPROPERTY()
	EMonsterType Type = EMonsterType::Melee;
};

UCLASS()
class DEMO_CYY_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float WorldExtent = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MinimapRadius = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float UpdateInterval = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Style")
	TSubclassOf<UUserWidget> MeleeBlipClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap|Style")
	TSubclassOf<UUserWidget> RangedBlipClass;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UImage> MinimapBackground;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UImage> PlayerIcon;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly)
	TObjectPtr<UCanvasPanel> EnemyLayer;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	FTimerHandle UpdateTimer;

	UPROPERTY(Transient)
	TMap<TWeakObjectPtr<AEnemyBase>, FBlipEntry> ActiveBlips;

	UPROPERTY(Transient)
	TArray<FBlipEntry> MeleeBlipPool;

	UPROPERTY(Transient)
	TArray<FBlipEntry> RangedBlipPool;

	TSet<TWeakObjectPtr<AEnemyBase>> UsedThisFrame;

	void StartTracking();
	void StopTracking();
	void RefreshMinimap();
	FVector2D WorldToMinimap(FVector WorldPos, FVector PlayerPos, float PlayerYaw) const;
	UUserWidget* GetOrCreateBlip(EMonsterType Type);
	void PositionBlip(UUserWidget* Blip, FVector2D CenterPos) const;
	void RecycleUnusedBlips();
};

