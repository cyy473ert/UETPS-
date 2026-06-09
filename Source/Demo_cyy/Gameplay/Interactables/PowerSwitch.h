#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interact.h"
#include "PowerSwitch.generated.h"

class USceneComponent;
class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UUserWidget;
class USoundBase;

UCLASS()
class DEMO_CYY_API APowerSwitch : public AActor, public IInteract
{
	GENERATED_BODY()

public:
	APowerSwitch();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SwitchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InteractHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HintWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> ActivateSound;

	bool bActivated = false;

	// IInteract
	virtual void ShowInteractWidget_Implementation() override;
	virtual void HideInteractWidget_Implementation() override;
	virtual UWeapon* DataAsset_Implementation() override { return nullptr; }
	virtual void OnInteract_Implementation(ACYYCharacterFather* Interactor) override;
};
