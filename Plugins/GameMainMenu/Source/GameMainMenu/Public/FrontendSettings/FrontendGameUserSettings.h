// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "GameplayTagContainer.h"
#include "FrontendGameUserSettings.generated.h"

/**
 * Custom game user settings for the frontend menu.
 *
 * Extends UGameUserSettings with gameplay, audio, video, and localization settings.
 * All UFUNCTION getters/setters are callable via TFunction dispatch
 * through FOptionsDataInteractionHelper, enabling data-driven options configuration.
 */
UCLASS()
class GAMEMAINMENU_API UFrontendGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	UFrontendGameUserSettings();

	/** Returns the singleton instance via GEngine->GetGameUserSettings(). */
	static UFrontendGameUserSettings* Get();

	/** Setters only update stored values; runtime effects are applied together. */
	virtual void ApplyNonResolutionSettings() override;
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ValidateSettings() override;
	/** Repairs corrupt dimensions without applying or saving settings. */
	void RepairResolutionSettings();

	/**
	 * Applies the runtime effect of a single setting.
	 *
	 * Continuous edits (sliders) report one change per input event, so they must not run the full
	 * ApplyNonResolutionSettings, which re-applies every quality cvar, the culture and all sound mixes.
	 */
	void ApplyVolumeSetting(const FGameplayTag& VolumeTag, float Volume);
	void ApplyDisplayGammaSetting();
	/** Switches the language, resolving an unset choice from the operating system first. */
	void ApplyCurrentCultureSetting();

	// ---- Gameplay ----
	/**@{*/
	UFUNCTION()
	FString GetDifficulty() const {return Difficulty;}

	UFUNCTION()
	void SetDifficulty(const FString& InStringValue){ Difficulty = InStringValue; }

	UFUNCTION()
	bool GetShowHealthBar() const {return bShowHealthBar;}

	UFUNCTION()
	void SetShowHealthBar(const bool InValue);

	UFUNCTION()
	bool GetShowDamageNumbers() const {return bShowDamageNumbers;}

	UFUNCTION()
	void SetShowDamageNumbers(const bool InValue);

	UFUNCTION()
	bool GetHighlightInteraction() const {return bHighlightInteraction;}

	UFUNCTION()
	void SetHighlightInteraction(const bool InValue);

	UFUNCTION()
	bool GetShowSubtitles() const {return bShowSubtitles;}

	UFUNCTION()
	void SetShowSubtitles(const bool InValue);

	UFUNCTION()
	bool GetAutoSave() const {return bAutoSave;}

	UFUNCTION()
	void SetAutoSave(const bool InValue);

	/** Automatic pickup mode: Off, MaterialsOnly, or AllItems. */
	UFUNCTION(BlueprintPure, Category = "Settings|Gameplay")
	FString GetAutoPickup() const { return AutoPickup; }

	/** Updates the stored mode; Options commits its draft on Back. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetAutoPickup(const FString& InValue);

	/** Sprint input mode: Hold or Toggle. */
	UFUNCTION(BlueprintPure, Category = "Settings|Gameplay")
	FString GetSprintMode() const { return SprintMode; }

	/** Updates the stored mode; Options commits its draft on Back. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetSprintMode(const FString& InValue);

	/** Crouch input mode: Hold or Toggle. */
	UFUNCTION(BlueprintPure, Category = "Settings|Gameplay")
	FString GetCrouchMode() const { return CrouchMode; }

	/** Updates the stored mode; Options commits its draft on Back. */
	UFUNCTION(BlueprintCallable, Category = "Settings|Gameplay")
	void SetCrouchMode(const FString& InValue);
	/**@}*/

	// ---- HUD ----
	UFUNCTION(BlueprintPure, Category = "Settings|HUD")
	bool GetShowMinimap() const { return bShowMinimap; }

	UFUNCTION(BlueprintCallable, Category = "Settings|HUD")
	void SetShowMinimap(bool InValue);

	UFUNCTION(BlueprintPure, Category = "Settings|HUD")
	bool GetShowAmmoCount() const { return bShowAmmoCount; }

	UFUNCTION(BlueprintCallable, Category = "Settings|HUD")
	void SetShowAmmoCount(bool InValue);

	/** Crosshair scale multiplier; 1.0 is the original size. */
	UFUNCTION(BlueprintPure, Category = "Settings|HUD")
	float GetCrosshairSize() const { return CrosshairSize; }

	UFUNCTION(BlueprintCallable, Category = "Settings|HUD")
	void SetCrosshairSize(float InValue);

	// ---- Audio ----
	/**@{*/
	
	UFUNCTION()
	float GetOverallVolume() const {return OverallVolume;}

	UFUNCTION()
	void SetOverallVolume(const float InValue);

	UFUNCTION()
	float GetMusicVolume() const {return MusicVolume;}

	UFUNCTION()
	void SetMusicVolume(const float InValue);

	UFUNCTION()
	float GetSFXVolume() const {return SFXVolume;}

	UFUNCTION()
	void SetSFXVolume(const float InValue);

	UFUNCTION()
	float GetVoiceVolume() const {return VoiceVolume;}

	UFUNCTION()
	void SetVoiceVolume(const float InValue);

	UFUNCTION()
	float GetAmbientVolume() const {return AmbientVolume;}

	UFUNCTION()
	void SetAmbientVolume(const float InValue);

	UFUNCTION()
	float GetUIVolume() const {return UIVolume;}

	UFUNCTION()
	void SetUIVolume(const float InValue);
	
	UFUNCTION()
	bool GetAllowBackgroundSound() const {return bAllowBackgroundSound;}

	UFUNCTION()
	void SetAllowBackgroundSound(const bool InValue);

	UFUNCTION()
	bool GetSubtitlesEnabled() const {return bSubtitlesEnabled;}

	UFUNCTION()
	void SetSubtitlesEnabled(const bool InValue);

	UFUNCTION()
	bool GetHRTFEnabled() const {return bHRTFEnabled;}

	UFUNCTION()
	void SetHRTFEnabled(const bool InValue);

	UFUNCTION()
	bool GetMonoOutput() const {return bMonoOutput;}

	UFUNCTION()
	void SetMonoOutput(const bool InValue);

	UFUNCTION()
	bool GetDynamicRangeCompression() const {return bDynamicRangeCompression;}

	UFUNCTION()
	void SetDynamicRangeCompression(const bool InValue);

	UFUNCTION()
	bool GetDialogueBoost() const {return bDialogueBoost;}

	UFUNCTION()
	void SetDialogueBoost(const bool InValue);
	/**@}*/


	// ---- Video ----
	/**@{*/
	
	UFUNCTION()
	float GetCurrentDisplayGamma() const;

	UFUNCTION()
	void SetCurrentDisplayGamma(const float InValue);
	/**@}*/

	// ---- Language ----
	/**@{*/

	/**
	 * Language to run in: the stored choice, or — while unset — the supported culture matching the
	 * operating system, so the options screen shows the language the game actually runs in.
	 */
	UFUNCTION()
	FString GetCurrentCulture() const;

	UFUNCTION()
	void SetCurrentCulture(const FString& InCulture);
	/**@}*/


