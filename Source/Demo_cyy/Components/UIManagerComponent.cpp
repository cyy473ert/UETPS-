#include "Components/UIManagerComponent.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Combat/CombatCharacter.h"
#include "Components/HealthComponent.h"
#include "Components/Button.h"
#include "Components/InventoryComponent.h"
#include "GameFramework/PlayerController.h"
#include "DataAssets/Weapon.h"
#include "Gameplay/Player/CYYCharacterFather.h"
#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/AmmoWidget.h"
#include "UI/BackpackWidget.h"
#include "UI/BeginUserWidget.h"
#include "UI/EnemyHealthWidget.h"
#include "UI/EndUserWidget.h"
#include "UI/HUDRootWidget.h"
#include "UI/HealthWidget.h"
#include "UI/MinimapWidget.h"
#include "UI/Set_UserWidget.h"
#include "UI/SettingsUserWidget.h"
#include "UI/ToastBannerWidget.h"
#include "Misc/DateTime.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectGlobals.h"
#include "Save/CYYSaveManager.h"
#include "Save/CYYSaveGame.h"

UUIManagerComponent::UUIManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	StartGameLevelName = TEXT("/Game/11/Level1_Shelter");
	MainMenuLevelName = TEXT("/Game/CYY/Maps/UMGStartMap");

	static ConstructorHelpers::FClassFinder<UUserWidget> StartMenuClassFinder(TEXT("/Game/CYY/UI/W_Begin"));
	if (StartMenuClassFinder.Succeeded())
	{
		StartMenuClass = StartMenuClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuClassFinder(TEXT("/Game/CYY/UI/W_Set"));
	if (PauseMenuClassFinder.Succeeded())
	{
		PauseMenuClass = PauseMenuClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<USettingsUserWidget> SettingsMenuClassFinder(TEXT("/Game/CYY/UI/W_Settings"));
	if (SettingsMenuClassFinder.Succeeded())
	{
		SettingsMenuClass = SettingsMenuClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UEndUserWidget> ResultMenuClassFinder(TEXT("/Game/CYY/UI/W_End"));
	if (ResultMenuClassFinder.Succeeded())
	{
		ResultMenuClass = ResultMenuClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> HipAimClassFinder(TEXT("/Game/CYY/UI/WBP_Aim"));
	if (HipAimClassFinder.Succeeded())
	{
		HipAimWidgetClass = HipAimClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> SightAimClassFinder(TEXT("/Game/CYY/UI/WBP_ISight_Aim"));
	if (SightAimClassFinder.Succeeded())
	{
		SightAimWidgetClass = SightAimClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UHealthWidget> HealthWidgetClassFinder(TEXT("/Game/CYY/UI/Character/WBP_HealthWidget"));
	if (HealthWidgetClassFinder.Succeeded())
	{
		HealthWidgetClass = HealthWidgetClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UBackpackWidget> BackpackClassFinder(TEXT("/Game/CYY/UI/WBP_Backpack"));
	if (BackpackClassFinder.Succeeded())
	{
		BackpackWidgetClass = BackpackClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> BackpackHintClassFinder(TEXT("/Game/CYY/UI/WBP_BackpackHint"));
	if (BackpackHintClassFinder.Succeeded())
	{
		BackpackHintClass = BackpackHintClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UToastBannerWidget> ToastBannerClassFinder(TEXT("/Game/CYY/UI/WBP_ToastBanner"));
	if (ToastBannerClassFinder.Succeeded())
	{
		ToastBannerClass = ToastBannerClassFinder.Class;
	}
}

void UUIManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UUIManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	UnbindHealth();
	UnbindInventory();
	ClearEnemyTarget();

	HideWidget(SightAimWidgetInstance);
	HideWidget(HipAimWidgetInstance);
	HideWidget(BackpackWidgetInstance);
	HideWidget(BackpackHintInstance);
	HideWidget(ToastBannerInstance);
	HideWidget(PauseMenuInstance);
	HideWidget(SettingsMenuInstance);
	HideWidget(ResultMenuInstance);
	HideWidget(StartMenuInstance);
	if (!HUDRootInstance)
	{
		HideWidget(AmmoWidget);
		HideWidget(HealthWidget);
	}
	HideWidget(MinimapWidget);
	HideWidget(HUDRootInstance);

	Super::EndPlay(EndPlayReason);
}

void UUIManagerComponent::InitializeForController(APlayerController* InPlayerController)
{
	if (!InPlayerController || !InPlayerController->IsLocalController())
	{
		return;
	}

	LocalPlayerController = InPlayerController;
	bControllerInitialized = true;

	EnsureHUDCreated();
	EnsureAimWidgetsCreated();
	EnsureBackpackCreated();
	EnsureBackpackHintCreated();
	EnsureToastBannerCreated();

	EFlowState ResolvedInitialState = InitialState;
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	if (ResolvedInitialState == EFlowState::InGame
		&& !MainMenuLevelName.IsNone()
		&& CurrentLevelName.Equals(FPackageName::GetShortName(MainMenuLevelName.ToString()), ESearchCase::IgnoreCase))
	{
		ResolvedInitialState = EFlowState::MainMenu;
	}

	SwitchState(ResolvedInitialState);
	InitializeForCharacter(Cast<ACombatCharacter>(LocalPlayerController->GetPawn()));
}

void UUIManagerComponent::InitializeForCharacter(ACombatCharacter* InCombatCharacter)
{
	CombatCharacter = InCombatCharacter;
	UnbindHealth();
	UnbindInventory();

	if (!bControllerInitialized)
	{
		return;
	}

	EnsureHUDCreated();
	BoundHealthComponent = CombatCharacter ? CombatCharacter->GetHealthComponent() : nullptr;
	if (!BoundHealthComponent)
	{
		if (HealthWidget)
		{
			HealthWidget->SetHealthPercent(0.0f);
		}
		return;
	}

	BoundHealthComponent->OnHealthChanged.AddUniqueDynamic(this, &UUIManagerComponent::OnHealthChanged);
	BoundHealthComponent->OnDeath.AddUniqueDynamic(this, &UUIManagerComponent::OnCharacterDeath);
	OnHealthChanged(BoundHealthComponent, BoundHealthComponent->GetCurrentHealth(), 0.0f, nullptr);
	BindInventory();
}

void UUIManagerComponent::SwitchState(EFlowState NewState)
{
	if (!bControllerInitialized || !LocalPlayerController)
	{
		return;
	}

	CurrentState = NewState;
	ApplyState(NewState);
}

void UUIManagerComponent::TogglePauseMenu()
{
	if (CurrentState == EFlowState::Paused)
	{
		RequestFlowAction(EFlowAction::Resume);
		return;
	}

	RequestFlowAction(EFlowAction::Pause);
}

void UUIManagerComponent::ToggleBackpack()
{
	if (CurrentState != EFlowState::InGame && CurrentState != EFlowState::Paused)
	{
		return;
	}

	EnsureBackpackCreated();
	if (!BackpackWidgetInstance)
	{
		return;
	}

	bBackpackVisible = !bBackpackVisible;
	if (bBackpackVisible)
	{
		RefreshBackpackUI();
		ShowWidget(BackpackWidgetInstance, 80);
		HideWidget(BackpackHintInstance);
	}
	else
	{
		HideWidget(BackpackWidgetInstance);
		if (CurrentState == EFlowState::InGame)
		{
			ShowWidget(BackpackHintInstance, 100);
		}
	}

	ApplyInputModeForState(CurrentState);
}

void UUIManagerComponent::AutoCloseBackpack()
{
	if (!bBackpackVisible)
	{
		return;
	}

	bBackpackVisible = false;
	HideWidget(BackpackWidgetInstance);
	if (CurrentState == EFlowState::InGame)
	{
		ShowWidget(BackpackHintInstance, 100);
	}
	ApplyInputModeForState(CurrentState);
}

void UUIManagerComponent::SetAiming(bool bInAiming)
{
	bIsAiming = bInAiming;
	UpdateAimWidgetsVisibility();
}

void UUIManagerComponent::ShowGameResult(bool bPlayerWon)
{
	FGameResultData ResultData;
	ResultData.ResultType = bPlayerWon ? EGameResultType::Win : EGameResultType::Lose;
	ResultData.EndReason = bPlayerWon ? EGameEndReason::AllEnemiesCleared : EGameEndReason::PlayerDead;
	ResultData.Timestamp = FDateTime::UtcNow();
	ShowGameResult(ResultData);
}

void UUIManagerComponent::ShowMedkitQuickFeedback(EMedkitUseResult Result)
{
	ShowMedkitUseFeedback(Result);
}

void UUIManagerComponent::ShowPickupBanner(const FString& Message)
{
	EnsureToastBannerCreated();
	if (ToastBannerInstance)
	{
		ToastBannerInstance->ShowMessage(Message, 2.0f);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[Pickup] %s"), *Message);
	}
}

void UUIManagerComponent::ShowGameResult(const FGameResultData& InResultData)
{
	FFlowActionPayload Payload;
	Payload.Source = TEXT("GameMode");
	Payload.bHasGameResultData = true;
	Payload.GameResultData = InResultData;
	RequestFlowAction(EFlowAction::ShowResult, &Payload);
}

bool UUIManagerComponent::RequestFlowAction(EFlowAction Action, const FFlowActionPayload* Payload)
{
	if (!bControllerInitialized || !LocalPlayerController)
	{
		return false;
	}

	const FString Source = Payload ? Payload->Source : FString(TEXT("Unknown"));
	const double Now = FPlatformTime::Seconds();
	if (bActionLocked && Action != EFlowAction::ShowResult)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlowAction locked, reject action=%s source=%s ts=%s"),
			FlowActionToString(Action), *Source, *FDateTime::UtcNow().ToIso8601());
		return false;
	}

	if (Action == LastAction && (Now - LastActionRealSeconds) <= ActionDebounceWindowSeconds)
	{
		UE_LOG(LogTemp, Verbose, TEXT("FlowAction debounce drop action=%s source=%s"),
			FlowActionToString(Action), *Source);
		return false;
	}
	LastAction = Action;
	LastActionRealSeconds = Now;

	switch (Action)
	{
	case EFlowAction::Start:
		if (!TransitionToState(EFlowState::InGame, Source))
		{
			return false;
		}
		if (!StartGameLevelName.IsNone())
		{
			const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
			if (!CurrentLevelName.Equals(FPackageName::GetShortName(StartGameLevelName.ToString()), ESearchCase::IgnoreCase))
			{
				OpenLevelWithActionLock(StartGameLevelName);
			}
		}
		return true;

	case EFlowAction::Retry:
	{
		if (!TransitionToState(EFlowState::Transition, Source))
		{
			return false;
		}
		const FName CurrentLevelName(*UGameplayStatics::GetCurrentLevelName(this, true));
		if (!CurrentLevelName.IsNone())
		{
			OpenLevelWithActionLock(CurrentLevelName);
			return true;
		}
		return false;
	}

	case EFlowAction::ReturnMenu:
		if (CurrentState == EFlowState::Paused)
		{
			SaveCurrentGame();
			if (!TransitionToState(EFlowState::MainMenu, Source))
			{
				return false;
			}
		}
		else if (CurrentState == EFlowState::Result)
		{
			if (!TransitionToState(EFlowState::Transition, Source))
			{
				return false;
			}
		}
		else
		{
			LogRejectedTransition(EFlowState::MainMenu, Source);
			return false;
		}
		if (MainMenuLevelName.IsNone())
		{
			return false;
		}
		OpenLevelWithActionLock(MainMenuLevelName);
		return true;

	case EFlowAction::Quit:
		UKismetSystemLibrary::QuitGame(this, LocalPlayerController, EQuitPreference::Quit, false);
		return true;

	case EFlowAction::Pause:
		return TransitionToState(EFlowState::Paused, Source);

	case EFlowAction::Resume:
		return TransitionToState(EFlowState::InGame, Source);

	case EFlowAction::ShowResult:
		if (bResultShown)
		{
			UE_LOG(LogTemp, Warning, TEXT("ShowResult ignored due to bResultShown source=%s"), *Source);
			return false;
		}
		if (Payload && Payload->bHasGameResultData)
		{
			LastResultData = Payload->GameResultData;
			bHasLastResultData = true;
		}
		bResultShown = true;
		EnsureResultMenuCreated();
		if (!ResultMenuInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("ShowResult failed: ResultMenuInstance is null. Check W_End asset and ResultMenuClass binding."));
			return false;
		}
			if (ResultMenuInstance && bHasLastResultData)
			{
				ResultMenuInstance->SetResultText(LastResultData.ResultType == EGameResultType::Win
					? FText::FromString(TEXT("胜利"))
					: FText::FromString(TEXT("失败")));
				ResultMenuInstance->SetGameResultData(LastResultData);
			}
		return TransitionToState(EFlowState::Result, Source);
	default:
		return false;
	}
}

void UUIManagerComponent::SetAmmo(int32 InClipAmmo, int32 InTotalAmmo, float InReloadAlpha)
{
	if (!AmmoWidget)
	{
		EnsureHUDCreated();
	}

	if (AmmoWidget)
	{
		AmmoWidget->SetAmmo(InClipAmmo, InTotalAmmo, InReloadAlpha);
	}
}

void UUIManagerComponent::UpdateLevelInfo(int32 Level, float CurrentHP, float MaxHP, float DamageMult)
{
	if (!AmmoWidget)
	{
		EnsureHUDCreated();
	}

	if (AmmoWidget)
	{
		AmmoWidget->SetLevelInfo(Level, CurrentHP, MaxHP, DamageMult);
	}
}

void UUIManagerComponent::UpdateWeaponSlotsUI(const TArray<FWeaponSlot>& Slots, int32 ActiveSlot)
{
	if (!AmmoWidget)
	{
		EnsureHUDCreated();
	}

	if (AmmoWidget)
	{
		for (int32 Index = 0; Index < Slots.Num(); ++Index)
		{
			UTexture2D* Icon = nullptr;
			if (Slots.IsValidIndex(Index) && Slots[Index].WeaponActor)
			{
				if (const UWeapon* WeaponData = Slots[Index].WeaponActor->DataAsset_Implementation())
				{
					Icon = WeaponData->WeaponIcon;
				}
			}
			AmmoWidget->SetWeaponSlotIcon(Index, Icon);
		}

		AmmoWidget->SetActiveWeaponSlot(ActiveSlot);
	}

	RefreshBackpackUI();
}

void UUIManagerComponent::ShowHitNumber(int32 Damage, FVector WorldLocation, bool bHeadShot)
{
	if (!LocalPlayerController || !HUDRootInstance || !HitNumberWidgetClass)
	{
		return;
	}

	UCanvasPanel* HitNumberLayer = HUDRootInstance->GetHitNumberLayer();
	if (!HitNumberLayer)
	{
		return;
	}

	FVector2D ScreenPosition;
	const bool bProjected = UGameplayStatics::ProjectWorldToScreen(LocalPlayerController, WorldLocation, ScreenPosition);
	if (!bProjected)
	{
		return;
	}

	UUserWidget* HitNumberWidget = CreateWidget<UUserWidget>(LocalPlayerController, HitNumberWidgetClass);
	if (!HitNumberWidget)
	{
		return;
	}
	//�˺�ֵ����
	// ��ͼ��ѡʵ�֣����������˺������ı��ͱ�ͷ��ʽ��
	struct FHitNumberDataParams
	{
		int32 InDamage = 0;
		bool bInHeadShot = false;
	};

	static const FName FuncName = TEXT("SetHitNumberData");
	if (UFunction* SetDataFunc = HitNumberWidget->FindFunction(FuncName))
	{
		FHitNumberDataParams Params;
		Params.InDamage = Damage;
		Params.bInHeadShot = bHeadShot;
		HitNumberWidget->ProcessEvent(SetDataFunc, &Params);
	}
		// ���ӵ��������ֲ㣬��������Ļλ�á�
		//HitNumberWidget->SetVisibility(ESlateVisibility::Visible);
		if (UCanvasPanelSlot* Slot = HitNumberLayer->AddChildToCanvas(HitNumberWidget))
		{
			Slot->SetAutoSize(true);
			//Slot->SetAlignment(FVector2D(0.5f, 0.5f));
			//Slot->SetPosition(ScreenPosition + FVector2D(0.0f, -20.0f));
			//Slot->SetZOrder(50);

			//// ĳЩ�����֣����� Widget ���� CanvasPanel�����ܵ��� AutoSize Ϊ 0����һ�����׳ߴ硣
			//if (HitNumberWidget->GetDesiredSize().IsNearlyZero())
			//{
			//	Slot->SetAutoSize(false);
			//	Slot->SetSize(FVector2D(96.0f, 48.0f));
			//}
		}
}

void UUIManagerComponent::UpdateEnemyTarget(ACombatCharacter* InEnemy)
{
	if (InEnemy == CurrentEnemyTarget)
	{
		return;
	}

	ClearEnemyTarget();

	if (!InEnemy || InEnemy == CombatCharacter)
	{
		return;
	}

	CurrentEnemyTarget = InEnemy;
	BoundEnemyHealthComponent = InEnemy->GetHealthComponent();
	if (!BoundEnemyHealthComponent || !EnemyHealthWidget)
	{
		CurrentEnemyTarget = nullptr;
		BoundEnemyHealthComponent = nullptr;
		return;
	}

	BoundEnemyHealthComponent->OnHealthChanged.AddUniqueDynamic(this, &UUIManagerComponent::OnEnemyHealthChanged);
	BoundEnemyHealthComponent->OnDeath.AddUniqueDynamic(this, &UUIManagerComponent::OnEnemyDeath);
	EnemyHealthWidget->SetVisibility(ESlateVisibility::Visible);
	OnEnemyHealthChanged(BoundEnemyHealthComponent, BoundEnemyHealthComponent->GetCurrentHealth(), 0.0f, nullptr);
}

void UUIManagerComponent::ClearEnemyTarget()
{
	if (BoundEnemyHealthComponent)
	{
		BoundEnemyHealthComponent->OnHealthChanged.RemoveDynamic(this, &UUIManagerComponent::OnEnemyHealthChanged);
		BoundEnemyHealthComponent->OnDeath.RemoveDynamic(this, &UUIManagerComponent::OnEnemyDeath);
		BoundEnemyHealthComponent = nullptr;
	}

	CurrentEnemyTarget = nullptr;
	if (EnemyHealthWidget)
	{
		EnemyHealthWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UUIManagerComponent::EnsureHUDCreated()
{
	if (!LocalPlayerController)
	{
		return;
	}

	if (HUDRootInstance || HealthWidget || AmmoWidget)
	{
		return;
	}

	if (HUDRootClass)
	{
		HUDRootInstance = CreateWidget<UHUDRootWidget>(LocalPlayerController, HUDRootClass);
		if (!HUDRootInstance)
		{
			return;
		}

		ShowWidget(HUDRootInstance, 0);
		CacheHUDChildren();
		return;
	}

	if (HealthWidgetClass)
	{
		HealthWidget = CreateWidget<UHealthWidget>(LocalPlayerController, HealthWidgetClass);
		ShowWidget(HealthWidget, 0);
	}

	if (AmmoWidgetClass)
	{
		AmmoWidget = CreateWidget<UAmmoWidget>(LocalPlayerController, AmmoWidgetClass);
		ShowWidget(AmmoWidget, 0);
	}

	if (!MinimapWidget && MinimapWidgetClass)
	{
		MinimapWidget = CreateWidget<UMinimapWidget>(LocalPlayerController, MinimapWidgetClass);
		ShowWidget(MinimapWidget, 0);
	}
}

void UUIManagerComponent::EnsureStartMenuCreated()
{
	if (StartMenuInstance || !StartMenuClass || !LocalPlayerController)
	{
		return;
	}

	StartMenuInstance = CreateWidget<UUserWidget>(LocalPlayerController, StartMenuClass);
	if (UBeginUserWidget* BeginMenu = Cast<UBeginUserWidget>(StartMenuInstance))
	{
		BeginMenu->OnStartClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleStartRequested);
		BeginMenu->OnContinueClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleStartContinueRequested);
		BeginMenu->OnSetClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleStartSettingsRequested);
		BeginMenu->OnEndClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandlePauseQuitRequested);
		BeginMenu->SetContinueButtonVisible(HasSaveFile());
	}
}

void UUIManagerComponent::EnsureSettingsMenuCreated()
{
	if (SettingsMenuInstance || !SettingsMenuClass || !LocalPlayerController)
	{
		return;
	}

	SettingsMenuInstance = CreateWidget<USettingsUserWidget>(LocalPlayerController, SettingsMenuClass);
	if (SettingsMenuInstance)
	{
		SettingsMenuInstance->OnBackClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleSettingsBackRequested);
		SettingsMenuInstance->OnClearSaveClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleSettingsClearSaveRequested);
		SettingsMenuInstance->OnApplyClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleSettingsApplied);
	}
}

