#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UUserWidget;
class UImage;
class UBorder;
class UTexture2D;

UCLASS()
class DEMO_CYY_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetAmmo(int32 InClipAmmo, int32 InTotalAmmo, float InReloadAlpha);
	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetWeaponSlotIcon(int32 SlotIndex, UTexture2D* Icon);
	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetActiveWeaponSlot(int32 SlotIndex);

protected:
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* TB_ClipAmmo = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* TB_ReserveAmmo = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UProgressBar* PB_Reload = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UUserWidget* ReloadButtonWidget = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UImage* WeaponIcon_1 = nullptr;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UImage* WeaponIcon_2 = nullptr;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UBorder* SlotBorder_1 = nullptr;
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UBorder* SlotBorder_2 = nullptr;
};
