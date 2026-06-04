#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/Weapon.h"
#include "Types/UIFlowTypes.h"
#include "UIManagerComponent.generated.h"

class UHUDRootWidget;
class UHealthWidget;
class UAmmoWidget;
class UEnemyHealthWidget;
class UEndUserWidget;
class UHealthComponent;
class UCYYSaveGame;
class ACombatCharacter;
class APlayerController;
class UUserWidget;
class UCanvasPanel;
class UMinimapWidget;
class UInventoryComponent;
class UBackpackWidget;
class UToastBannerWidget;
struct FWeaponSlot;

UENUM(BlueprintType)
enum class EFlowAction : uint8
{
	Start UMETA(DisplayName="Start"),
	Retry UMETA(DisplayName="Retry"),
	ReturnMenu UMETA(DisplayName="Return Menu"),
	Quit UMETA(DisplayName="Quit"),
	Pause UMETA(DisplayName="Pause"),
	Resume UMETA(DisplayName="Resume"),
	ShowResult UMETA(DisplayName="Show Result")
};

UENUM(BlueprintType)
enum class EBackpackItemType : uint8
{
	Medkit UMETA(DisplayName="Medkit"),
	Weapon UMETA(DisplayName="Weapon")
};

USTRUCT(BlueprintType)
struct FBackpackItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	EBackpackItemType ItemType = EBackpackItemType::Medkit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	int32 StackCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	bool bCanUse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	FText UseButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	FText DisabledReason;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	E_Weapon WeaponType = E_Weapon::NoWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	FGuid WeaponInstanceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	int32 ClipAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	int32 TotalAmmo = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	int32 Durability = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	bool bIsEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	int32 WeaponSlotIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	bool bInBackpackStorage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Backpack")
	int32 BackpackIndex = -1;
};

struct FFlowActionPayload
{
	FString Source;
	bool bHasGameResultData = false;
	FGameResultData GameResultData;
};

UCLASS(ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class DEMO_CYY_API UUIManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIManagerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category="UI")
	void InitializeForController(APlayerController* InPlayerController);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void InitializeForCharacter(ACombatCharacter* InCombatCharacter);

	UFUNCTION(BlueprintCallable, Category="UI")
	void SwitchState(EFlowState NewState);

	UFUNCTION(BlueprintCallable, Category="UI")
	void TogglePauseMenu();
	UFUNCTION(BlueprintCallable, Category="UI")
	void ToggleBackpack();
	UFUNCTION(BlueprintCallable, Category="UI")
	void AutoCloseBackpack();

	UFUNCTION(BlueprintCallable, Category="UI")
	void SetAiming(bool bInAiming);
	UFUNCTION(BlueprintCallable, Category="UI")
	void ShowGameResult(bool bPlayerWon);
	UFUNCTION(BlueprintCallable, Category="UI|Backpack")
	void ShowMedkitQuickFeedback(EMedkitUseResult Result);
	UFUNCTION(BlueprintCallable, Category="UI")
	void ShowPickupBanner(const FString& Message);

	// Save functions (BlueprintCallable kept temporarily so editor can open;
	// remove after deleting blueprint nodes that call them directly)
	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	bool HasSaveFile() const;

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	void ContinueGame();

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	void LoadAndStartLevel(int32 LevelNumber);

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	int32 GetContinueLevelNumber() const;

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	bool IsLevelUnlocked(int32 LevelNumber) const;

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	bool IsLevelCompleted(int32 LevelNumber) const;

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	int32 GetMaxUnlockedLevel() const;

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	void SaveCurrentGame();

	UFUNCTION(BlueprintCallable, Category = "UI|Save")
	void PlayNextLevel();

	void ShowGameResult(const FGameResultData& InResultData);
	bool RequestFlowAction(EFlowAction Action, const FFlowActionPayload* Payload = nullptr);

	UFUNCTION(BlueprintCallable, Category="HUD")
	void SetAmmo(int32 InClipAmmo, int32 InTotalAmmo, float InReloadAlpha = 0.0f);
	UFUNCTION(BlueprintCallable, Category="HUD")
	void UpdateWeaponSlotsUI(const TArray<FWeaponSlot>& Slots, int32 ActiveSlot);

	void UpdateEnemyTarget(ACombatCharacter* InEnemy);
	void ClearEnemyTarget();


	void ShowHitNumber(int32 Damage, FVector WorldLocation, bool bHeadShot);

	UFUNCTION(BlueprintPure, Category="UI")
	EFlowState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category="HUD")
	UHUDRootWidget* GetHUDRoot() const { return HUDRootInstance; }