void UUIManagerComponent::EnsurePauseMenuCreated()
{
	if (PauseMenuInstance || !PauseMenuClass || !LocalPlayerController)
	{
		return;
	}

	PauseMenuInstance = CreateWidget<UUserWidget>(LocalPlayerController, PauseMenuClass);
	if (USet_UserWidget* PauseMenu = Cast<USet_UserWidget>(PauseMenuInstance))
	{
		PauseMenu->OnContinueClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandlePauseResumeRequested);
		PauseMenu->OnSaveClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandlePauseSaveRequested);
		PauseMenu->OnEndClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandlePauseQuitRequested);
		PauseMenu->OnReturnClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandlePauseReturnRequested);
	}
}

void UUIManagerComponent::EnsureResultMenuCreated()
{
	if (ResultMenuInstance || !ResultMenuClass || !LocalPlayerController)
	{
		return;
	}

	ResultMenuInstance = CreateWidget<UEndUserWidget>(LocalPlayerController, ResultMenuClass);
	if (ResultMenuInstance)
	{
		ResultMenuInstance->OnRetryClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleResultRetryRequested);
		ResultMenuInstance->OnNextLevelClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleResultNextLevelRequested);
		ResultMenuInstance->OnReturnClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleResultReturnRequested);
		ResultMenuInstance->OnQuitClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandlePauseQuitRequested);
		ResultMenuInstance->SetNextLevelButtonVisible(GetMaxUnlockedLevel() > 1);
	}
}

