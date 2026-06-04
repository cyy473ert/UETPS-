#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interact.h"
#include "KeyCardItem.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class USoundBase;
class ACYYCharacterFather;

UCLASS()
class DEMO_CYY_API AKeyCardItem : public AActor, public IInteract
{
	GENERATED_BODY()

public:
	AKeyCardItem();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CardMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> PickupSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> PickupSound;

	// IInteract
	virtual void ShowInteractWidget_Implementation() override;
	virtual void HideInteractWidget_Implementation() override;
	virtual UWeapon* DataAsset_Implementation() override { return nullptr; }
	virtual void OnInteract_Implementation(ACYYCharacterFather* Interactor) override;
};
