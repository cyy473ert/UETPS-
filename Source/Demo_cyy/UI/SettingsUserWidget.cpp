#include "SettingsUserWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/Slider.h"
#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Save/CYYSettingsSaveGame.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

namespace
{
template <typename T>
T* FindWidgetByNames(UUserWidget* Widget, const TArray<FName>& Names)
{
	if (!Widget)
	{
		return nullptr;
	}

	for (const FName& Name : Names)
	{
		if (T* FoundWidget = Cast<T>(Widget->GetWidgetFromName(Name)))
		{
			return FoundWidget;
		}
	}
	return nullptr;
}

void SelectComboOption(UComboBoxString* ComboBox, const FString& Option)
{
	if (!ComboBox)
	{
		return;
	}

	if (ComboBox->FindOptionIndex(Option) == INDEX_NONE)
	{
		ComboBox->AddOption(Option);
	}
	ComboBox->SetSelectedOption(Option);
}
}

void USettingsUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UButton* BackButton = FindWidgetByNames<UButton>(this, {TEXT("BackBtn"), TEXT("ReturnBtn"), TEXT("Back")}))
	{
		BackButton->OnClicked.AddUniqueDynamic(this, &USettingsUserWidget::BackButtonClick);
	}

	if (UButton* ClearSaveButton = FindWidgetByNames<UButton>(this, {TEXT("ClearSaveBtn")}))
	{
		ClearSaveButton->OnClicked.AddUniqueDynamic(this, &USettingsUserWidget::ClearSaveButtonClick);
	}

	if (UButton* ApplyButton = FindWidgetByNames<UButton>(this, {TEXT("ApplyBtn"), TEXT("ConfirmBtn")}))
	{
		ApplyButton->OnClicked.AddUniqueDynamic(this, &USettingsUserWidget::ApplyButtonClick);
	}

	if (USlider* MasterSlider = FindWidgetByNames<USlider>(this, {TEXT("MasterVolumeSlider"), TEXT("MasterSlider")}))
	{
		MasterSlider->OnValueChanged.AddUniqueDynamic(this, &USettingsUserWidget::MasterVolumeChanged);
	}

	RefreshControlsFromSavedSettings();
}

void USettingsUserWidget::BackButtonClick()
{
	OnBackClicked.Broadcast();
}

void USettingsUserWidget::ClearSaveButtonClick()
{
	OnClearSaveClicked.Broadcast();
}

void USettingsUserWidget::ApplyButtonClick()
{
	ApplyGraphicsSettings();
	if (USlider* MasterSlider = FindWidgetByNames<USlider>(this, {TEXT("MasterVolumeSlider"), TEXT("MasterSlider")}))
	{
		const float Volume = FMath::Clamp(MasterSlider->GetValue(), 0.0f, 1.0f);
		ApplyVolume(MasterSoundClass, Volume);
		SaveAudioSettings(Volume);
	}
	OnApplyClicked.Broadcast();
}

void USettingsUserWidget::MasterVolumeChanged(float Value)
{
	if (bRefreshingControls)
	{
		return;
	}

	const float Volume = FMath::Clamp(Value, 0.0f, 1.0f);
	ApplyVolume(MasterSoundClass, Volume);
	SaveAudioSettings(Volume);
}

void USettingsUserWidget::RefreshControlsFromSavedSettings()
{
	InitializeGraphicsControls();

	UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (UserSettings)
	{
		UserSettings->LoadSettings(false);

		if (UCheckBox* FullscreenCheckBox = FindWidgetByNames<UCheckBox>(this, {TEXT("FullscreenCheckBox"), TEXT("FullscreenToggle")}))
		{
			FullscreenCheckBox->SetIsChecked(UserSettings->GetFullscreenMode() != EWindowMode::Windowed);
		}

		if (UComboBoxString* ResolutionCombo = FindWidgetByNames<UComboBoxString>(this, {TEXT("ResolutionCombo"), TEXT("ResolutionBox")}))
		{
			const FIntPoint Resolution = UserSettings->GetScreenResolution();
			SelectComboOption(ResolutionCombo, FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y));
		}

		if (UComboBoxString* QualityCombo = FindWidgetByNames<UComboBoxString>(this, {TEXT("QualityCombo"), TEXT("QualityBox")}))
		{
			const int32 Quality = UserSettings->GetOverallScalabilityLevel();
			const FString QualityText = Quality <= 0 ? TEXT("低") : Quality == 1 ? TEXT("中") : Quality >= 3 ? TEXT("极高") : TEXT("高");
			SelectComboOption(QualityCombo, QualityText);
		}
	}

	const float SavedMasterVolume = LoadSavedMasterVolume();
	if (USlider* MasterSlider = FindWidgetByNames<USlider>(this, {TEXT("MasterVolumeSlider"), TEXT("MasterSlider")}))
	{
		bRefreshingControls = true;
		MasterSlider->SetValue(SavedMasterVolume);
		bRefreshingControls = false;
	}
	ApplyVolume(MasterSoundClass, SavedMasterVolume);
}

