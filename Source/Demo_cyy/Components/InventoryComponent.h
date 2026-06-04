#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/UIFlowTypes.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryChangedSignature, int32, NewMedkitCount);

UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class DEMO_CYY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddMedkit(int32 Amount);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool ConsumeMedkit();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	EMedkitUseResult TryUseMedkit(EFlowState FlowState, bool bIsFullHealth);

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void ClearInventory();

	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetMedkitCount() const { return MedkitCount; }

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnInventoryChangedSignature OnInventoryChanged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	int32 MedkitCount = 0;
};
