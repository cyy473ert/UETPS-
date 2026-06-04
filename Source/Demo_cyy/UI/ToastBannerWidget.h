#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToastBannerWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class DEMO_CYY_API UToastBannerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowMessage(const FString& Message, float DurationSeconds = 2.0f);

protected:
	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* ToastText = nullptr;

	UPROPERTY(Transient, meta=(BindWidgetAnimOptional), BlueprintReadOnly)
	UWidgetAnimation* ShowAnimation = nullptr;

private:
	FTimerHandle HideTimer;

	void Hide();
};
