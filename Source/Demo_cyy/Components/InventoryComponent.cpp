#include "Components/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddMedkit(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	MedkitCount += Amount;
	OnInventoryChanged.Broadcast(MedkitCount);
}

bool UInventoryComponent::ConsumeMedkit()
{
	if (MedkitCount <= 0)
	{
		return false;
	}

	MedkitCount = FMath::Max(0, MedkitCount - 1);
	OnInventoryChanged.Broadcast(MedkitCount);
	return true;
}

EMedkitUseResult UInventoryComponent::TryUseMedkit(EFlowState FlowState, bool bIsFullHealth)
{
	if (FlowState == EFlowState::Result || FlowState == EFlowState::Transition)
	{
		return EMedkitUseResult::FlowBlocked;
	}

	if (bIsFullHealth)
	{
		return EMedkitUseResult::FullHealth;
	}

	if (MedkitCount <= 0)
	{
		return EMedkitUseResult::NoItem;
	}

	ConsumeMedkit();
	return EMedkitUseResult::Success;
}

void UInventoryComponent::ClearInventory()
{
	if (MedkitCount == 0)
	{
		return;
	}

	MedkitCount = 0;
	OnInventoryChanged.Broadcast(MedkitCount);
}
