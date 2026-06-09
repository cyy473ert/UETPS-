#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interact.h"
#include "DataAssets/Weapon.h"
#include "Item.generated.h"

class USkeletalMeshComponent;
class USphereComponent;
class UWidgetComponent;
class UParticleSystem;
class USoundBase;
class ACYYCharacterFather;

UCLASS()
class DEMO_CYY_API AItem : public AActor, public IInteract
{
	GENERATED_BODY()

public:
	AItem();
	virtual void ShowInteractWidget_Implementation() override;
	virtual void HideInteractWidget_Implementation() override;
	virtual UWeapon* DataAsset_Implementation() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "True"))
	USkeletalMeshComponent* ItemMesh;

	// Dedicated pickup collider used by Interact trace channel.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "True"))
	USphereComponent* PickupCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = "True"))
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlash;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	UWeapon* WeaponDataAsset = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Weapon)
	TSubclassOf<UWeapon> WeaponDataAssetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* PickupSound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EquipSound = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SocketName, meta = (AllowPrivateAccess = "true"))
	FName EquitmentSocketName = TEXT("Weapon");

	UFUNCTION()
	void AttachtoCharacter(FName SocketName);

	UFUNCTION()
	void EquitmentWeapon();

	UFUNCTION()
	void ShootParticles();

	void DecrementAmmo();
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
};
