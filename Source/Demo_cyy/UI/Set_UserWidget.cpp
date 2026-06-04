#include "Set_UserWidget.h"

#include "Components/Button.h"

void USet_UserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* ContinueButton = Cast<UButton>(GetWidgetFromName(TEXT("continue"))))
	{
		ContinueButton->OnClicked.AddUniqueDynamic(this, &USet_UserWidget::ContinueButtonClick);
	}

	if (UButton* SaveButton = Cast<UButton>(GetWidgetFromName(TEXT("SaveBtn"))))
	{
		SaveButton->OnClicked.AddUniqueDynamic(this, &USet_UserWidget::SaveButtonClick);
	}

	if (UButton* EndButton = Cast<UButton>(GetWidgetFromName(TEXT("END"))))
	{
		EndButton->OnClicked.AddUniqueDynamic(this, &USet_UserWidget::EndButtonClick);
	}

	if (UButton* ReturnButton = Cast<UButton>(GetWidgetFromName(TEXT("Return"))))
	{
		ReturnButton->OnClicked.AddUniqueDynamic(this, &USet_UserWidget::ReturnButtonClick);
	}
}

void USet_UserWidget::ContinueButtonClick()
{
	OnContinueClicked.Broadcast();
}

void USet_UserWidget::SaveButtonClick()
{
	OnSaveClicked.Broadcast();
}

void USet_UserWidget::EndButtonClick()
{
	OnEndClicked.Broadcast();
}

void USet_UserWidget::ReturnButtonClick()
{
	OnReturnClicked.Broadcast();
}
