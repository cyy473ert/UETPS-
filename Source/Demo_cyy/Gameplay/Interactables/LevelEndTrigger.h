#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelEndTrigger.generated.h"

class UBoxComponent;

UCLASS()
class DEMO_CYY_API ALevelEndTrigger : public AActor
{
	GENERATED_BODY()

public:
	ALevelEndTrigger();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> EndBox;

	bool bTriggered = false;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
