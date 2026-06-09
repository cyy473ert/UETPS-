// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDRootWidget.generated.h"

class UHealthWidget;
class UAmmoWidget;
class UEnemyHealthWidget;
class UButton;
class UCanvasPanel;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHUDMedkitClickedSignature);

UCLASS()
class DEMO_CYY_API UHUDRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	// Root仅做容器：暴露子控件引用给UIManager使用。
	UFUNCTION(BlueprintPure, Category="HUD")
	UHealthWidget* GetHealthWidget() const { return HealthWidget; }
	UFUNCTION(BlueprintPure, Category="HUD")
	UAmmoWidget* GetAmmoWidget() const { return AmmoWidget; }
	UFUNCTION(BlueprintPure, Category="HUD")
	UEnemyHealthWidget* GetEnemyHealthWidget() const { return EnemyHealthWidget; }
	UFUNCTION(BlueprintPure, Category = "HUD")
	UCanvasPanel* GetHitNumberLayer() const { return HitNumberLayer; }

	UPROPERTY(BlueprintAssignable, Category="HUD|Event")
	FHUDMedkitClickedSignature OnMedkitClicked;

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetMedkitState(int32 Count, bool bCanUse, const FText& DisabledReason);

protected:
	// 玩家血条
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UHealthWidget* HealthWidget = nullptr;

	// 蓝图里放置并绑定的弹药子控件。
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UAmmoWidget* AmmoWidget = nullptr;

	// 敌人血条
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UEnemyHealthWidget* EnemyHealthWidget = nullptr;
	// 伤害数字父画布
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* HitNumberLayer = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UButton* MedkitButton = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* MedkitCountText = nullptr;

private:
	UFUNCTION()
	void HandleMedkitButtonClicked();
};