void UUIManagerComponent::EnsureAimWidgetsCreated()
{
	if (!LocalPlayerController)
	{
		return;
	}

	if (!HipAimWidgetInstance && HipAimWidgetClass)
	{
		HipAimWidgetInstance = CreateWidget<UUserWidget>(LocalPlayerController, HipAimWidgetClass);
	}

	if (!SightAimWidgetInstance && SightAimWidgetClass)
	{
		SightAimWidgetInstance = CreateWidget<UUserWidget>(LocalPlayerController, SightAimWidgetClass);
	}
}

void UUIManagerComponent::EnsureBackpackCreated()
{
	if (!LocalPlayerController || BackpackWidgetInstance || !BackpackWidgetClass)
	{
		return;
	}

	BackpackWidgetInstance = CreateWidget<UBackpackWidget>(LocalPlayerController, BackpackWidgetClass);
	if (!BackpackWidgetInstance)
	{
		return;
	}

	BackpackWidgetInstance->OnUseClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleUseSelectedItem);
	BackpackWidgetInstance->OnDiscardClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleDiscardSelectedItem);
	BackpackWidgetInstance->OnCloseClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleCloseBackpack);
	BackpackWidgetInstance->OnItemClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleItemClicked);
	BackpackWidgetInstance->OnItemDoubleClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleItemDoubleClicked);
	RefreshBackpackUI();
}