private:

	/** Best supported culture for the operating system, or empty when no cultures are configured. */
	FString ResolveSystemCulture() const;

	// ---- Gameplay ----
	UPROPERTY(Config)
	FString Difficulty;

	UPROPERTY(Config)
	bool bShowHealthBar;

	UPROPERTY(Config)
	bool bShowDamageNumbers;

	UPROPERTY(Config)
	bool bHighlightInteraction;

	UPROPERTY(Config)
	bool bShowSubtitles;

	UPROPERTY(Config)
	bool bAutoSave;

	UPROPERTY(Config)
	FString AutoPickup;

	UPROPERTY(Config)
	FString SprintMode;

	UPROPERTY(Config)
	FString CrouchMode;

	// ---- HUD ----
	UPROPERTY(Config)
	bool bShowMinimap;

	UPROPERTY(Config)
	bool bShowAmmoCount;

	UPROPERTY(Config)
	float CrosshairSize;

	// ---- Audio ----

	UPROPERTY(Config)
	float OverallVolume;

	UPROPERTY(Config)
	float MusicVolume;

	UPROPERTY(Config)
	float SFXVolume;

	UPROPERTY(Config)
	float VoiceVolume;

	UPROPERTY(Config)
	float AmbientVolume;

	UPROPERTY(Config)
	float UIVolume;

	UPROPERTY(Config)
	bool bAllowBackgroundSound;

	UPROPERTY(Config)
	bool bSubtitlesEnabled;

	UPROPERTY(Config)
	bool bHRTFEnabled;

	UPROPERTY(Config)
	bool bMonoOutput;

	UPROPERTY(Config)
	bool bDynamicRangeCompression;

	UPROPERTY(Config)
	bool bDialogueBoost;

	// ---- Video ----

	// (Video properties are inherited from UGameUserSettings)
	UPROPERTY(Config)
	float DisplayGamma = 2.2f;


	// ---- Language ----

	UPROPERTY(Config)
	FString CurrentCulture;
};
