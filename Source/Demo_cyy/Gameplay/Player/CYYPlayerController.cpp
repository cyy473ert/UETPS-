#include "Gameplay/Player/CYYPlayerController.h"

#include "Components/UIManagerComponent.h"
#include "Combat/CombatCharacter.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "Engine/LocalPlayer.h"

ACYYPlayerController::ACYYPlayerController()
	: BaseTurnRate(45.0f)
	, BaseLookUpRate(45.0f)
	, AxisVectorX(0.0f)
	, AxisVectorY(0.0f)
{
	bReplicates = true;
	UIManagerComponent = CreateDefaultSubobject<UUIManagerComponent>(TEXT("UIManagerComponent"));
}

UUIManagerComponent* ACYYPlayerController::GetUIManagerComponent() const
{
	if (UIManagerComponent)
	{
		return UIManagerComponent;
	}
	return FindComponentByClass<UUIManagerComponent>();
}

void ACYYPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		if (ensure(CharacterContext))
		{
			if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
			{
				if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					InputSystem->AddMappingContext(CharacterContext, 0);
				}
			}
		}
	}

	if (UUIManagerComponent* UIManager = GetUIManagerComponent())
	{
		UIManager->InitializeForController(this);
	}
}

void ACYYPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UUIManagerComponent* UIManager = GetUIManagerComponent())
	{
		UIManager->InitializeForCharacter(Cast<ACombatCharacter>(InPawn));
	}
}

void ACYYPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (UUIManagerComponent* UIManager = GetUIManagerComponent())
	{
		UIManager->SetAiming(false);
		UIManager->ClearEnemyTarget();
		UIManager->InitializeForCharacter(nullptr);
	}
}

void ACYYPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACYYPlayerController::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACYYPlayerController::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACYYPlayerController::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACYYPlayerController::StopJumping);
	EnhancedInputComponent->BindAction(SuspendAction, ETriggerEvent::Started, this, &ACYYPlayerController::Suspend);
	if (ToggleBackpackAction)
	{
		EnhancedInputComponent->BindAction(ToggleBackpackAction, ETriggerEvent::Started, this, &ACYYPlayerController::ToggleBackpack);
	}
	if (UseMedkitAction)
	{
		EnhancedInputComponent->BindAction(UseMedkitAction, ETriggerEvent::Started, this, &ACYYPlayerController::UseMedkit);
	}
	EnhancedInputComponent->BindAction(PickUpAction, ETriggerEvent::Started, this, &ACYYPlayerController::PickUp);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ACYYPlayerController::Aim);
	EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ACYYPlayerController::StopAim);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &ACYYPlayerController::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ACYYPlayerController::StopRun);
	EnhancedInputComponent->BindAction(GunShotAction, ETriggerEvent::Started, this, &ACYYPlayerController::GunShot);
	if (ReloadAction)
	{
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ACYYPlayerController::Reload);
	}
	if (SwitchWeapon1Action)
	{
		EnhancedInputComponent->BindAction(SwitchWeapon1Action, ETriggerEvent::Started, this, &ACYYPlayerController::SwitchWeapon1);
	}
	if (SwitchWeapon2Action)
	{
		EnhancedInputComponent->BindAction(SwitchWeapon2Action, ETriggerEvent::Started, this, &ACYYPlayerController::SwitchWeapon2);
	}
}

void ACYYPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	AxisVectorX = InputAxisVector.X;
	AxisVectorY = InputAxisVector.Y;

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlPawn = GetPawn<APawn>())
	{
		ControlPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ACYYPlayerController::Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookVector = InputActionValue.Get<FVector2D>();
	if (APawn* ControlPawn = GetPawn<APawn>())
	{
		ControlPawn->AddControllerYawInput(LookVector.X * BaseTurnRate * 0.02f);
		ControlPawn->AddControllerPitchInput(LookVector.Y * BaseLookUpRate * 0.02f);
	}
}

void ACYYPlayerController::Jump()
{
	if (ACharacter* ControlChar = Cast<ACharacter>(GetPawn()))
	{
		ControlChar->Jump();
	}
}

void ACYYPlayerController::StopJumping()
{
	if (ACharacter* ControlChar = Cast<ACharacter>(GetPawn()))
	{
		ControlChar->StopJumping();
	}
}

void ACYYPlayerController::Suspend()
{
	if (UUIManagerComponent* UIManager = GetUIManagerComponent())
	{
		UIManager->TogglePauseMenu();
	}
}

void ACYYPlayerController::ToggleBackpack()
{
	if (UUIManagerComponent* UIManager = GetUIManagerComponent())
	{
		UIManager->ToggleBackpack();
	}
}

void ACYYPlayerController::UseMedkit()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn());
	if (!PlayerChar)
	{
		return;
	}

	const EMedkitUseResult Result = PlayerChar->TryUseMedkit();
	if (UUIManagerComponent* UIManager = GetUIManagerComponent())
	{
		UIManager->ShowMedkitQuickFeedback(Result);
	}
}

void ACYYPlayerController::PickUp()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->PickUp();
	}
}

void ACYYPlayerController::Aim()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn());
	if (!PlayerChar)
	{
		return;
	}

	PlayerChar->Aim();

	if (UUIManagerComponent* UIManager = GetUIManagerComponent(); UIManager && PlayerChar->bIsAiming)
	{
		UIManager->SetAiming(true);
	}
}

void ACYYPlayerController::StopAim()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn());
	if (!PlayerChar)
	{
		return;
	}

	const bool bWasAiming = PlayerChar->bIsAiming;
	PlayerChar->StopAiming();

	if (UUIManagerComponent* UIManager = GetUIManagerComponent(); UIManager && bWasAiming)
	{
		UIManager->SetAiming(false);
	}
}

void ACYYPlayerController::Run()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->Run();
	}
}

void ACYYPlayerController::StopRun()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->StopRuning();
	}
}

void ACYYPlayerController::GunShot()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->GunShot();
	}
}

void ACYYPlayerController::Reload()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->Reload();
	}
}

void ACYYPlayerController::SwitchWeapon1()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->SwitchWeapon(0);
	}
}

void ACYYPlayerController::SwitchWeapon2()
{
	if (ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(GetPawn()))
	{
		PlayerChar->SwitchWeapon(1);
	}
}

