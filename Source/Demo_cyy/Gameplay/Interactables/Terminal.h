#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Terminal.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;

UCLASS()
class DEMO_CYY_API ATerminal : public AActor
{
	GENERATED_BODY()

public:
	ATerminal();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> TerminalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> ScreenWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Display")
	float UpdateInterval = 0.5f;

	float Accumulator = 0.0f;
};
