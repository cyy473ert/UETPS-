#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Set_UserWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPauseMenuContinueClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPauseMenuSaveClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPauseMenuEndClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPauseMenuReturnClickedSignature);

UCLASS()
class DEMO_CYY_API USet_UserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ContinueButtonClick();

	UFUNCTION()
	void SaveButtonClick();

	UFUNCTION()
	void EndButtonClick();

	UFUNCTION()
	void ReturnButtonClick();

public:
	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FPauseMenuContinueClickedSignature OnContinueClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FPauseMenuSaveClickedSignature OnSaveClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FPauseMenuEndClickedSignature OnEndClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FPauseMenuReturnClickedSignature OnReturnClicked;
};
