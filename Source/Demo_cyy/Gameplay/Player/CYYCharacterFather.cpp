#include "Gameplay/Player/CYYCharacterFather.h"

#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/HealthComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/LevelProgressionComponent.h"
#include "Components/UIManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DataAssets/Weapon.h"
#include "Gameplay/Player/CYYPlayerController.h"
#include "AI/Enemy/EnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Interaction/Interact.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

namespace
{
int32 ResolveWeaponEquipSlot(const E_Weapon WeaponType, const int32 SlotCount)
{
	if (SlotCount <= 0)
	{
		return INDEX_NONE;
	}

	switch (WeaponType)
	{
	case E_Weapon::RU74:
		return 0;
	case E_Weapon::Beretta:
		return SlotCount > 1 ? 1 : 0;
	default:
		return 0;
	}
}
}

ACYYCharacterFather::ACYYCharacterFather()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		CharacterMesh->SetOwnerNoSee(true);
	}

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(GetCapsuleComponent());
	FollowCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 64.0f));
	FollowCamera->bUsePawnControlRotation = true;

	Arm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arm"));
	Arm->SetupAttachment(FollowCamera);
	Arm->SetRelativeLocation(FVector(109.0f, 7.0f, -102.0f));
	Arm->SetRelativeRotation(FRotator(64.0f, 0.0f, 0.0f));
	Arm->SetOnlyOwnerSee(true);
	Arm->SetCastShadow(false);
	Arm->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponSlots.SetNum(MaxWeaponSlots);
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	LevelComponent = CreateDefaultSubobject<ULevelProgressionComponent>(TEXT("LevelComponent"));
}

void ACYYCharacterFather::BeginPlay()
{
	Super::BeginPlay();

	if (Arm)
	{
		ArmDefaultRelativeLocation = Arm->GetRelativeLocation();
		ArmDefaultRelativeRotation = Arm->GetRelativeRotation();
	}

	if (UHealthComponent* HC = GetHealthComponent())
	{
		HC->OnHealthChanged.AddDynamic(this, &ACYYCharacterFather::HandleSelfHealthChanged);
		HC->OnDeath.AddDynamic(this, &ACYYCharacterFather::HandleSelfDeath);
	}

	if (LevelComponent)
	{
		LevelComponent->OnLevelUpClient.AddDynamic(this, &ACYYCharacterFather::OnLeveledUp);
	}

	UpdateAmmoUI(0.0f);
}

void ACYYCharacterFather::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Velocity{GetVelocity()};
	Velocity.Z = 0.0f;
	Speed = Velocity.Size();

	if (ACYYPlayerController* CYYPC = Cast<ACYYPlayerController>(GetController()))
	{
		AxisVectorX = CYYPC->AxisVectorX;
		AxisVectorY = CYYPC->AxisVectorY;
	}

	if (CurrentWeaponType == E_Weapon::RU74)
	{
		Arm->SetRelativeLocation(FVector(-5.0f, 1.0f, -169.0f));
		Arm->SetRelativeRotation(FRotator(0.0f, -90.0f, -1.0f));
	}
	else if (CurrentWeaponType == E_Weapon::Beretta)
	{
		Arm->SetRelativeLocation(FVector(-5.0f, 4.0f, -169.0f));
		Arm->SetRelativeRotation(FRotator(0.0f, -100.0f, -1.0f));
	}
	else
	{
		Arm->SetRelativeLocation(ArmDefaultRelativeLocation);
		Arm->SetRelativeRotation(ArmDefaultRelativeRotation);
	}

	LineTrace();
	UpdatePlayerState();
	UpdateMoveSpeed();

	if (bIsReloading)
	{
		const float Remaining = GetWorldTimerManager().GetTimerRemaining(ReloadTimerHandle);
		const float Alpha = ReloadDuration > 0.0f ? 1.0f - FMath::Clamp(Remaining / ReloadDuration, 0.0f, 1.0f) : 1.0f;
		UpdateAmmoUI(Alpha);
	}
}

