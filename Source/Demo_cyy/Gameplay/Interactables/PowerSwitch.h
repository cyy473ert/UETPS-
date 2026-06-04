#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interact.h"
#include "PowerSwitch.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class USoundBase;

UCLASS()
class DEMO_CYY_API APowerSwitch : public AActor, public IInteract
{
	GENERATED_BODY()

public:
	APowerSwitch();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SwitchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSubclassOf<UUserWidget> HintWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> ActivateSound;

	bool bActivated = false;

	// IInteract
	virtual void ShowInteractWidget_Implementation() override;
	virtual void HideInteractWidget_Implementation() override;
	virtual UWeapon* DataAsset_Implementation() override { return nullptr; }
	virtual void OnInteract_Implementation(ACYYCharacterFather* Interactor) override;
};
