#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BeginUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginMenuStartClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginMenuContinueClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginMenuSetClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBeginMenuEndClickedSignature);

UCLASS()
class DEMO_CYY_API UBeginUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void StartButtonClick();

	UFUNCTION()
	void ContinueButtonClick();

	UFUNCTION()
	void SetButtonClick();

	UFUNCTION()
	void EndButtonClick();

public:
	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FBeginMenuStartClickedSignature OnStartClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FBeginMenuContinueClickedSignature OnContinueClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FBeginMenuSetClickedSignature OnSetClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FBeginMenuEndClickedSignature OnEndClicked;

	void SetContinueButtonVisible(bool bVisible) const;
};
