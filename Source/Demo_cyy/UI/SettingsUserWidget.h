#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsUserWidget.generated.h"

class UButton;
class UCheckBox;
class UComboBoxString;
class USlider;
class USoundBase;
class USoundClass;
class USoundMix;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsMenuBackClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsMenuClearSaveClickedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSettingsMenuApplyClickedSignature);

UCLASS()
class DEMO_CYY_API USettingsUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void RefreshControlsFromSavedSettings();

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FSettingsMenuBackClickedSignature OnBackClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FSettingsMenuClearSaveClickedSignature OnClearSaveClicked;

	UPROPERTY(BlueprintAssignable, Category="UI|Event")
	FSettingsMenuApplyClickedSignature OnApplyClicked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Audio")
	USoundMix* SettingsSoundMix = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Settings|Audio")
	USoundClass* MasterSoundClass = nullptr;

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void BackButtonClick();

	UFUNCTION()
	void ClearSaveButtonClick();

	UFUNCTION()
	void ApplyButtonClick();

	UFUNCTION()
	void MasterVolumeChanged(float Value);

	void InitializeGraphicsControls() const;
	void SaveAudioSettings(float MasterVolume) const;
	float LoadSavedMasterVolume() const;
	void ApplyGraphicsSettings() const;
	void ApplyVolume(USoundClass* SoundClass, float Value) const;

	bool bRefreshingControls = false;
};
