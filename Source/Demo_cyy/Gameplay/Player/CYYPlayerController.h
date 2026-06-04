#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CYYPlayerController.generated.h"

UCLASS()
class DEMO_CYY_API ACYYPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACYYPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputMappingContext* CharacterContext;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* SuspendAction;
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* ToggleBackpackAction;
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* UseMedkitAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* AimAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* GunShotAction;
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* SwitchWeapon1Action;
	UPROPERTY(EditAnywhere, Category="Input")
	class UInputAction* SwitchWeapon2Action;

	void Move(const struct FInputActionValue& InputActionValue);
	void Look(const struct FInputActionValue& InputActionValue);
	void Jump();
	void StopJumping();
	void Suspend();
	void ToggleBackpack();
	void UseMedkit();
	void PickUp();
	void Aim();
	void StopAim();
	void Run();
	void StopRun();
	void GunShot();
	void Reload();
	void SwitchWeapon1();
	void SwitchWeapon2();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="True"))
	TObjectPtr<class UUIManagerComponent> UIManagerComponent = nullptr;

public:
	UFUNCTION(BlueprintPure, Category="UI")
	class UUIManagerComponent* GetUIManagerComponent() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	float AxisVectorX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	float AxisVectorY;
};
