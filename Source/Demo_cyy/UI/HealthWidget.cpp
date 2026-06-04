#include "HealthWidget.h"

#include "Components/ProgressBar.h"

void UHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthWidget::SetHealthPercent(float InPercent)
{
	if (PB_Health)
	{
		// 血条显示值统一限制在 0~1。
		PB_Health->SetPercent(FMath::Clamp(InPercent, 0.0f, 1.0f));
	}
}
