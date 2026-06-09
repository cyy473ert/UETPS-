// Fill out your copyright notice in the Description page of Project Settings.

#include "AmmoWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Border.h"
#include "Components/Image.h"

void UAmmoWidget::SetAmmo(int32 InClipAmmo, int32 InTotalAmmo, float InReloadAlpha)
{
	if (TB_ClipAmmo)
	{
		TB_ClipAmmo->SetText(FText::AsNumber(InClipAmmo));
	}

	if (TB_ReserveAmmo)
	{
		TB_ReserveAmmo->SetText(FText::AsNumber(InTotalAmmo));
	}

	if (PB_Reload)
	{
		PB_Reload->SetPercent(FMath::Clamp(InReloadAlpha, 0.0f, 1.0f));
	}

	if (ReloadButtonWidget)
	{
		const bool bShow = (InClipAmmo <= 0 && InTotalAmmo > 0 && InReloadAlpha <= 0.0f);
		ReloadButtonWidget->SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UAmmoWidget::SetWeaponSlotIcon(int32 SlotIndex, UTexture2D* Icon)
{
	UImage* SlotImage = nullptr;
	switch (SlotIndex)
	{
	case 0: SlotImage = WeaponIcon_1; break;
	case 1: SlotImage = WeaponIcon_2; break;
	default: break;
	}

	if (!SlotImage)
	{
		return;
	}

	if (Icon)
	{
		SlotImage->SetBrushFromTexture(Icon);
		SlotImage->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SlotImage->SetBrushFromTexture(nullptr);
		SlotImage->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UAmmoWidget::SetActiveWeaponSlot(int32 SlotIndex)
{
	TArray<UBorder*> Borders = {SlotBorder_1, SlotBorder_2};
	const FLinearColor ActiveColor(0.18f, 0.52f, 1.0f, 1.0f);
	const FLinearColor InactiveColor(0.2f, 0.2f, 0.2f, 0.6f);

	for (int32 Index = 0; Index < Borders.Num(); ++Index)
	{
		if (Borders[Index])
		{
			Borders[Index]->SetBrushColor(Index == SlotIndex ? ActiveColor : InactiveColor);
		}
	}
}

void UAmmoWidget::SetLevelInfo(int32 Level, float CurrentHP, float MaxHP, float DamageMult)
{
	if (TB_Level)
	{
		TB_Level->SetText(FText::FromString(FString::Printf(TEXT("Lv.%d"), Level)));
	}
	if (TB_HPValue)
	{
		TB_HPValue->SetText(FText::FromString(
			FString::Printf(TEXT("%.0f/%.0f"), CurrentHP, MaxHP)));
	}
	if (TB_DamageMult)
	{
		TB_DamageMult->SetText(FText::FromString(
			FString::Printf(TEXT("%.1fx"), DamageMult)));
	}
}