void ACYYCharacterFather::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACYYCharacterFather::OnFireMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsFiring = false;
}

void ACYYCharacterFather::HandleSelfHealthChanged(UHealthComponent* InHealthComponent, float NewHealth, float Delta, AActor* InstigatorActor)
{
	UE_LOG(LogTemp, Verbose, TEXT("Player Health Changed: %.1f (Delta %.1f)"), NewHealth, Delta);

	if (Delta < 0.0f)
	{
		if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
		{
			if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
			{
				UIManager->AutoCloseBackpack();
			}
		}
	}
}

void ACYYCharacterFather::HandleSelfDeath(AActor* DeadActor)
{
	bIsAiming = false;
	bIsFiring = false;
	bIsReloading = false;
	FocusedEnemyTarget = nullptr;
	FocusedEnemyHitResult = FHitResult();
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	if (GetFollowCamera())
	{
		GetFollowCamera()->SetFieldOfView(90.0f);
	}

	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			UIManager->ClearEnemyTarget();
			UIManager->SetAmmo(CurrentClipAmmo, CurrentTotalAmmo, 0.0f);
		}
	}
}

void ACYYCharacterFather::TriggerFireByAnimNotify()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ShootParticles();
	}

	if (!CurrentWeaponData || CurrentClipAmmo <= 0)
	{
		UpdateAmmoUI(0.0f);
		return;
	}

	CurrentClipAmmo = FMath::Max(0, CurrentClipAmmo - 1);
	UpdateAmmoUI(0.0f);

	// 伤害结算只使用 Tick 中 LineTrace 缓存的目标，避免在动画通知里再做一套射线逻辑。
	if (!FocusedEnemyTarget || !IsValid(FocusedEnemyTarget))
	{
		return;
	}

	if (UHealthComponent* EnemyHealth = FocusedEnemyTarget->GetHealthComponent())
	{
		if (EnemyHealth->IsDead())
		{
			return;
		}
	}

	const FVector ShotDirection = FollowCamera ? FollowCamera->GetForwardVector() : GetActorForwardVector();
	const bool bHeadShot = FocusedEnemyHitResult.BoneName.ToString().Contains(TEXT("head"), ESearchCase::IgnoreCase);
	int32 FinalDamage = FMath::RoundToInt(CurrentWeaponData->Damage);
	if (bHeadShot)
	{
		FinalDamage *= 2;
	}
	UGameplayStatics::ApplyPointDamage(
		FocusedEnemyTarget,
		CurrentWeaponData->Damage,
		ShotDirection,
		FocusedEnemyHitResult,
		GetController(),
		this,
		UDamageType::StaticClass()
	);
	IDamageable::Execute_Hit(FocusedEnemyTarget, FocusedEnemyHitResult);
	
	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			UIManager->ShowHitNumber(FinalDamage, FocusedEnemyHitResult.ImpactPoint, bHeadShot);
		}
	}
}

void ACYYCharacterFather::GunShot()
{
	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			UIManager->AutoCloseBackpack();
		}
	}

	if (!CurrentWeaponData || CurrentWeaponType == E_Weapon::NoWeapon || !Arm || bIsReloading)
	{
		return;
	}
	if (CurrentClipAmmo <= 0)
	{
		return;
	}
	UAnimInstance* AnimIns = Arm->GetAnimInstance();
	if (!AnimIns)
	{
		return;
	}
	// 开镜 → ADS开镜射击动画 | 不开镜 → 腰射射击动画
	UAnimMontage* ShotMontage = (CurrentPlayerState == E_PlayerState::Aim)
		? CurrentWeaponData->ADSFireMontage
		: CurrentWeaponData->FireMontage;
	if (!ShotMontage)
	{
		return;
	}
	// 避免快速点击时反复重启同一开火蒙太奇，减少顿挫感。
	if (AnimIns->Montage_IsPlaying(ShotMontage))
	{
		return;
	}

	bIsFiring = true;

	// 开火音效做最小间隔节流，避免快速射击时音频叠播引起卡顿。
	if (FireSound && GetWorld())
	{
		const float Now = GetWorld()->GetTimeSeconds();
		if (Now - LastFireSoundTime >= FireSoundMinInterval)
		{
			UGameplayStatics::PlaySound2D(this, FireSound);
			LastFireSoundTime = Now;
		}
	}

	AnimIns->Montage_Play(ShotMontage);
	FOnMontageEnded OnMontageEnded;
	OnMontageEnded.BindUObject(this, &ACYYCharacterFather::OnFireMontageEnded);
	AnimIns->Montage_SetEndDelegate(OnMontageEnded, ShotMontage);
}