void UUIManagerComponent::EnsureBackpackHintCreated()
{
	if (!LocalPlayerController || BackpackHintInstance || !BackpackHintClass)
	{
		return;
	}

	BackpackHintInstance = CreateWidget<UUserWidget>(LocalPlayerController, BackpackHintClass);
}

void UUIManagerComponent::EnsureToastBannerCreated()
{
	if (!LocalPlayerController || ToastBannerInstance || !ToastBannerClass)
	{
		return;
	}

	ToastBannerInstance = CreateWidget<UToastBannerWidget>(LocalPlayerController, ToastBannerClass);
	if (ToastBannerInstance)
	{
		ShowWidget(ToastBannerInstance, 200);
		ToastBannerInstance->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUIManagerComponent::ApplyState(EFlowState NewState)
{
	EnsureHUDCreated();

	switch (NewState)
	{
	case EFlowState::InGame:
		ShowWidget(HUDRootInstance, 0);
		ShowWidget(MinimapWidget, 0);
		if (!HUDRootInstance)
		{
			ShowWidget(HealthWidget, 0);
			ShowWidget(AmmoWidget, 0);
		}
		HideWidget(StartMenuInstance);
		HideWidget(PauseMenuInstance);
		HideWidget(SettingsMenuInstance);
		HideWidget(ResultMenuInstance);
		EnsureBackpackHintCreated();
		if (bBackpackVisible)
		{
			HideWidget(BackpackHintInstance);
			ShowWidget(BackpackWidgetInstance, 80);
		}
		else
		{
			ShowWidget(BackpackHintInstance, 100);
			HideWidget(BackpackWidgetInstance);
		}
		ApplyInputModeForState(NewState);
		break;

	case EFlowState::Paused:
		EnsurePauseMenuCreated();
		ShowWidget(HUDRootInstance, 0);
		ShowWidget(MinimapWidget, 0);
		if (!HUDRootInstance)
		{
			ShowWidget(HealthWidget, 0);
			ShowWidget(AmmoWidget, 0);
		}
		HideWidget(StartMenuInstance);
		HideWidget(SettingsMenuInstance);
		HideWidget(ResultMenuInstance);
		ShowWidget(PauseMenuInstance, 100);
		HideWidget(BackpackHintInstance);
		if (bBackpackVisible)
		{
			ShowWidget(BackpackWidgetInstance, 80);
		}
		else
		{
			HideWidget(BackpackWidgetInstance);
		}
		ApplyInputModeForState(NewState);
		break;

	case EFlowState::MainMenu:
		EnsureStartMenuCreated();
		HideWidget(HUDRootInstance);
		HideWidget(MinimapWidget);
		if (!HUDRootInstance)
		{
			HideWidget(HealthWidget);
			HideWidget(AmmoWidget);
		}
		ClearEnemyTarget();
		HideWidget(BackpackHintInstance);
		HideWidget(PauseMenuInstance);
		HideWidget(SettingsMenuInstance);
		HideWidget(ResultMenuInstance);
		HideWidget(BackpackWidgetInstance);
		ShowWidget(StartMenuInstance, 100);

		// 「继续游戏」按钮：有存档才显示
		if (StartMenuInstance)
		{
			if (UButton* ContinueBtn = Cast<UButton>(StartMenuInstance->GetWidgetFromName(TEXT("ContinueBtn"))))
			{
				ContinueBtn->SetVisibility(
					HasSaveFile() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			}
		}

		ApplyInputModeForState(NewState);
		break;

	case EFlowState::Result:
		EnsureResultMenuCreated();
		if (!ResultMenuInstance)
		{
			UE_LOG(LogTemp, Error, TEXT("ApplyState(Result) failed: ResultMenuInstance is null."));
			UGameplayStatics::SetGamePaused(this, false);
			LocalPlayerController->SetShowMouseCursor(false);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(LocalPlayerController);
			break;
		}
		HideWidget(PauseMenuInstance);
		HideWidget(StartMenuInstance);
		HideWidget(SettingsMenuInstance);
		ShowWidget(HUDRootInstance, 0);
		HideWidget(MinimapWidget);
		if (!HUDRootInstance)
		{
			ShowWidget(HealthWidget, 0);
			ShowWidget(AmmoWidget, 0);
		}
		HideWidget(BackpackHintInstance);
		ShowWidget(ResultMenuInstance, 120);
		HideWidget(BackpackWidgetInstance);

		// 「下一关」按钮：胜利才显示
		if (ResultMenuInstance)
		{
			if (UButton* NextLevelBtn = Cast<UButton>(ResultMenuInstance->GetWidgetFromName(TEXT("NextLevelBtn"))))
			{
				const bool bShowNext = bHasLastResultData
					&& LastResultData.ResultType == EGameResultType::Win
					&& LevelNumberToMapName(GetContinueLevelNumber()) != NAME_None;
				NextLevelBtn->SetVisibility(
					bShowNext ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			}
		}

		ApplyInputModeForState(NewState);
		break;

	case EFlowState::Transition:
		HideWidget(PauseMenuInstance);
		HideWidget(ResultMenuInstance);
		HideWidget(StartMenuInstance);
		HideWidget(SettingsMenuInstance);
		HideWidget(BackpackHintInstance);
		HideWidget(BackpackWidgetInstance);
		break;

	default:
		break;
	}

	UpdateAimWidgetsVisibility();
	RefreshMedkitHUD();
}

void UUIManagerComponent::ApplyInputModeForState(EFlowState NewState)
{
	if (!LocalPlayerController)
	{
		return;
	}

	switch (NewState)
	{
	case EFlowState::Paused:
		UGameplayStatics::SetGamePaused(this, true);
		LocalPlayerController->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(LocalPlayerController, PauseMenuInstance, EMouseLockMode::DoNotLock);
		break;

	case EFlowState::MainMenu:
		UGameplayStatics::SetGamePaused(this, false);
		LocalPlayerController->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(LocalPlayerController, StartMenuInstance, EMouseLockMode::DoNotLock);
		break;

	case EFlowState::Result:
		UGameplayStatics::SetGamePaused(this, true);
		LocalPlayerController->SetShowMouseCursor(true);
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(LocalPlayerController, ResultMenuInstance, EMouseLockMode::DoNotLock);
		break;

	case EFlowState::InGame:
		UGameplayStatics::SetGamePaused(this, false);
		if (bBackpackVisible && BackpackWidgetInstance && BackpackWidgetInstance->IsInViewport())
		{
			LocalPlayerController->SetShowMouseCursor(true);
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(
				LocalPlayerController,
				BackpackWidgetInstance,
				EMouseLockMode::DoNotLock,
				false);
		}
		else
		{
			LocalPlayerController->SetShowMouseCursor(false);
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(LocalPlayerController);
		}
		break;

	case EFlowState::Transition:
	default:
		UGameplayStatics::SetGamePaused(this, false);
		LocalPlayerController->SetShowMouseCursor(false);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(LocalPlayerController);
		break;
	}
}

void UUIManagerComponent::UpdateAimWidgetsVisibility()
{
	EnsureAimWidgetsCreated();

	if (CurrentState != EFlowState::InGame)
	{
		HideWidget(HipAimWidgetInstance);
		HideWidget(SightAimWidgetInstance);
		return;
	}

	if (bIsAiming)
	{
		HideWidget(HipAimWidgetInstance);
		ShowWidget(SightAimWidgetInstance, 10);
		return;
	}

	HideWidget(SightAimWidgetInstance);
	ShowWidget(HipAimWidgetInstance, 10);
}

void UUIManagerComponent::ShowWidget(UUserWidget* InWidget, int32 ZOrder)
{
	if (!InWidget)
	{
		return;
	}

	if (!InWidget->IsInViewport())
	{
		InWidget->AddToViewport(ZOrder);
	}
	InWidget->SetVisibility(ESlateVisibility::Visible);
}

void UUIManagerComponent::HideWidget(UUserWidget* InWidget)
{
	if (!InWidget)
	{
		return;
	}
	if (InWidget->IsInViewport())
	{
		InWidget->RemoveFromParent();
	}
}

void UUIManagerComponent::CacheHUDChildren()
{
	HealthWidget = HUDRootInstance ? HUDRootInstance->GetHealthWidget() : nullptr;
	AmmoWidget = HUDRootInstance ? HUDRootInstance->GetAmmoWidget() : nullptr;
	EnemyHealthWidget = HUDRootInstance ? HUDRootInstance->GetEnemyHealthWidget() : nullptr;
	if (HUDRootInstance)
	{
		HUDRootInstance->OnMedkitClicked.AddUniqueDynamic(this, &UUIManagerComponent::HandleHUDMedkitRequested);
	}
	if (EnemyHealthWidget)
	{
		EnemyHealthWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	RefreshMedkitHUD();
}

void UUIManagerComponent::UnbindHealth()
{
	if (!BoundHealthComponent)
	{
		return;
	}

	BoundHealthComponent->OnHealthChanged.RemoveDynamic(this, &UUIManagerComponent::OnHealthChanged);
	BoundHealthComponent->OnDeath.RemoveDynamic(this, &UUIManagerComponent::OnCharacterDeath);
	BoundHealthComponent = nullptr;
}

void UUIManagerComponent::UnbindInventory()
{
	if (!BoundInventoryComponent)
	{
		return;
	}
	BoundInventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UUIManagerComponent::OnInventoryChanged);
	BoundInventoryComponent = nullptr;
}

void UUIManagerComponent::BindInventory()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	BoundInventoryComponent = PlayerChar ? PlayerChar->GetInventoryComponent() : nullptr;
	if (!BoundInventoryComponent)
	{
		RefreshMedkitHUD();
		return;
	}

	BoundInventoryComponent->OnInventoryChanged.AddUniqueDynamic(this, &UUIManagerComponent::OnInventoryChanged);
	RefreshMedkitHUD();
	RefreshBackpackUI();
}

void UUIManagerComponent::OnHealthChanged(UHealthComponent* InHealthComponent, float NewHealth, float Delta, AActor* InstigatorActor)
{
	if (!InHealthComponent)
	{
		return;
	}

	if (HealthWidget)
	{
		HealthWidget->SetHealthPercent(InHealthComponent->GetHealthPercent());
	}
	RefreshMedkitHUD();
	RefreshBackpackUI();
}

void UUIManagerComponent::OnCharacterDeath(AActor* DeadActor)
{
	SetAiming(false);
	ClearEnemyTarget();
	RefreshMedkitHUD();
}

void UUIManagerComponent::OnEnemyHealthChanged(UHealthComponent* InHealthComponent, float NewHealth, float Delta, AActor* InstigatorActor)
{
	if (!InHealthComponent || !EnemyHealthWidget)
	{
		return;
	}

	EnemyHealthWidget->SetHealthPercent(InHealthComponent->GetHealthPercent());
}

void UUIManagerComponent::OnEnemyDeath(AActor* DeadActor)
{
	ClearEnemyTarget();
}

void UUIManagerComponent::HandleStartRequested()
{
	StartNewGame();
}

void UUIManagerComponent::HandlePauseResumeRequested()
{
	FFlowActionPayload Payload;
	Payload.Source = TEXT("PauseMenu.Continue");
	RequestFlowAction(EFlowAction::Resume, &Payload);
}

void UUIManagerComponent::HandlePauseQuitRequested()
{
	FFlowActionPayload Payload;
	Payload.Source = TEXT("Menu.Quit");
	RequestFlowAction(EFlowAction::Quit, &Payload);
}

void UUIManagerComponent::HandlePauseReturnRequested()
{
	FFlowActionPayload Payload;
	Payload.Source = TEXT("PauseMenu.Return");
	RequestFlowAction(EFlowAction::ReturnMenu, &Payload);
}

void UUIManagerComponent::HandleResultRetryRequested()
{
	FFlowActionPayload Payload;
	Payload.Source = TEXT("ResultMenu.Retry");
	RequestFlowAction(EFlowAction::Retry, &Payload);
}

void UUIManagerComponent::HandleResultReturnRequested()
{
	FFlowActionPayload Payload;
	Payload.Source = TEXT("ResultMenu.Return");
	RequestFlowAction(EFlowAction::ReturnMenu, &Payload);
}

void UUIManagerComponent::HandleStartContinueRequested()
{
	ContinueGame();
}

void UUIManagerComponent::HandleStartSettingsRequested()
{
	EnsureSettingsMenuCreated();
	if (!SettingsMenuInstance || !LocalPlayerController)
	{
		return;
	}

	HideWidget(StartMenuInstance);
	SettingsMenuInstance->RefreshControlsFromSavedSettings();
	ShowWidget(SettingsMenuInstance, 150);
	LocalPlayerController->SetShowMouseCursor(true);
	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(LocalPlayerController, SettingsMenuInstance, EMouseLockMode::DoNotLock);
}

void UUIManagerComponent::HandleSettingsBackRequested()
{
	HideWidget(SettingsMenuInstance);
	if (CurrentState == EFlowState::MainMenu && LocalPlayerController)
	{
		ShowWidget(StartMenuInstance, 100);
		if (StartMenuInstance)
		{
			if (UButton* ContinueBtn = Cast<UButton>(StartMenuInstance->GetWidgetFromName(TEXT("ContinueBtn"))))
			{
				ContinueBtn->SetVisibility(HasSaveFile() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			}
		}
		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(LocalPlayerController, StartMenuInstance, EMouseLockMode::DoNotLock);
	}
	else
	{
		ApplyInputModeForState(CurrentState);
	}
}

void UUIManagerComponent::HandleSettingsClearSaveRequested()
{
	UE_LOG(LogTemp, Warning, TEXT("[UIManager] Clear save requested from settings menu."));

	FCYYSaveManager::DeleteSave();
	CachedSaveData = nullptr;

	if (StartMenuInstance)
	{
		if (UButton* ContinueBtn = Cast<UButton>(StartMenuInstance->GetWidgetFromName(TEXT("ContinueBtn"))))
		{
			ContinueBtn->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	ShowPickupBanner(TEXT("存档已清除"));
}

void UUIManagerComponent::HandleSettingsApplied()
{
	ShowPickupBanner(TEXT("设置已应用"));
}

void UUIManagerComponent::HandlePauseSaveRequested()
{
	SaveCurrentGame();
}

void UUIManagerComponent::HandleResultNextLevelRequested()
{
	PlayNextLevel();
}

void UUIManagerComponent::OnInventoryChanged(int32 NewMedkitCount)
{
	RefreshMedkitHUD();
	RefreshBackpackUI();
}

void UUIManagerComponent::RefreshMedkitHUD()
{
	if (!HUDRootInstance)
	{
		return;
	}

	const ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	const UInventoryComponent* Inventory = PlayerChar ? PlayerChar->GetInventoryComponent() : nullptr;
	const UHealthComponent* Health = PlayerChar ? PlayerChar->GetHealthComponent() : nullptr;

	const int32 MedkitCount = Inventory ? Inventory->GetMedkitCount() : 0;
	const bool bFullHealth = Health ? (Health->GetCurrentHealth() >= Health->GetMaxHealth() - KINDA_SMALL_NUMBER) : true;
	const bool bFlowBlocked = (CurrentState == EFlowState::Result || CurrentState == EFlowState::Transition);
	const bool bCanUse = (MedkitCount > 0) && !bFullHealth && !bFlowBlocked;

	FText DisabledReason = FText::GetEmpty();
	if (MedkitCount <= 0)
	{
		DisabledReason = FText::FromString(TEXT("没有医疗包"));
	}
	else if (bFullHealth)
	{
		DisabledReason = FText::FromString(TEXT("生命值已满"));
	}
	else if (bFlowBlocked)
	{
		DisabledReason = FText::FromString(TEXT("当前状态不可用"));
	}

	HUDRootInstance->SetMedkitState(MedkitCount, bCanUse, DisabledReason);
}

void UUIManagerComponent::HandleHUDMedkitRequested()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	if (!PlayerChar)
	{
		return;
	}

	const EMedkitUseResult Result = PlayerChar->TryUseMedkit();
	ShowMedkitUseFeedback(Result);
	RefreshMedkitHUD();
	RefreshBackpackUI();
}

void UUIManagerComponent::HandleUseSelectedItem()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	if (!PlayerChar || !BackpackWidgetInstance)
	{
		return;
	}

	const FBackpackItemEntry* SelectedEntry = BackpackWidgetInstance->GetSelectedEntry();
	if (!SelectedEntry || !SelectedEntry->bCanUse)
	{
		return;
	}

	if (SelectedEntry->ItemType == EBackpackItemType::Medkit)
	{
		const EMedkitUseResult Result = PlayerChar->TryUseMedkit();
		ShowMedkitUseFeedback(Result);
		RefreshBackpackUI();
		return;
	}

	if (SelectedEntry->ItemType == EBackpackItemType::Weapon)
	{
		if (SelectedEntry->bInBackpackStorage && SelectedEntry->BackpackIndex >= 0)
		{
			PlayerChar->EquipFromBackpack(SelectedEntry->BackpackIndex);
		}
		else if (SelectedEntry->WeaponSlotIndex >= 0)
		{
			PlayerChar->SwitchWeapon(SelectedEntry->WeaponSlotIndex);
		}
		RefreshBackpackUI();
	}
}

void UUIManagerComponent::HandleDiscardSelectedItem()
{
	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	if (!PlayerChar || !BackpackWidgetInstance)
	{
		return;
	}

	const FBackpackItemEntry* SelectedEntry = BackpackWidgetInstance->GetSelectedEntry();
	if (!SelectedEntry)
	{
		return;
	}

	if (SelectedEntry->ItemType == EBackpackItemType::Medkit)
	{
		if (UInventoryComponent* Inventory = PlayerChar->GetInventoryComponent())
		{
			Inventory->ConsumeMedkit();
		}
		RefreshBackpackUI();
		return;
	}

	if (SelectedEntry->ItemType != EBackpackItemType::Weapon)
	{
		return;
	}

	if (SelectedEntry->bInBackpackStorage)
	{
		TArray<FWeaponSlot>& BackpackSlots = PlayerChar->GetBackpackWeaponsMutable();
		if (!BackpackSlots.IsValidIndex(SelectedEntry->BackpackIndex))
		{
			return;
		}

		if (BackpackSlots[SelectedEntry->BackpackIndex].WeaponActor)
		{
			BackpackSlots[SelectedEntry->BackpackIndex].WeaponActor->Destroy();
		}
		BackpackSlots.RemoveAt(SelectedEntry->BackpackIndex);
		RefreshBackpackUI();
		return;
	}

	if (SelectedEntry->WeaponSlotIndex >= 0)
	{
		TArray<FWeaponSlot>& EquippedSlots = PlayerChar->GetWeaponSlotsMutable();
		if (!EquippedSlots.IsValidIndex(SelectedEntry->WeaponSlotIndex))
		{
			return;
		}

		const int32 RemovedSlotIndex = SelectedEntry->WeaponSlotIndex;
		const bool bRemovedActive = (PlayerChar->GetActiveWeaponSlot() == RemovedSlotIndex);
		FWeaponSlot RemovedSlot = EquippedSlots[RemovedSlotIndex];
		E_Weapon RemovedWeaponType = E_Weapon::NoWeapon;
		if (RemovedSlot.WeaponActor)
		{
			if (const UWeapon* RemovedWeaponData = RemovedSlot.WeaponActor->DataAsset_Implementation())
			{
				RemovedWeaponType = RemovedWeaponData->CurrentWeapon;
			}
			RemovedSlot.WeaponActor->Destroy();
		}

		EquippedSlots[RemovedSlotIndex].WeaponActor = nullptr;
		EquippedSlots[RemovedSlotIndex].ClipAmmo = 0;
		EquippedSlots[RemovedSlotIndex].TotalAmmo = 0;

		int32 ReplacementBackpackIndex = INDEX_NONE;
		TArray<FWeaponSlot>& BackpackSlots = PlayerChar->GetBackpackWeaponsMutable();
		for (int32 Index = 0; Index < BackpackSlots.Num(); ++Index)
		{
			const FWeaponSlot& Candidate = BackpackSlots[Index];
			if (!Candidate.WeaponActor)
			{
				continue;
			}

			if (const UWeapon* CandidateData = Candidate.WeaponActor->DataAsset_Implementation())
			{
				if (CandidateData->CurrentWeapon == RemovedWeaponType)
				{
					ReplacementBackpackIndex = Index;
					break;
				}
			}
		}

		if (ReplacementBackpackIndex != INDEX_NONE)
		{
			if (bRemovedActive)
			{
				PlayerChar->EquipFromBackpack(ReplacementBackpackIndex);
			}
			else if (BackpackSlots.IsValidIndex(ReplacementBackpackIndex))
			{
				FWeaponSlot ReplacementSlot = BackpackSlots[ReplacementBackpackIndex];
				BackpackSlots.RemoveAt(ReplacementBackpackIndex);
				if (ReplacementSlot.WeaponActor)
				{
					ReplacementSlot.WeaponActor->SetOwner(PlayerChar);
					ReplacementSlot.WeaponActor->EquitmentWeapon();
					ReplacementSlot.WeaponActor->SetActorHiddenInGame(true);
				}
				EquippedSlots[RemovedSlotIndex] = ReplacementSlot;
				PlayerChar->UpdateAmmoUI(0.0f);
			}
		}
		else if (bRemovedActive)
		{
			PlayerChar->SwitchWeapon(RemovedSlotIndex);
		}
		else
		{
			PlayerChar->UpdateAmmoUI(0.0f);
		}

		RefreshBackpackUI();
	}
}

void UUIManagerComponent::HandleCloseBackpack()
{
	AutoCloseBackpack();
}

void UUIManagerComponent::HandleItemClicked(int32 ItemIndex)
{
	(void)ItemIndex;
}

void UUIManagerComponent::HandleItemDoubleClicked(int32 ItemIndex)
{
	(void)ItemIndex;
	HandleUseSelectedItem();
}

void UUIManagerComponent::RefreshBackpackUI()
{
	if (!BackpackWidgetInstance)
	{
		return;
	}

	const ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	if (!PlayerChar)
	{
		BackpackWidgetInstance->RebuildItems(TArray<FBackpackItemEntry>());
		return;
	}

	const UInventoryComponent* Inventory = PlayerChar->GetInventoryComponent();
	const UHealthComponent* Health = PlayerChar->GetHealthComponent();
	const float CurrentHP = Health ? Health->GetCurrentHealth() : 0.0f;
	const float MaxHP = Health ? Health->GetMaxHealth() : 0.0f;

	const int32 MedkitCount = Inventory ? Inventory->GetMedkitCount() : 0;
	const bool bFullHealth = Health ? (Health->GetCurrentHealth() >= Health->GetMaxHealth() - KINDA_SMALL_NUMBER) : true;
	const bool bFlowBlocked = (CurrentState == EFlowState::Result || CurrentState == EFlowState::Transition);

	TArray<FBackpackItemEntry> Entries;

	FBackpackItemEntry MedkitEntry;
	MedkitEntry.ItemType = EBackpackItemType::Medkit;
	MedkitEntry.Icon = MedkitIcon;
	MedkitEntry.DisplayName = FText::FromString(TEXT("医疗包"));
	MedkitEntry.Description = FText::FromString(
		FString::Printf(TEXT("恢复%.0f生命值 | 当前生命: %.0f / %.0f"), ACYYCharacterFather::MedkitHealAmount, CurrentHP, MaxHP));
	MedkitEntry.StackCount = MedkitCount;
	MedkitEntry.bCanUse = (MedkitCount > 0) && !bFullHealth && !bFlowBlocked;
	MedkitEntry.UseButtonText = FText::FromString(TEXT("使用"));
	if (MedkitCount <= 0)
	{
		MedkitEntry.DisabledReason = FText::FromString(TEXT("没有医疗包"));
	}
	else if (bFullHealth)
	{
		MedkitEntry.DisabledReason = FText::FromString(TEXT("生命值已满"));
	}
	else if (bFlowBlocked)
	{
		MedkitEntry.DisabledReason = FText::FromString(TEXT("当前状态不可用"));
	}
	MedkitEntry.DisplayName = FText::FromString(TEXT("医疗包"));
	MedkitEntry.Description = FText::FromString(
		FString::Printf(TEXT("恢复%.0f生命值 | 当前生命: %.0f / %.0f"), ACYYCharacterFather::MedkitHealAmount, CurrentHP, MaxHP));
	MedkitEntry.UseButtonText = FText::FromString(TEXT("使用"));
	if (MedkitCount <= 0)
	{
		MedkitEntry.DisabledReason = FText::FromString(TEXT("没有医疗包"));
	}
	else if (bFullHealth)
	{
		MedkitEntry.DisabledReason = FText::FromString(TEXT("生命值已满"));
	}
	else if (bFlowBlocked)
	{
		MedkitEntry.DisabledReason = FText::FromString(TEXT("当前状态不可用"));
	}
	Entries.Add(MedkitEntry);

	// Keep backpack weapon entries in sync with the live weapon slots used by AmmoWidget.
	BuildWeaponItemEntries(
		Entries,
		PlayerChar->GetWeaponSlots(),
		PlayerChar->GetBackpackWeapons(),
		PlayerChar->GetActiveWeaponSlot());
	BackpackWidgetInstance->RebuildItems(Entries);
}

void UUIManagerComponent::BuildWeaponItemEntries(
	TArray<FBackpackItemEntry>& OutEntries,
	const TArray<FWeaponSlot>& EquippedSlots,
	const TArray<FWeaponSlot>& BackpackSlots,
	int32 ActiveSlot)
{
	for (int32 Index = 0; Index < EquippedSlots.Num(); ++Index)
	{
		const FWeaponSlot& Slot = EquippedSlots[Index];
		if (!Slot.WeaponActor)
		{
			continue;
		}

		FBackpackItemEntry Entry;
		Entry.ItemType = EBackpackItemType::Weapon;
		Entry.StackCount = 1;
		Entry.WeaponSlotIndex = Index;
		Entry.BackpackIndex = INDEX_NONE;
		Entry.bInBackpackStorage = false;
		Entry.ClipAmmo = Slot.ClipAmmo;
		Entry.TotalAmmo = Slot.TotalAmmo;
		Entry.bIsEquipped = (Index == ActiveSlot);
		Entry.bCanUse = !Entry.bIsEquipped;
		Entry.UseButtonText = FText::FromString(TEXT("装备"));
		Entry.DisabledReason = Entry.bIsEquipped ? FText::FromString(TEXT("已装备")) : FText::GetEmpty();
		Entry.Durability = 0;
		const uint64 PtrValue = reinterpret_cast<uint64>(Slot.WeaponActor);
		Entry.WeaponInstanceId = FGuid(
			static_cast<uint32>((PtrValue >> 32) & 0xFFFFFFFF),
			static_cast<uint32>(PtrValue & 0xFFFFFFFF),
			static_cast<uint32>(Index),
			0xBACC0001);

		if (const UWeapon* WeaponData = Slot.WeaponActor->DataAsset_Implementation())
		{
			Entry.Icon = WeaponData->WeaponIcon;
			Entry.WeaponType = WeaponData->CurrentWeapon;
			Entry.DisplayName = FText::FromString(UEnum::GetValueAsString(WeaponData->CurrentWeapon));
			Entry.Description = FText::FromString(FString::Printf(TEXT("伤害 %.0f  射速 %.1f"), WeaponData->Damage, WeaponData->FireRate));
		}
		else
		{
			Entry.DisplayName = FText::FromString(TEXT("武器"));
		}

		OutEntries.Add(Entry);
	}

	for (int32 Index = 0; Index < BackpackSlots.Num(); ++Index)
	{
		const FWeaponSlot& Slot = BackpackSlots[Index];
		if (!Slot.WeaponActor)
		{
			continue;
		}

		FBackpackItemEntry Entry;
		Entry.ItemType = EBackpackItemType::Weapon;
		Entry.StackCount = 1;
		Entry.WeaponSlotIndex = INDEX_NONE;
		Entry.BackpackIndex = Index;
		Entry.bInBackpackStorage = true;
		Entry.ClipAmmo = Slot.ClipAmmo;
		Entry.TotalAmmo = Slot.TotalAmmo;
		Entry.bIsEquipped = false;
		Entry.bCanUse = true;
		Entry.UseButtonText = FText::FromString(TEXT("装备"));
		Entry.DisabledReason = FText::GetEmpty();
		Entry.Durability = 0;
		const uint64 PtrValue = reinterpret_cast<uint64>(Slot.WeaponActor);
		Entry.WeaponInstanceId = FGuid(
			static_cast<uint32>((PtrValue >> 32) & 0xFFFFFFFF),
			static_cast<uint32>(PtrValue & 0xFFFFFFFF),
			static_cast<uint32>(Index),
			0xBACC0002);

		if (const UWeapon* WeaponData = Slot.WeaponActor->DataAsset_Implementation())
		{
			Entry.Icon = WeaponData->WeaponIcon;
			Entry.WeaponType = WeaponData->CurrentWeapon;
			Entry.DisplayName = FText::FromString(UEnum::GetValueAsString(WeaponData->CurrentWeapon));
			Entry.Description = FText::FromString(FString::Printf(TEXT("伤害 %.0f  射速 %.1f"), WeaponData->Damage, WeaponData->FireRate));
		}
		else
		{
			Entry.DisplayName = FText::FromString(TEXT("武器"));
		}

		OutEntries.Add(Entry);
	}

	// Entries are consumed directly by C++ widget rebuilding (no Blueprint event bridge).
}

void UUIManagerComponent::ShowMedkitUseFeedback(EMedkitUseResult UseResult)
{
	FString Msg;
	switch (UseResult)
	{
	case EMedkitUseResult::Success:
	{
		int32 Remaining = 0;
		if (const ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter))
		{
			if (const UInventoryComponent* Inventory = PlayerChar->GetInventoryComponent())
			{
				Remaining = Inventory->GetMedkitCount();
			}
		}
		Msg = FString::Printf(TEXT("已使用血包 (剩余 x%d)"), Remaining);
		break;
	}
	case EMedkitUseResult::NoItem: Msg = TEXT("没有血包"); break;
	case EMedkitUseResult::FullHealth: Msg = TEXT("生命值已满"); break;
	case EMedkitUseResult::FlowBlocked: Msg = TEXT("当前不可用"); break;
	default: break;
	}

	if (Msg.IsEmpty())
	{
		return;
	}

	EnsureToastBannerCreated();
	if (ToastBannerInstance)
	{
		ToastBannerInstance->ShowMessage(Msg, 2.0f);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *Msg);
	}
}

bool UUIManagerComponent::IsTransitionAllowed(EFlowState FromState, EFlowState ToState) const
{
	switch (FromState)
	{
	case EFlowState::MainMenu: return ToState == EFlowState::InGame;
	case EFlowState::InGame: return ToState == EFlowState::Paused || ToState == EFlowState::Result;
	case EFlowState::Paused: return ToState == EFlowState::InGame || ToState == EFlowState::MainMenu;
	case EFlowState::Result: return ToState == EFlowState::Transition;
	case EFlowState::Transition: return ToState == EFlowState::InGame || ToState == EFlowState::MainMenu;
	default: return false;
	}
}

bool UUIManagerComponent::TransitionToState(EFlowState TargetState, const FString& Source)
{
	if (CurrentState == TargetState)
	{
		return true;
	}

	if (!IsTransitionAllowed(CurrentState, TargetState))
	{
		LogRejectedTransition(TargetState, Source);
		return false;
	}

	CurrentState = TargetState;
	ApplyState(TargetState);
	return true;
}

void UUIManagerComponent::LogRejectedTransition(EFlowState TargetState, const FString& Source) const
{
	UE_LOG(LogTemp, Warning, TEXT("IllegalFlowTransition source=%s from=%s to=%s ts=%s"),
		*Source,
		FlowStateToString(CurrentState),
		FlowStateToString(TargetState),
		*FDateTime::UtcNow().ToIso8601());
}

void UUIManagerComponent::HandleMapLoaded(UWorld* LoadedWorld)
{
	bActionLocked = false;
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);
	const bool bMainMenuLevel = CurrentLevelName.Equals(FPackageName::GetShortName(MainMenuLevelName.ToString()), ESearchCase::IgnoreCase);
	const EFlowState PostLoadTarget = bMainMenuLevel ? EFlowState::MainMenu : EFlowState::InGame;
	if (CurrentState == EFlowState::Transition)
	{
		TransitionToState(PostLoadTarget, TEXT("MapLoaded"));
	}
	else if (CurrentState == EFlowState::MainMenu && !bMainMenuLevel)
	{
		TransitionToState(EFlowState::InGame, TEXT("MapLoaded"));
	}
	if (PostLoadTarget == EFlowState::InGame)
	{
		bBackpackVisible = false;
		bResultShown = false;
		bHasLastResultData = false;
	}
}

void UUIManagerComponent::OpenLevelWithActionLock(FName TargetLevel)
{
	bActionLocked = true;
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UUIManagerComponent::HandleMapLoaded);
	UGameplayStatics::SetGamePaused(this, false);
	UGameplayStatics::OpenLevel(this, TargetLevel);
}

