#pragma once

#include "CoreMinimal.h"
#include "Combat/CombatCharacter.h"
#include "Types/CYYPlayerState.h"
#include "CYYCharacterFather.generated.h"

class AItem;
class UWeapon;
class UAnimInstance;
class UHealthComponent;
class UInventoryComponent;
class ULevelProgressionComponent;
enum class EMedkitUseResult : uint8;

USTRUCT(BlueprintType)
struct FWeaponSlot
{
	GENERATED_BODY()

	UPROPERTY()
	AItem* WeaponActor = nullptr;

	UPROPERTY()
	int32 ClipAmmo = 0;

	UPROPERTY()
	int32 TotalAmmo = 0;
};

UCLASS()
class DEMO_CYY_API ACYYCharacterFather : public ACombatCharacter
{
	GENERATED_BODY()

public:
	ACYYCharacterFather();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UFUNCTION()
	void HandleSelfHealthChanged(UHealthComponent* InHealthComponent, float NewHealth, float Delta, AActor* InstigatorActor);
	UFUNCTION()
	void HandleSelfDeath(AActor* DeadActor);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	AActor* FocusedInteractActor = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="True"))
	ACombatCharacter* FocusedEnemyTarget = nullptr;
	FHitResult FocusedEnemyHitResult;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	AItem* EquippedWeapon = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	UWeapon* CurrentWeaponData = nullptr;
	static constexpr int32 MaxWeaponSlots = 2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	TArray<FWeaponSlot> WeaponSlots;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	TArray<FWeaponSlot> BackpackWeapons;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	int32 ActiveWeaponSlot = -1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	FVector ArmDefaultRelativeLocation = FVector::ZeroVector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	FRotator ArmDefaultRelativeRotation = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Weapon, meta=(AllowPrivateAccess="True"))
	float InteractTraceDistance = 250.0f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera, meta=(AllowPrivateAccess="True"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	class USkeletalMeshComponent* Arm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	float Speed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess="true"))
	class USoundCue* FireSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	float FireSoundMinInterval = 0.06f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess="true"))
	UAnimInstance* AnimInstance = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	bool bIsAiming = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Aim")
	bool bIsFiring = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=PlayerState, meta=(AllowPrivateAccess="True"))
	E_PlayerState CurrentPlayerState = E_PlayerState::Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	E_Weapon CurrentWeaponType = E_Weapon::NoWeapon;

public:
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE AItem* GetEquippedItem() const { return EquippedWeapon; }
	void SetEquippedItem(AItem* NewItem);
	FORCEINLINE UWeapon* GetCurrentWeaponData() const { return CurrentWeaponData; }

	void GunShot();
	UFUNCTION(BlueprintCallable)
	void Reload();

	UFUNCTION()
	void OnFireMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void TriggerFireByAnimNotify();
	UFUNCTION()
	void FinishReload();

	void Aim();
	void StopAiming();
	void Run();
	void StopRuning();
	void PickUp();
	void SwitchWeapon(int32 SlotIndex);
	bool EquipFromBackpack(int32 BackpackIndex);
	void LineTrace();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Run")
	bool bIsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	float AxisVectorX = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="True"))
	float AxisVectorY = 0.0f;
	float LastFireSoundTime = -1000.0f;

	void UpdatePlayerState();
	void UpdateMoveSpeed();
	void UpdateAmmoUI(float ReloadAlpha = 0.0f);
	EMedkitUseResult TryUseMedkit();
	void ClearMedkitInventory();

	// 从存档恢复武器
	void RestoreWeaponsFromSave(const TArray<E_Weapon>& EquippedTypes, const TArray<E_Weapon>& BackpackTypes);

	UFUNCTION()
	void OnLeveledUp(int32 NewLevel, int32 OldLevel);

private:
	// 武器类型 → 蓝图类映射（在 BP_CharacterFather 里配）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TMap<E_Weapon, TSubclassOf<AItem>> WeaponClassMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	int32 CurrentClipAmmo = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	int32 CurrentTotalAmmo = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True"))
	bool bIsReloading = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Weapon, meta=(AllowPrivateAccess="True", ClampMin="0.1"))
	float ReloadDuration = 1.2f;
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="True"))
	UInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Progression", meta=(AllowPrivateAccess="True"))
	ULevelProgressionComponent* LevelComponent = nullptr;

public:
	UFUNCTION(BlueprintPure, Category="Inventory")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	UFUNCTION(BlueprintPure, Category="Progression")
	ULevelProgressionComponent* GetLevelComponent() const { return LevelComponent; }

	const TArray<FWeaponSlot>& GetWeaponSlots() const { return WeaponSlots; }
	TArray<FWeaponSlot>& GetWeaponSlotsMutable() { return WeaponSlots; }
	const TArray<FWeaponSlot>& GetBackpackWeapons() const { return BackpackWeapons; }
	TArray<FWeaponSlot>& GetBackpackWeaponsMutable() { return BackpackWeapons; }
	int32 GetActiveWeaponSlot() const { return ActiveWeaponSlot; }
};