void ACYYCharacterFather::Reload()
{
	if (!CurrentWeaponData || bIsReloading)
	{
		return;
	}

	const int32 MagazineSize = FMath::Max(1, CurrentWeaponData->MagazineSize);
	if (CurrentClipAmmo >= MagazineSize || CurrentTotalAmmo <= 0)
	{
		return;
	}

	bIsReloading = true;
	UpdateAmmoUI(0.01f);

	if (CurrentWeaponData->ReloadMontage && Arm)
	{
		if (UAnimInstance* AnimIns = Arm->GetAnimInstance())
		{
			AnimIns->Montage_Play(CurrentWeaponData->ReloadMontage);
			FOnMontageEnded EndDelegate;
			EndDelegate.BindLambda([this](UAnimMontage*, bool) { FinishReload(); });
			AnimIns->Montage_SetEndDelegate(EndDelegate, CurrentWeaponData->ReloadMontage);
			return;
		}
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ACYYCharacterFather::FinishReload, ReloadDuration, false);
	}
}

void ACYYCharacterFather::FinishReload()
{
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	if (!CurrentWeaponData)
	{
		bIsReloading = false;
		UpdateAmmoUI(0.0f);
		return;
	}

	const int32 MagazineSize = FMath::Max(1, CurrentWeaponData->MagazineSize);
	const int32 Needed = MagazineSize - CurrentClipAmmo;
	const int32 LoadAmount = FMath::Min(Needed, CurrentTotalAmmo);
	CurrentClipAmmo += LoadAmount;
	CurrentTotalAmmo -= LoadAmount;
	bIsReloading = false;
	UpdateAmmoUI(0.0f);
}

void ACYYCharacterFather::Aim()
{
	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			UIManager->AutoCloseBackpack();
		}
	}

	if (!CurrentWeaponData || CurrentWeaponType == E_Weapon::NoWeapon)
	{
		return;
	}

	bIsAiming = true;
	if (GetFollowCamera())
	{
		GetFollowCamera()->SetFieldOfView(60.0f);
	}
}

void ACYYCharacterFather::StopAiming()
{
	if (!bIsAiming)
	{
		return;
	}

	bIsAiming = false;
	if (GetFollowCamera())
	{
		GetFollowCamera()->SetFieldOfView(90.0f);
	}
}

void ACYYCharacterFather::Run()
{
	bIsRunning = true;
}

void ACYYCharacterFather::StopRuning()
{
	bIsRunning = false;
}

void ACYYCharacterFather::SetEquippedItem(AItem* NewItem)
{
	EquippedWeapon = NewItem;
}

