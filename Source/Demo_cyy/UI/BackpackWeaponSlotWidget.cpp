#include "UI/BackpackWeaponSlotWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

void UBackpackWeaponSlotWidget::ApplyEntry(const FBackpackItemEntry& Entry, int32 InItemIndex)
{
	ItemIndex = InItemIndex;

	if (WeaponIcon)
	{
		WeaponIcon->SetBrushFromTexture(Entry.Icon, false);
		WeaponIcon->SetVisibility(Entry.Icon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(Entry.DisplayName);
	}

	if (Entry.ItemType == EBackpackItemType::Weapon)
	{
		if (AmmoText)
		{
			AmmoText->SetVisibility(ESlateVisibility::Visible);
			AmmoText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Entry.ClipAmmo, Entry.TotalAmmo)));
		}
		if (StackCountText)
		{
			StackCountText->SetVisibility(ESlateVisibility::Hidden);
		}
		if (EquippedBorder)
		{
			EquippedBorder->SetVisibility(ESlateVisibility::Visible);
			EquippedBorder->SetBrushColor(Entry.bIsEquipped
				? FLinearColor(0.18f, 0.52f, 1.0f, 1.0f)
				: FLinearColor(0.3f, 0.3f, 0.3f, 0.6f));
		}
	}
	else
	{
		if (AmmoText)
		{
			AmmoText->SetVisibility(ESlateVisibility::Hidden);
		}
		if (StackCountText)
		{
			StackCountText->SetVisibility(ESlateVisibility::Visible);
			StackCountText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Entry.StackCount)));
		}
		if (EquippedBorder)
		{
			EquippedBorder->SetVisibility(ESlateVisibility::Visible);
			EquippedBorder->SetBrushColor(FLinearColor(0.3f, 0.3f, 0.3f, 0.6f));
		}
	}

	// Medkit icon is dimmed when the stack is empty.
	if (WeaponIcon)
	{
		if (Entry.ItemType == EBackpackItemType::Medkit && Entry.StackCount <= 0)
		{
			WeaponIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 0.3f));
		}
		else
		{
			WeaponIcon->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}

	SetSelected(false);
}

void UBackpackWeaponSlotWidget::SetSelected(bool bSelected)
{
	if (SelectedBorder)
	{
		SelectedBorder->SetVisibility(bSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

FReply UBackpackWeaponSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (SuperReply.IsEventHandled())
	{
		return SuperReply;
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ItemIndex != INDEX_NONE)
	{
		OnItemClicked.Broadcast(ItemIndex);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UBackpackWeaponSlotWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FReply SuperReply = Super::NativeOnMouseButtonDoubleClick(InGeometry, InMouseEvent);
	if (SuperReply.IsEventHandled())
	{
		return SuperReply;
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ItemIndex != INDEX_NONE)
	{
		OnItemDoubleClicked.Broadcast(ItemIndex);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UBackpackWeaponSlotWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (SuperReply.IsEventHandled())
	{
		return SuperReply;
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ItemIndex != INDEX_NONE)
	{
		OnItemRightClicked.Broadcast(ItemIndex);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}
