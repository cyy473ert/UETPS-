#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UIManagerComponent.h"
#include "BackpackWeaponSlotWidget.generated.h"

class UBorder;
class UImage;
class UTextBlock;
struct FGeometry;
struct FPointerEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackSlotClickedSignature, int32, ItemIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackSlotDoubleClickedSignature, int32, ItemIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBackpackSlotRightClickedSignature, int32, ItemIndex);

UCLASS()
class DEMO_CYY_API UBackpackWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Backpack")
	void ApplyEntry(const FBackpackItemEntry& Entry, int32 InItemIndex);

	UFUNCTION(BlueprintCallable, Category="Backpack")
	void SetSelected(bool bSelected);

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackSlotClickedSignature OnItemClicked;

	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackSlotDoubleClickedSignature OnItemDoubleClicked;
	UPROPERTY(BlueprintAssignable, Category="Backpack|Event")
	FOnBackpackSlotRightClickedSignature OnItemRightClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UBorder* SelectedBorder = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UBorder* EquippedBorder = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UImage* WeaponIcon = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* AmmoText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* ItemNameText = nullptr;

	UPROPERTY(meta=(BindWidgetOptional), BlueprintReadOnly)
	UTextBlock* StackCountText = nullptr;

private:
	int32 ItemIndex = INDEX_NONE;
};