void ACYYCharacterFather::PickUp()
{
	if (!FocusedInteractActor)
	{
		return;
	}

	AItem* TargetWeapon = Cast<AItem>(FocusedInteractActor);
	if (!TargetWeapon)
	{
		// 非武器交互（电闸、钥匙、安全门等）：调 OnInteract
		if (FocusedInteractActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
		{
			IInteract::Execute_OnInteract(FocusedInteractActor, this);
		}
		return;
	}

	bIsReloading = false;
	GetWorldTimerManager().ClearTimer(ReloadTimerHandle);

	TargetWeapon->SetOwner(this);
	TargetWeapon->EquitmentWeapon();
	TargetWeapon->SetActorHiddenInGame(false);

	UWeapon* NewWeaponData = TargetWeapon->DataAsset_Implementation();
	const E_Weapon WeaponType = NewWeaponData ? NewWeaponData->CurrentWeapon : E_Weapon::NoWeapon;

	if (WeaponSlots.Num() <= 0)
	{
		return;
	}

	const int32 NewClipAmmo = NewWeaponData
		? (NewWeaponData->CurrentAmmo > 0
			? FMath::Min(NewWeaponData->CurrentAmmo, NewWeaponData->MagazineSize)
			: NewWeaponData->MagazineSize)
		: 0;
	const int32 NewTotalAmmo = NewWeaponData ? NewWeaponData->MaxAmmo : 0;
	FWeaponSlot NewSlotEntry;
	NewSlotEntry.WeaponActor = TargetWeapon;
	NewSlotEntry.ClipAmmo = NewClipAmmo;
	NewSlotEntry.TotalAmmo = NewTotalAmmo;

	bool bEquippedDirectly = false;
	const int32 PreferredSlot = ResolveWeaponEquipSlot(WeaponType, WeaponSlots.Num());
	if (WeaponSlots.IsValidIndex(PreferredSlot) && !WeaponSlots[PreferredSlot].WeaponActor)
	{
		WeaponSlots[PreferredSlot] = NewSlotEntry;
		bEquippedDirectly = true;
		SwitchWeapon(PreferredSlot);
	}
	else
	{
		BackpackWeapons.Add(NewSlotEntry);
		TargetWeapon->SetActorHiddenInGame(true);
		UpdateAmmoUI(0.0f);
	}

	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			FString WeaponName = TEXT("武器");
			if (NewWeaponData)
			{
				if (const UEnum* WeaponEnum = StaticEnum<E_Weapon>())
				{
					WeaponName = WeaponEnum->GetDisplayNameTextByValue(static_cast<int64>(NewWeaponData->CurrentWeapon)).ToString();
				}
			}
			if (bEquippedDirectly)
			{
				UIManager->ShowPickupBanner(FString::Printf(TEXT("%s已装备"), *WeaponName));
			}
			else
			{
				UIManager->ShowPickupBanner(FString::Printf(TEXT("%s已存入背包"), *WeaponName));
			}
		}
	}

	if (!NewWeaponData)
	{
		UE_LOG(LogTemp, Warning, TEXT("PickUp: WeaponData is null. Actor=%s"), *GetNameSafe(TargetWeapon));
	}

	IInteract::Execute_HideInteractWidget(FocusedInteractActor);
	FocusedInteractActor = nullptr;
}

bool ACYYCharacterFather::EquipFromBackpack(int32 BackpackIndex)
{
	if (!BackpackWeapons.IsValidIndex(BackpackIndex) || WeaponSlots.Num() <= 0)
	{
		return false;
	}

	FWeaponSlot BackpackSlot = BackpackWeapons[BackpackIndex];
	if (!BackpackSlot.WeaponActor)
	{
		BackpackWeapons.RemoveAt(BackpackIndex);
		return false;
	}

	UWeapon* BackpackWeaponData = BackpackSlot.WeaponActor->DataAsset_Implementation();
	const E_Weapon BackpackWeaponType = BackpackWeaponData ? BackpackWeaponData->CurrentWeapon : E_Weapon::NoWeapon;
	const int32 TargetSlotIndex = ResolveWeaponEquipSlot(BackpackWeaponType, WeaponSlots.Num());
	if (!WeaponSlots.IsValidIndex(TargetSlotIndex))
	{
		return false;
	}

	FWeaponSlot PreviousSlot = WeaponSlots[TargetSlotIndex];
	if (PreviousSlot.WeaponActor && ActiveWeaponSlot == TargetSlotIndex)
	{
		PreviousSlot.ClipAmmo = CurrentClipAmmo;
		PreviousSlot.TotalAmmo = CurrentTotalAmmo;
	}

	BackpackWeapons.RemoveAt(BackpackIndex);
	if (PreviousSlot.WeaponActor)
	{
		PreviousSlot.WeaponActor->SetActorHiddenInGame(true);
		BackpackWeapons.Add(PreviousSlot);
	}

	BackpackSlot.WeaponActor->SetOwner(this);
	BackpackSlot.WeaponActor->EquitmentWeapon();
	BackpackSlot.WeaponActor->SetActorHiddenInGame(false);
	WeaponSlots[TargetSlotIndex] = BackpackSlot;

	if (ActiveWeaponSlot == TargetSlotIndex)
	{
		ActiveWeaponSlot = INDEX_NONE;
	}
	SwitchWeapon(TargetSlotIndex);
	return true;
}

