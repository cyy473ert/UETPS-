#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealPickup.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class USoundBase;

UCLASS()
class DEMO_CYY_API AHealPickup : public AActor
{
	GENERATED_BODY()

public:
	AHealPickup();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPickupSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup", meta=(AllowPrivateAccess="true"))
	USphereComponent* PickupSphere = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* PickupMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Pickup", meta=(AllowPrivateAccess="true", ClampMin="1"))
	int32 StackAmount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup|Sound", meta = (AllowPrivateAccess = "true"))
	USoundBase* PickupSound = nullptr;

	UPROPERTY(Transient)
	bool bPicked = false;
};

