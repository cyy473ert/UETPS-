#include "TerminalScreenWidget.h"

#include "Components/TextBlock.h"

void UTerminalScreenWidget::Refresh(bool bPower, bool bCard)
{
	if (PowerLine)
	{
		PowerLine->SetText(bPower
			? FText::FromString(TEXT("电力：通电"))
			: FText::FromString(TEXT("电力：未通电")));
		PowerLine->SetColorAndOpacity(bPower ? FLinearColor::Green : FLinearColor::Red);
	}

	if (CardLine)
	{
		CardLine->SetText(bCard
			? FText::FromString(TEXT("钥匙：发现"))
			: FText::FromString(TEXT("钥匙：未发现")));
		CardLine->SetColorAndOpacity(bCard ? FLinearColor::Green : FLinearColor::Red);
	}

	if (DoorLine)
	{
		if (bPower && bCard)
		{
			DoorLine->SetText(FText::FromString(TEXT("安全门：未锁住")));
			DoorLine->SetColorAndOpacity(FLinearColor::Green);
		}
		else
		{
			DoorLine->SetText(FText::FromString(TEXT("安全门：锁住")));
			DoorLine->SetColorAndOpacity(FLinearColor::Red);
		}
	}
}
