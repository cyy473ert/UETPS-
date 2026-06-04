#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthWidget.generated.h"

class UProgressBar;

UCLASS()
class DEMO_CYY_API UEnemyHealthWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void SetHealthPercent(float InPercent);

	UPROPERTY(meta=(BindWidgetOptional))
	UProgressBar* PB_Health = nullptr;
};