void ACYYCharacterFather::LineTrace()
{
	if (!FollowCamera || !IsLocallyControlled())
	{
		return;
	}

	// 交互检测：只负责拾取/交互提示。
	FHitResult PickUpHit;
	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector End = Start + FollowCamera->GetForwardVector() * InteractTraceDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(PickUpHit, Start, End, ECollisionChannel::ECC_GameTraceChannel1, QueryParams);

	AActor* NewFocusedActor = nullptr;
	if (PickUpHit.bBlockingHit)
	{
		AActor* HitActor = PickUpHit.GetActor();
		if (HitActor && HitActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
		{
			NewFocusedActor = HitActor;
		}
	}

	if (FocusedInteractActor != NewFocusedActor)
	{
		if (FocusedInteractActor && FocusedInteractActor->GetClass()->ImplementsInterface(UInteract::StaticClass()))
		{
			IInteract::Execute_HideInteractWidget(FocusedInteractActor);
		}

		FocusedInteractActor = NewFocusedActor;
		if (FocusedInteractActor)
		{
			IInteract::Execute_ShowInteractWidget(FocusedInteractActor);
		}
	}

	// 敌人检测：统一在角色内做可见性射线，供顶部敌人血条与开火命中复用。
	FHitResult EnemyHit;
	const FVector EnemyTraceEnd = Start + FollowCamera->GetForwardVector() * 50000.0f;

	FCollisionQueryParams EnemyQueryParams;
	EnemyQueryParams.AddIgnoredActor(this);
	if (EquippedWeapon)
	{
		EnemyQueryParams.AddIgnoredActor(EquippedWeapon);
	}

	ACombatCharacter* NewEnemyTarget = nullptr;
	if (GetWorld()->LineTraceSingleByChannel(EnemyHit, Start, EnemyTraceEnd, ECollisionChannel::ECC_Visibility, EnemyQueryParams))
	{
		NewEnemyTarget = Cast<AEnemyBase>(EnemyHit.GetActor());
	}

	if (FocusedEnemyTarget != NewEnemyTarget)
	{
		FocusedEnemyTarget = NewEnemyTarget;
		FocusedEnemyHitResult = EnemyHit;

		if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
		{
			if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
			{
				if (FocusedEnemyTarget)
				{
					UIManager->UpdateEnemyTarget(FocusedEnemyTarget);
				}
				else
				{
					UIManager->ClearEnemyTarget();
				}
			}
		}
	}
	else if (FocusedEnemyTarget)
	{
		// 持续瞄准同一敌人时也要刷新命中信息，避免命中点数据陈旧。
		FocusedEnemyHitResult = EnemyHit;
	}
}

void ACYYCharacterFather::UpdatePlayerState()
{
	if (bIsAiming)
	{
		CurrentPlayerState = E_PlayerState::Aim;
	}
	else
	{
		if (AxisVectorY > 0)
		{
			if (bIsRunning)
			{
				CurrentPlayerState = E_PlayerState::Run;
			}
			else
			{
				CurrentPlayerState = E_PlayerState::Walk;
			}
		}
		else
		{
			if (Speed > 0)
			{
				CurrentPlayerState = E_PlayerState::Walk;
			}
			else
			{
				CurrentPlayerState = E_PlayerState::Idle;
			}
		}
	}
}

void ACYYCharacterFather::UpdateMoveSpeed()
{
	float TargetMaxSpeed = 0.0f;

	switch (CurrentPlayerState)
	{
	case E_PlayerState::Idle:
		TargetMaxSpeed = 400.0f;
		break;

	case E_PlayerState::Walk:
		TargetMaxSpeed = 400.0f;
		break;

	case E_PlayerState::Run:
		TargetMaxSpeed = 600.0f;
		break;

	case E_PlayerState::Aim:
		TargetMaxSpeed = 265.0f;
		break;

	default:
		TargetMaxSpeed = 400.0f;
		break;
	}

	GetCharacterMovement()->MaxWalkSpeed = TargetMaxSpeed;
}

void ACYYCharacterFather::UpdateAmmoUI(float ReloadAlpha)
{
	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			const float DisplayAlpha = bIsReloading ? FMath::Clamp(ReloadAlpha, 0.01f, 1.0f) : 0.0f;
			UIManager->SetAmmo(CurrentClipAmmo, CurrentTotalAmmo, DisplayAlpha);
			UIManager->UpdateWeaponSlotsUI(WeaponSlots, ActiveWeaponSlot);
		}
	}
}

