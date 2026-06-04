#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UIManagerComponent.h"
#include "Input/Reply.h"
#include "BackpackWidget.generated.h"

class UBackpackWeaponSlotWidget;
class UButton;
class UImage;
class UPanelWidget;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackItemClickedSignature, int32, ItemIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackItemDoubleClickedSignature, int32, ItemIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackpackUseClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackpackDiscardClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackpackCloseClickedSignature);

UCLASS()
class DEMO_CYY_API UBackpackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UBackpackWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Backpack")
	void RebuildItems(const TArray<FBackpackItemEntry>& Entries);

	UFUNCTION(BlueprintCallable, Category="Backpack")
	void SetSelectedItem(int32 Index);

	UFUNCTION(BlueprintCallable, Category="Backpack")
	void ClearSelection();

	UFUNCTION(BlueprintCallable, Category="Backpack")
	void RefreshUseDiscardButtons();

	UFUNCTION(BlueprintPure, Category="Backpack")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	const FBackpackItemEntry* GetSelectedEntry() const;

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackItemClickedSignature OnItemClicked;

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackItemDoubleClickedSignature OnItemDoubleClicked;

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackUseClickedSignature OnUseClicked;

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackDiscardClickedSignature OnDiscardClicked;

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackCloseClickedSignature OnCloseClicked;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void HandleUseButtonClicked();

	UFUNCTION()
	void HandleDiscardButtonClicked();

	UFUNCTION()
	void HandleCloseButtonClicked();

	UFUNCTION()
	void HandleSlotClicked(int32 ItemIndex);

	UFUNCTION()
	void HandleSlotDoubleClicked(int32 ItemIndex);
	UFUNCTION()
	void HandleSlotRightClicked(int32 ItemIndex);

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UPanelWidget* ItemGridPanel = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UButton* CloseButton = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UImage* InfoIcon = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* InfoText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UButton* UseButton = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UButton* DiscardButton = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* UseButtonText = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Backpack", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UBackpackWeaponSlotWidget> WeaponSlotWidgetClass;

private:
	UPROPERTY(Transient)
	TArray<FBackpackItemEntry> CurrentEntries;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBackpackWeaponSlotWidget>> SlotWidgets;

	int32 SelectedIndex = INDEX_NONE;
};
