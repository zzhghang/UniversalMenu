// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "FrontendGameUserSettings.generated.h"

/**
 * Custom game user settings for the frontend menu.
 *
 * Extends UGameUserSettings with gameplay, audio, video, and localization settings.
 * All UFUNCTION getters/setters are callable via PropertyPathHelpers string reflection
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

	// ---- Gameplay ----
	/**@{*/
	UFUNCTION()
	FString GetDifficulty() const {return Difficulty;}

	UFUNCTION()
	void SetDifficulty(const FString& InStringValue){ Difficulty = InStringValue; SaveSettings();}

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
	/**@}*/

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

	UFUNCTION()
	FString GetCurrentCulture() const { return CurrentCulture; }

	UFUNCTION()
	void SetCurrentCulture(const FString& InCulture);
	/**@}*/


private:

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


	// ---- Language ----

	UPROPERTY(Config)
	FString CurrentCulture;
};