void USettingsUserWidget::InitializeGraphicsControls() const
{
	if (UComboBoxString* ResolutionCombo = FindWidgetByNames<UComboBoxString>(const_cast<USettingsUserWidget*>(this), {TEXT("ResolutionCombo"), TEXT("ResolutionBox")}))
	{
		if (ResolutionCombo->GetOptionCount() == 0)
		{
			ResolutionCombo->AddOption(TEXT("1280x720"));
			ResolutionCombo->AddOption(TEXT("1600x900"));
			ResolutionCombo->AddOption(TEXT("1920x1080"));
			ResolutionCombo->AddOption(TEXT("2560x1440"));
		}
		if (ResolutionCombo->GetSelectedOption().IsEmpty())
		{
			ResolutionCombo->SetSelectedOption(TEXT("1920x1080"));
		}
	}

	if (UComboBoxString* QualityCombo = FindWidgetByNames<UComboBoxString>(const_cast<USettingsUserWidget*>(this), {TEXT("QualityCombo"), TEXT("QualityBox")}))
	{
		if (QualityCombo->GetOptionCount() == 0)
		{
			QualityCombo->AddOption(TEXT("低"));
			QualityCombo->AddOption(TEXT("中"));
			QualityCombo->AddOption(TEXT("高"));
			QualityCombo->AddOption(TEXT("极高"));
		}
		if (QualityCombo->GetSelectedOption().IsEmpty())
		{
			QualityCombo->SetSelectedOption(TEXT("高"));
		}
	}
}

void USettingsUserWidget::SaveAudioSettings(float MasterVolume) const
{
	UCYYSettingsSaveGame* Save = Cast<UCYYSettingsSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UCYYSettingsSaveGame::SaveSlotName, 0));
	if (!Save)
	{
		Save = NewObject<UCYYSettingsSaveGame>();
	}

	if (Save)
	{
		Save->MasterVolume = FMath::Clamp(MasterVolume, 0.0f, 1.0f);
		UGameplayStatics::SaveGameToSlot(Save, UCYYSettingsSaveGame::SaveSlotName, 0);
	}
}

float USettingsUserWidget::LoadSavedMasterVolume() const
{
	const UCYYSettingsSaveGame* Save = Cast<UCYYSettingsSaveGame>(
		UGameplayStatics::LoadGameFromSlot(UCYYSettingsSaveGame::SaveSlotName, 0));
	if (Save)
	{
		return FMath::Clamp(Save->MasterVolume, 0.0f, 1.0f);
	}

	return 1.0f;
}

void USettingsUserWidget::ApplyGraphicsSettings() const
{
	UGameUserSettings* UserSettings = GEngine ? GEngine->GetGameUserSettings() : nullptr;
	if (!UserSettings)
	{
		return;
	}

	if (UCheckBox* FullscreenCheckBox = FindWidgetByNames<UCheckBox>(const_cast<USettingsUserWidget*>(this), {TEXT("FullscreenCheckBox"), TEXT("FullscreenToggle")}))
	{
		UserSettings->SetFullscreenMode(FullscreenCheckBox->IsChecked()
			? EWindowMode::Fullscreen
			: EWindowMode::Windowed);
	}

	if (UComboBoxString* ResolutionCombo = FindWidgetByNames<UComboBoxString>(const_cast<USettingsUserWidget*>(this), {TEXT("ResolutionCombo"), TEXT("ResolutionBox")}))
	{
		const FString SelectedResolution = ResolutionCombo->GetSelectedOption();
		FString WidthText;
		FString HeightText;
		if (SelectedResolution.Split(TEXT("x"), &WidthText, &HeightText))
		{
			UserSettings->SetScreenResolution(FIntPoint(FCString::Atoi(*WidthText), FCString::Atoi(*HeightText)));
		}
	}

	if (UComboBoxString* QualityCombo = FindWidgetByNames<UComboBoxString>(const_cast<USettingsUserWidget*>(this), {TEXT("QualityCombo"), TEXT("QualityBox")}))
	{
		const FString SelectedQuality = QualityCombo->GetSelectedOption();
		int32 Quality = 2;
		if (SelectedQuality.Equals(TEXT("低"), ESearchCase::IgnoreCase)
			|| SelectedQuality.Equals(TEXT("Low"), ESearchCase::IgnoreCase))
		{
			Quality = 0;
		}
		else if (SelectedQuality.Equals(TEXT("中"), ESearchCase::IgnoreCase)
			|| SelectedQuality.Equals(TEXT("Medium"), ESearchCase::IgnoreCase))
		{
			Quality = 1;
		}
		else if (SelectedQuality.Equals(TEXT("极高"), ESearchCase::IgnoreCase)
			|| SelectedQuality.Equals(TEXT("Epic"), ESearchCase::IgnoreCase))
		{
			Quality = 3;
		}
		UserSettings->SetOverallScalabilityLevel(Quality);
	}

	UserSettings->ApplySettings(false);
	UserSettings->SaveSettings();
}

void USettingsUserWidget::ApplyVolume(USoundClass* SoundClass, float Value) const
{
	if (!SettingsSoundMix || !SoundClass)
	{
		return;
	}

	UGameplayStatics::SetSoundMixClassOverride(this, SettingsSoundMix, SoundClass, FMath::Clamp(Value, 0.0f, 1.0f), 1.0f, 0.0f, true);
	UGameplayStatics::PushSoundMixModifier(this, SettingsSoundMix);
}