private:
	void EnsureHUDCreated();
	void EnsureStartMenuCreated();
	void EnsurePauseMenuCreated();
	void EnsureResultMenuCreated();
	void EnsureAimWidgetsCreated();
	void EnsureBackpackCreated();
	void EnsureBackpackHintCreated();
	void EnsureToastBannerCreated();
	void ApplyState(EFlowState NewState);
	void ApplyInputModeForState(EFlowState NewState);
	void UpdateAimWidgetsVisibility();
	void ShowWidget(UUserWidget* InWidget, int32 ZOrder = 0);
	void HideWidget(UUserWidget* InWidget);
	void UnbindHealth();
	void UnbindInventory();
	void BindInventory();
	void CacheHUDChildren();
	void RefreshBackpackUI();
	void BuildWeaponItemEntries(
		TArray<FBackpackItemEntry>& OutEntries,
		const TArray<FWeaponSlot>& EquippedSlots,
		const TArray<FWeaponSlot>& BackpackSlots,
		int32 ActiveSlot);
	void ShowMedkitUseFeedback(EMedkitUseResult UseResult);
	bool IsTransitionAllowed(EFlowState FromState, EFlowState ToState) const;
	bool TransitionToState(EFlowState TargetState, const FString& Source);
	FName LevelNumberToMapName(int32 LevelNumber) const;
	void LogRejectedTransition(EFlowState TargetState, const FString& Source) const;
	void HandleMapLoaded(UWorld* LoadedWorld);
	void OpenLevelWithActionLock(FName TargetLevel);
	static const TCHAR* FlowStateToString(EFlowState InState);
	static const TCHAR* FlowActionToString(EFlowAction InAction);

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* InHealthComponent, float NewHealth, float Delta, AActor* InstigatorActor);

	UFUNCTION()
	void OnCharacterDeath(AActor* DeadActor);

	UFUNCTION()
	void OnEnemyHealthChanged(UHealthComponent* InHealthComponent, float NewHealth, float Delta, AActor* InstigatorActor);

	UFUNCTION()
	void OnEnemyDeath(AActor* DeadActor);

	UFUNCTION()
	void OnInventoryChanged(int32 NewMedkitCount);

	UFUNCTION()
	void HandleStartRequested();

	UFUNCTION()
	void HandleStartContinueRequested();

	UFUNCTION()
	void HandlePauseResumeRequested();

	UFUNCTION()
	void HandlePauseSaveRequested();

	UFUNCTION()
	void HandlePauseQuitRequested();

	UFUNCTION()
	void HandlePauseReturnRequested();
	UFUNCTION()
	void HandleResultRetryRequested();
	UFUNCTION()
	void HandleResultReturnRequested();
	UFUNCTION()
	void HandleResultNextLevelRequested();

	UFUNCTION()
	void HandleUseSelectedItem();
	UFUNCTION()
	void HandleDiscardSelectedItem();
	UFUNCTION()
	void HandleCloseBackpack();
	UFUNCTION()
	void HandleItemClicked(int32 ItemIndex);
	UFUNCTION()
	void HandleItemDoubleClicked(int32 ItemIndex);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HUD", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UHUDRootWidget> HUDRootClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HUD", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UHealthWidget> HealthWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HUD", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UAmmoWidget> AmmoWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI|Minimap", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UMinimapWidget> MinimapWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HUD", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> HitNumberWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> StartMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> PauseMenuClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UEndUserWidget> ResultMenuClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> HipAimWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> SightAimWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Backpack", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UBackpackWidget> BackpackWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Backpack", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UUserWidget> BackpackHintClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Feedback", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UToastBannerWidget> ToastBannerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI|Backpack", meta=(AllowPrivateAccess="true"))
	UTexture2D* MedkitIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	EFlowState InitialState = EFlowState::InGame;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	FName StartGameLevelName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	FName MainMenuLevelName = NAME_None;

	UPROPERTY(Transient)
	UHUDRootWidget* HUDRootInstance = nullptr;

	UPROPERTY(Transient)
	UUserWidget* StartMenuInstance = nullptr;

	UPROPERTY(Transient)
	UUserWidget* PauseMenuInstance = nullptr;
	UPROPERTY(Transient)
	UEndUserWidget* ResultMenuInstance = nullptr;

	UPROPERTY(Transient)
	UUserWidget* HipAimWidgetInstance = nullptr;

	UPROPERTY(Transient)
	UUserWidget* SightAimWidgetInstance = nullptr;

	UPROPERTY(Transient)
	UBackpackWidget* BackpackWidgetInstance = nullptr;
	UPROPERTY(Transient)
	UUserWidget* BackpackHintInstance = nullptr;
	UPROPERTY(Transient)
	UToastBannerWidget* ToastBannerInstance = nullptr;

	UPROPERTY(Transient)
	UHealthWidget* HealthWidget = nullptr;

	UPROPERTY(Transient)
	UAmmoWidget* AmmoWidget = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMinimapWidget> MinimapWidget = nullptr;

	UPROPERTY(Transient)
	UEnemyHealthWidget* EnemyHealthWidget = nullptr;

	UPROPERTY(Transient)
	APlayerController* LocalPlayerController = nullptr;

	UPROPERTY(Transient)
	ACombatCharacter* CombatCharacter = nullptr;

	UPROPERTY(Transient)
	UInventoryComponent* BoundInventoryComponent = nullptr;

	UPROPERTY(Transient)
	UHealthComponent* BoundHealthComponent = nullptr;

	UPROPERTY(Transient)
	ACombatCharacter* CurrentEnemyTarget = nullptr;

	UPROPERTY(Transient)
	UHealthComponent* BoundEnemyHealthComponent = nullptr;

	UPROPERTY(Transient)
	bool bIsAiming = false;
	UPROPERTY(Transient)
	bool bBackpackVisible = false;

	UPROPERTY(Transient)
	bool bControllerInitialized = false;

	UPROPERTY(Transient)
	EFlowState CurrentState = EFlowState::InGame;

	// Save cache (mutable allows lazy-loading in const methods)
	UPROPERTY(Transient)
	mutable TObjectPtr<UCYYSaveGame> CachedSaveData = nullptr;

	UPROPERTY(Transient)
	bool bActionLocked = false;

	UPROPERTY(Transient)
	bool bResultShown = false;

	UPROPERTY(Transient)
	EFlowAction LastAction = EFlowAction::Start;

	UPROPERTY(Transient)
	double LastActionRealSeconds = -1000.0;

	UPROPERTY(Transient)
	double ActionDebounceWindowSeconds = 0.12;

	UPROPERTY(Transient)
	FGameResultData LastResultData;

	UPROPERTY(Transient)
	bool bHasLastResultData = false;
};
