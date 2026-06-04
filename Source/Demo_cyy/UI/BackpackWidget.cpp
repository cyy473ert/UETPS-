#include "UI/BackpackWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "InputCoreTypes.h"
#include "UI/BackpackWeaponSlotWidget.h"
#include "UObject/ConstructorHelpers.h"

UBackpackWidget::UBackpackWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UBackpackWeaponSlotWidget> SlotWidgetClassFinder(TEXT("/Game/CYY/UI/Back/WBP_BackpackWeaponSlot"));
	if (SlotWidgetClassFinder.Succeeded())
	{
		WeaponSlotWidgetClass = SlotWidgetClassFinder.Class;
	}
}

void UBackpackWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UseButton)
	{
		UseButton->OnClicked.AddUniqueDynamic(this, &UBackpackWidget::HandleUseButtonClicked);
	}
	if (DiscardButton)
	{
		DiscardButton->OnClicked.AddUniqueDynamic(this, &UBackpackWidget::HandleDiscardButtonClicked);
	}
	if (CloseButton)
	{
		CloseButton->OnClicked.AddUniqueDynamic(this, &UBackpackWidget::HandleCloseButtonClicked);
	}

	RefreshUseDiscardButtons();
}

FReply UBackpackWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}

	const FVector2D ScreenPos = InMouseEvent.GetScreenSpacePosition();
	for (const UBackpackWeaponSlotWidget* SlotWidget : SlotWidgets)
	{
		if (!SlotWidget)
		{
			continue;
		}

		const ESlateVisibility SlotVisibility = SlotWidget->GetVisibility();
		if (SlotVisibility == ESlateVisibility::Collapsed || SlotVisibility == ESlateVisibility::Hidden)
		{
			continue;
		}

		if (SlotWidget->GetCachedGeometry().IsUnderLocation(ScreenPos))
		{
			return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
		}
	}

	ClearSelection();
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UBackpackWidget::RebuildItems(const TArray<FBackpackItemEntry>& Entries)
{
	CurrentEntries = Entries;
	SlotWidgets.Reset();
	SelectedIndex = INDEX_NONE;

	if (!ItemGridPanel)
	{
		return;
	}

	ItemGridPanel->ClearChildren();
	if (!WeaponSlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RebuildItems skipped: WeaponSlotWidgetClass is null."));
		return;
	}

	for (int32 Index = 0; Index < CurrentEntries.Num(); ++Index)
	{
		UBackpackWeaponSlotWidget* SlotWidget = CreateWidget<UBackpackWeaponSlotWidget>(this, WeaponSlotWidgetClass);
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->ApplyEntry(CurrentEntries[Index], Index);
		SlotWidget->OnItemClicked.AddUniqueDynamic(this, &UBackpackWidget::HandleSlotClicked);
		SlotWidget->OnItemDoubleClicked.AddUniqueDynamic(this, &UBackpackWidget::HandleSlotDoubleClicked);
		SlotWidget->OnItemRightClicked.AddUniqueDynamic(this, &UBackpackWidget::HandleSlotRightClicked);
		ItemGridPanel->AddChild(SlotWidget);
		SlotWidgets.Add(SlotWidget);
	}

	ClearSelection();
}

void UBackpackWidget::SetSelectedItem(int32 Index)
{
	if (!CurrentEntries.IsValidIndex(Index))
	{
		ClearSelection();
		return;
	}

	SelectedIndex = Index;
	for (int32 SlotIndex = 0; SlotIndex < SlotWidgets.Num(); ++SlotIndex)
	{
		if (SlotWidgets[SlotIndex])
		{
			SlotWidgets[SlotIndex]->SetSelected(SlotIndex == SelectedIndex);
		}
	}

	const FBackpackItemEntry& Entry = CurrentEntries[SelectedIndex];
	if (InfoIcon)
	{
		InfoIcon->SetBrushFromTexture(Entry.Icon, false);
		InfoIcon->SetVisibility(Entry.Icon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	if (InfoText)
	{
		FString Message = Entry.DisplayName.ToString();
		if (!Entry.Description.IsEmpty())
		{
			Message += LINE_TERMINATOR;
			Message += Entry.Description.ToString();
		}
		if (!Entry.bCanUse && !Entry.DisabledReason.IsEmpty())
		{
			Message += LINE_TERMINATOR;
			Message += Entry.DisabledReason.ToString();
		}
		InfoText->SetText(FText::FromString(Message));
	}

	RefreshUseDiscardButtons();
}

void UBackpackWidget::ClearSelection()
{
	SelectedIndex = INDEX_NONE;
	for (UBackpackWeaponSlotWidget* SlotWidget : SlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->SetSelected(false);
		}
	}

	if (InfoIcon)
	{
		InfoIcon->SetBrushFromTexture(nullptr, false);
		InfoIcon->SetVisibility(ESlateVisibility::Hidden);
	}
	if (InfoText)
	{
		InfoText->SetText(FText::FromString(TEXT("\u70b9\u51fb\u7269\u54c1\u67e5\u770b\u8be6\u60c5")));
	}

	RefreshUseDiscardButtons();
}

void UBackpackWidget::RefreshUseDiscardButtons()
{
	const FBackpackItemEntry* SelectedEntry = GetSelectedEntry();
	const bool bHasSelection = SelectedEntry != nullptr;
	const bool bCanUse = bHasSelection && SelectedEntry->bCanUse;
	const bool bCanDiscard = bHasSelection && (SelectedEntry->ItemType == EBackpackItemType::Weapon || SelectedEntry->StackCount > 0);

	if (UseButton)
	{
		UseButton->SetIsEnabled(bCanUse);
	}
	if (UseButtonText)
	{
		if (!bCanUse && bHasSelection && !SelectedEntry->DisabledReason.IsEmpty())
		{
			UseButtonText->SetText(SelectedEntry->DisabledReason);
		}
		else if (bHasSelection && !SelectedEntry->UseButtonText.IsEmpty())
		{
			UseButtonText->SetText(SelectedEntry->UseButtonText);
		}
		else
		{
			UseButtonText->SetText(FText::FromString(TEXT("使用")));
		}
	}
	if (DiscardButton)
	{
		DiscardButton->SetIsEnabled(bCanDiscard);
	}
}

const FBackpackItemEntry* UBackpackWidget::GetSelectedEntry() const
{
	return CurrentEntries.IsValidIndex(SelectedIndex) ? &CurrentEntries[SelectedIndex] : nullptr;
}

void UBackpackWidget::HandleUseButtonClicked()
{
	OnUseClicked.Broadcast();
}

void UBackpackWidget::HandleDiscardButtonClicked()
{
	OnDiscardClicked.Broadcast();
}

void UBackpackWidget::HandleCloseButtonClicked()
{
	OnCloseClicked.Broadcast();
}

void UBackpackWidget::HandleSlotClicked(int32 ItemIndex)
{
	SetSelectedItem(ItemIndex);
	OnItemClicked.Broadcast(ItemIndex);
}

void UBackpackWidget::HandleSlotDoubleClicked(int32 ItemIndex)
{
	SetSelectedItem(ItemIndex);
	OnItemDoubleClicked.Broadcast(ItemIndex);
}

void UBackpackWidget::HandleSlotRightClicked(int32 ItemIndex)
{
	SetSelectedItem(ItemIndex);
	OnDiscardClicked.Broadcast();
}
