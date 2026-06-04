#include "BeginUserWidget.h"

#include "Components/Button.h"
#include "Components/Widget.h"

void UBeginUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* StartButton = Cast<UButton>(GetWidgetFromName(TEXT("Start"))))
	{
		StartButton->OnClicked.AddUniqueDynamic(this, &UBeginUserWidget::StartButtonClick);
	}

	if (UButton* ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("ContinueBtn"))))
	{
		ContinueButton->OnClicked.AddUniqueDynamic(this, &UBeginUserWidget::ContinueButtonClick);
		ContinueButton->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UButton* SetButton = Cast<UButton>(GetWidgetFromName(TEXT("Set"))))
	{
		SetButton->OnClicked.AddUniqueDynamic(this, &UBeginUserWidget::SetButtonClick);
	}

	if (UButton* EndButton = Cast<UButton>(GetWidgetFromName(TEXT("End"))))
	{
		EndButton->OnClicked.AddUniqueDynamic(this, &UBeginUserWidget::EndButtonClick);
	}
}

void UBeginUserWidget::StartButtonClick()
{
	OnStartClicked.Broadcast();
}

void UBeginUserWidget::ContinueButtonClick()
{
	OnContinueClicked.Broadcast();
}

void UBeginUserWidget::SetButtonClick()
{
	OnSetClicked.Broadcast();
}

void UBeginUserWidget::EndButtonClick()
{
	OnEndClicked.Broadcast();
}

void UBeginUserWidget::SetContinueButtonVisible(bool bVisible) const
{
	if (UWidget* ContinueBtn = GetWidgetFromName(TEXT("ContinueBtn")))
	{
		ContinueBtn->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
