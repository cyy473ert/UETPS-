#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CabinetKnock.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class USoundBase;

UCLASS()
class DEMO_CYY_API ACabinetKnock : public AActor
{
	GENERATED_BODY()

public:
	ACabinetKnock();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> CabinetMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> TriggerSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> KnockSound1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> KnockSound2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> KnockSound3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> VoiceLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> LoudKnockSound;

	bool bTriggered = false;
	int32 KnockStep = 0;
	FTimerHandle KnockTimer;

	UFUNCTION()
	void OnPlayerApproach(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void PlayKnock();
};
