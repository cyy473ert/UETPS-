#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/UIFlowTypes.h"
#include "EndUserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndMenuRetryClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndMenuNextLevelClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndMenuReturnClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndMenuQuitClickedSignature);

UCLASS()
class DEMO_CYY_API UEndUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void RetryButtonClick();

	UFUNCTION()
	void NextLevelButtonClick();

	UFUNCTION()
	void ReturnButtonClick();

	UFUNCTION()
	void QuitButtonClick();

public:
	UFUNCTION(BlueprintCallable, Category="UI")
	void SetResultText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category="UI")
	void SetGameResultData(const FGameResultData& InResultData);

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FEndMenuRetryClickedSignature OnRetryClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FEndMenuNextLevelClickedSignature OnNextLevelClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FEndMenuReturnClickedSignature OnReturnClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FEndMenuQuitClickedSignature OnQuitClicked;

	void SetNextLevelButtonVisible(bool bVisible) const;
};
