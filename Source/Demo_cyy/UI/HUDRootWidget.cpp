// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDRootWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UHUDRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MedkitButton)
	{
		MedkitButton->OnClicked.AddUniqueDynamic(this, &UHUDRootWidget::HandleMedkitButtonClicked);
	}
}

void UHUDRootWidget::SetMedkitState(int32 Count, bool bCanUse, const FText& DisabledReason)
{
	if (MedkitCountText)
	{
		MedkitCountText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), FMath::Max(0, Count))));
		MedkitCountText->SetToolTipText(DisabledReason);
	}

	if (MedkitButton)
	{
		MedkitButton->SetIsEnabled(bCanUse);
		MedkitButton->SetToolTipText(DisabledReason);
	}
}

void UHUDRootWidget::HandleMedkitButtonClicked()
{
	OnMedkitClicked.Broadcast();
}
