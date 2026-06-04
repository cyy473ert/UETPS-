#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interact.h"
#include "LockedDoor.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class UWidgetComponent;
class USoundBase;

UCLASS()
class DEMO_CYY_API ALockedDoor : public AActor, public IInteract
{
	GENERATED_BODY()

public:
	ALockedDoor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPointLightComponent> StatusLight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractHint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	TSubclassOf<UUserWidget> HintWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> OpenSound;

	bool bUnlocked = false;
	bool bOpened = false;
	FRotator ClosedRotation;

	void Open();

	// IInteract
	virtual void ShowInteractWidget_Implementation() override;
	virtual void HideInteractWidget_Implementation() override;
	virtual UWeapon* DataAsset_Implementation() override { return nullptr; }
	virtual void OnInteract_Implementation(ACYYCharacterFather* Interactor) override;
};
