#include "UI/ToastBannerWidget.h"

#include "Components/TextBlock.h"
#include "TimerManager.h"

void UToastBannerWidget::ShowMessage(const FString& Message, float DurationSeconds)
{
	if (ToastText)
	{
		ToastText->SetText(FText::FromString(Message));
	}

	// 使用 HitTestInvisible 避免遮挡下层 UI 的鼠标交互
	// （暂停时 World Timer 不会触发，若用 Visible 会永久阻挡点击）
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (ShowAnimation)
	{
		StopAnimation(ShowAnimation);
		PlayAnimation(ShowAnimation);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HideTimer);
		World->GetTimerManager().SetTimer(HideTimer, this, &UToastBannerWidget::Hide, DurationSeconds, false);
	}
}

void UToastBannerWidget::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}