const TCHAR* UUIManagerComponent::FlowStateToString(EFlowState InState)
{
	switch (InState)
	{
	case EFlowState::MainMenu: return TEXT("MainMenu");
	case EFlowState::InGame: return TEXT("InGame");
	case EFlowState::Paused: return TEXT("Paused");
	case EFlowState::Result: return TEXT("Result");
	case EFlowState::Transition: return TEXT("Transition");
	default: return TEXT("Unknown");
	}
}

const TCHAR* UUIManagerComponent::FlowActionToString(EFlowAction InAction)
{
	switch (InAction)
	{
	case EFlowAction::Start: return TEXT("Start");
	case EFlowAction::Retry: return TEXT("Retry");
	case EFlowAction::ReturnMenu: return TEXT("ReturnMenu");
	case EFlowAction::Quit: return TEXT("Quit");
	case EFlowAction::Pause: return TEXT("Pause");
	case EFlowAction::Resume: return TEXT("Resume");
	case EFlowAction::ShowResult: return TEXT("ShowResult");
	default: return TEXT("Unknown");
	}
}

// ═══════════════════════════════════════════
// 存档相关
// ═══════════════════════════════════════════

bool UUIManagerComponent::HasSaveFile() const
{
	return FCYYSaveManager::HasSaveFile();
}

