#include "UI/EndUserWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"

void UEndUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* RetryButton = Cast<UButton>(GetWidgetFromName(TEXT("Retry"))))
	{
		RetryButton->OnClicked.AddUniqueDynamic(this, &UEndUserWidget::RetryButtonClick);
	}

	if (UButton* NextLevelButton = Cast<UButton>(GetWidgetFromName(TEXT("NextLevelBtn"))))
	{
		NextLevelButton->OnClicked.AddUniqueDynamic(this, &UEndUserWidget::NextLevelButtonClick);
		NextLevelButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UButton* ReturnButton = Cast<UButton>(GetWidgetFromName(TEXT("Return"))))
	{
		ReturnButton->OnClicked.AddUniqueDynamic(this, &UEndUserWidget::ReturnButtonClick);
	}

	if (UButton* EndButton = Cast<UButton>(GetWidgetFromName(TEXT("End"))))
	{
		EndButton->OnClicked.AddUniqueDynamic(this, &UEndUserWidget::QuitButtonClick);
	}
}

void UEndUserWidget::RetryButtonClick()
{
	OnRetryClicked.Broadcast();
}

void UEndUserWidget::NextLevelButtonClick()
{
	OnNextLevelClicked.Broadcast();
}

void UEndUserWidget::ReturnButtonClick()
{
	OnReturnClicked.Broadcast();
}

void UEndUserWidget::QuitButtonClick()
{
	OnQuitClicked.Broadcast();
}

void UEndUserWidget::SetResultText(const FText& InText)
{
	if (UTextBlock* ResultText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ResultText"))))
	{
		ResultText->SetText(InText);
	}
}

void UEndUserWidget::SetGameResultData(const FGameResultData& InResultData)
{
	if (UTextBlock* KillCountText = Cast<UTextBlock>(GetWidgetFromName(TEXT("KillCountText"))))
	{
		KillCountText->SetText(FText::AsNumber(InResultData.KillCount));
	}

	if (UTextBlock* ElapsedTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("ElapsedTimeText"))))
	{
		ElapsedTimeText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), InResultData.ElapsedTime)));
	}
}

void UEndUserWidget::SetNextLevelButtonVisible(bool bVisible) const
{
	if (UWidget* NextLevelBtn = GetWidgetFromName(TEXT("NextLevelBtn")))
	{
		NextLevelBtn->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