EMedkitUseResult ACYYCharacterFather::TryUseMedkit()
{
	UHealthComponent* Health = GetHealthComponent();
	if (!InventoryComponent || !Health)
	{
		return EMedkitUseResult::NoItem;
	}

	EFlowState FlowState = EFlowState::InGame;
	if (const ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (const UUIManagerComponent* UIManager = PC->GetUIManagerComponent())
		{
			FlowState = UIManager->GetCurrentState();
		}
	}

	const bool bIsFullHealth = Health->GetCurrentHealth() >= Health->GetMaxHealth() - KINDA_SMALL_NUMBER;
	const EMedkitUseResult Result = InventoryComponent->TryUseMedkit(FlowState, bIsFullHealth);
	if (Result == EMedkitUseResult::Success)
	{
		Health->Heal(50.0f, this);
	}
	return Result;
}

void ACYYCharacterFather::ClearMedkitInventory()
{
	if (InventoryComponent)
	{
		InventoryComponent->ClearInventory();
	}
}

void ACYYCharacterFather::SwitchWeapon(int32 SlotIndex)
{
	if (WeaponSlots.Num() <= 0)
	{
		return;
	}

	const int32 ClampedSlotIndex = FMath::Clamp(SlotIndex, 0, WeaponSlots.Num() - 1);
	if (ClampedSlotIndex == ActiveWeaponSlot && EquippedWeapon == WeaponSlots[ClampedSlotIndex].WeaponActor)
	{
		return;
	}

	if (ActiveWeaponSlot >= 0 && ActiveWeaponSlot < WeaponSlots.Num())
	{
		WeaponSlots[ActiveWeaponSlot].ClipAmmo = CurrentClipAmmo;
		WeaponSlots[ActiveWeaponSlot].TotalAmmo = CurrentTotalAmmo;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->SetActorHiddenInGame(true);
	}

	ActiveWeaponSlot = ClampedSlotIndex;
	const FWeaponSlot& NewSlot = WeaponSlots[ActiveWeaponSlot];
	SetEquippedItem(NewSlot.WeaponActor);

	if (!EquippedWeapon)
	{
		CurrentWeaponData = nullptr;
		CurrentWeaponType = E_Weapon::NoWeapon;
		CurrentClipAmmo = 0;
		CurrentTotalAmmo = 0;
		UpdateAmmoUI(0.0f);
		return;
	}

	EquippedWeapon->SetActorHiddenInGame(false);
	CurrentWeaponData = EquippedWeapon->DataAsset_Implementation();
	CurrentWeaponType = CurrentWeaponData ? CurrentWeaponData->CurrentWeapon : E_Weapon::NoWeapon;
	CurrentClipAmmo = NewSlot.ClipAmmo;
	CurrentTotalAmmo = NewSlot.TotalAmmo;
	UpdateAmmoUI(0.0f);
}