void UUIManagerComponent::ContinueGame()
{
	UCYYSaveGame* Save = FCYYSaveManager::LoadExisting();
	if (!Save || !Save->bHasStartedGame)
	{
		return;
	}

	int32 ContinueLevel = Save->CurrentLevel;
	if (ContinueLevel > 2)
	{
		ContinueLevel = 1;
	}
	CachedSaveData = Save;
	LoadAndStartLevel(ContinueLevel);
}

void UUIManagerComponent::StartNewGame()
{
	FCYYSaveManager::DeleteSave();
	CachedSaveData = FCYYSaveManager::CreateNew();
	LoadAndStartLevel(1);
}

void UUIManagerComponent::LoadAndStartLevel(int32 LevelNumber)
{
	const FName MapName = LevelNumberToMapName(LevelNumber);
	if (MapName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("[UIManager] 无效的关卡编号: %d"), LevelNumber);
		return;
	}

	// 缓存存档数据
	if (!CachedSaveData)
	{
		CachedSaveData = FCYYSaveManager::LoadExisting();
	}
	if (!CachedSaveData)
	{
		CachedSaveData = FCYYSaveManager::CreateNew();
	}
	if (CachedSaveData)
	{
		CachedSaveData->bHasStartedGame = true;
		CachedSaveData->CurrentLevel = LevelNumber;
		FCYYSaveManager::SaveGame(CachedSaveData);
	}

	// 设置目标关卡并执行 Start 流程
	StartGameLevelName = MapName;
	FFlowActionPayload Payload;
	Payload.Source = TEXT("LevelSelect");
	RequestFlowAction(EFlowAction::Start, &Payload);
}