void ACYYCharacterFather::OnLeveledUp(int32 NewLevel, int32 OldLevel)
{
	// 升级只扩上限，不补血（保持原血量比例）
	if (UHealthComponent* HC = GetHealthComponent())
	{
		HC->SetMaxHealthMultiplier(LevelComponent ? LevelComponent->GetHPMultiplier() : 1.0f);
	}

	// Toast
	if (ACYYPlayerController* PC = Cast<ACYYPlayerController>(GetController()))
	{
		if (UUIManagerComponent* UIMgr = PC->GetUIManagerComponent())
		{
			UIMgr->ShowPickupBanner(
				FString::Printf(TEXT("LEVEL UP! Lv%d → Lv%d"), OldLevel, NewLevel));
		}
	}
}

void ACYYCharacterFather::RestoreWeaponsFromSave(const TArray<E_Weapon>& EquippedTypes, const TArray<E_Weapon>& BackpackTypes)
{
	UWorld* World = GetWorld();
	if (!World) return;

	auto SpawnAndSetupWeapon = [&](E_Weapon WeaponType) -> AItem*
	{
		if (WeaponType == E_Weapon::NoWeapon) return nullptr;

		TSubclassOf<AItem>* Found = WeaponClassMap.Find(WeaponType);
		if (!Found || !*Found) return nullptr;

		FActorSpawnParameters Params;
		Params.Owner = this;
		AItem* Weapon = World->SpawnActor<AItem>(*Found, GetActorLocation(), FRotator::ZeroRotator, Params);
		if (!Weapon) return nullptr;

		Weapon->SetOwner(this);
		Weapon->EquitmentWeapon();
		return Weapon;
	};

	// 生成装备武器（最多 2 把）
	for (const E_Weapon WeaponType : EquippedTypes)
	{
		AItem* Weapon = SpawnAndSetupWeapon(WeaponType);
		if (!Weapon) continue;

		Weapon->SetActorHiddenInGame(false);
		UWeapon* WeaponData = Weapon->DataAsset_Implementation();
		FWeaponSlot Slot;
		Slot.WeaponActor = Weapon;
		Slot.ClipAmmo = WeaponData ? WeaponData->MagazineSize : 0;
		Slot.TotalAmmo = WeaponData ? WeaponData->MaxAmmo : 0;

		const E_Weapon Type = WeaponData ? WeaponData->CurrentWeapon : E_Weapon::NoWeapon;
		int32 TargetSlot = INDEX_NONE;
		if (Type == E_Weapon::RU74 && WeaponSlots.IsValidIndex(0) && !WeaponSlots[0].WeaponActor)
			TargetSlot = 0;
		else if (Type == E_Weapon::Beretta && WeaponSlots.IsValidIndex(1) && !WeaponSlots[1].WeaponActor)
			TargetSlot = 1;
		else if (WeaponSlots.IsValidIndex(0) && !WeaponSlots[0].WeaponActor)
			TargetSlot = 0;

		if (TargetSlot != INDEX_NONE)
			WeaponSlots[TargetSlot] = Slot;
		else
			BackpackWeapons.Add(Slot);
	}

	// 生成背包武器
	for (const E_Weapon WeaponType : BackpackTypes)
	{
		AItem* Weapon = SpawnAndSetupWeapon(WeaponType);
		if (!Weapon) continue;

		Weapon->SetActorHiddenInGame(true);
		UWeapon* WeaponData = Weapon->DataAsset_Implementation();
		FWeaponSlot Slot;
		Slot.WeaponActor = Weapon;
		Slot.ClipAmmo = WeaponData ? WeaponData->MagazineSize : 0;
		Slot.TotalAmmo = WeaponData ? WeaponData->MaxAmmo : 0;
		BackpackWeapons.Add(Slot);
	}

	// 切到第一个有武器的槽
	for (int32 i = 0; i < WeaponSlots.Num(); ++i)
	{
		if (WeaponSlots[i].WeaponActor)
		{
			SwitchWeapon(i);
			break;
		}
	}
}