int32 UUIManagerComponent::GetContinueLevelNumber() const
{
	CachedSaveData = FCYYSaveManager::LoadExisting();
	return CachedSaveData ? CachedSaveData->CurrentLevel : 1;
}

bool UUIManagerComponent::IsLevelUnlocked(int32 LevelNumber) const
{
	if (!CachedSaveData)
	{
		CachedSaveData = FCYYSaveManager::LoadExisting();
	}
	return CachedSaveData ? CachedSaveData->IsLevelUnlocked(LevelNumber) : (LevelNumber == 1);
}

bool UUIManagerComponent::IsLevelCompleted(int32 LevelNumber) const
{
	if (!CachedSaveData)
	{
		CachedSaveData = FCYYSaveManager::LoadExisting();
	}
	return CachedSaveData ? CachedSaveData->IsLevelCompleted(LevelNumber) : false;
}

int32 UUIManagerComponent::GetMaxUnlockedLevel() const
{
	// 每次都重新加载，防止 GameMode::SaveGameProgress 存档后缓存未更新
	CachedSaveData = FCYYSaveManager::LoadExisting();
	return CachedSaveData ? CachedSaveData->MaxUnlockedLevel : 1;
}

FName UUIManagerComponent::LevelNumberToMapName(int32 LevelNumber) const
{
	// ── 关卡编号 → 地图名称映射 ──
	switch (LevelNumber)
	{
	case 1: return TEXT("/Game/11/Level1_Shelter");
	case 2: return TEXT("/Game/11/Level2_Factory");
	default: return NAME_None;
	}
}

void UUIManagerComponent::SaveCurrentGame()
{
	UCYYSaveGame* Save = FCYYSaveManager::LoadOrCreate();
	if (!Save) return;
	Save->bHasStartedGame = true;

	ACYYCharacterFather* PlayerChar = Cast<ACYYCharacterFather>(CombatCharacter);
	if (PlayerChar)
	{
		FCYYSaveManager::CollectFromPlayer(Save, PlayerChar);
	}
	FCYYSaveManager::SaveGame(Save);
	ShowPickupBanner(TEXT("游戏已保存"));
}

void UUIManagerComponent::PlayNextLevel()
{
	UCYYSaveGame* Save = FCYYSaveManager::LoadExisting();
	if (!Save || !Save->bHasStartedGame)
	{
		ShowPickupBanner(TEXT("没有可用存档"));
		return;
	}

	const FName MapName = LevelNumberToMapName(Save->CurrentLevel);
	if (MapName.IsNone())
	{
		ShowPickupBanner(TEXT("没有可进入的下一关"));
		return;
	}

	CachedSaveData = Save;
	StartGameLevelName = MapName;
	TransitionToState(EFlowState::Transition, TEXT("ResultMenu.NextLevel"));
	OpenLevelWithActionLock(MapName);
}
