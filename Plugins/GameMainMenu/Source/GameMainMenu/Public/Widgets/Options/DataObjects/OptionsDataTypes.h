// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "OptionsDataTypes.generated.h"

UENUM()
enum class EOptionValueType : uint8
{
	Bool,
	Scalar,
	String,
	Enum,
	QualityPreset,
	Culture,
	Resolution,
	KeyBinding
};

UENUM()
enum class EGameUserSettingFunction : uint8
{
	None,

	// ---- Gameplay ----
	GetDifficulty, SetDifficulty,
	GetShowHealthBar, SetShowHealthBar,
	GetShowDamageNumbers, SetShowDamageNumbers,
	GetHighlightInteraction, SetHighlightInteraction,
	GetShowSubtitles, SetShowSubtitles,
	GetAutoSave, SetAutoSave,
	GetCurrentCulture, SetCurrentCulture,

	// ---- Audio ----
	GetOverallVolume, SetOverallVolume,
	GetMusicVolume, SetMusicVolume,
	GetSFXVolume, SetSFXVolume,
	GetVoiceVolume, SetVoiceVolume,
	GetAmbientVolume, SetAmbientVolume,
	GetUIVolume, SetUIVolume,
	GetAllowBackgroundSound, SetAllowBackgroundSound,
	GetSubtitlesEnabled, SetSubtitlesEnabled,
	GetHRTFEnabled, SetHRTFEnabled,
	GetMonoOutput, SetMonoOutput,
	GetDynamicRangeCompression, SetDynamicRangeCompression,
	GetDialogueBoost, SetDialogueBoost,

	// ---- Video ----
	GetFullscreenMode, SetFullscreenMode,
	GetScreenResolution, SetScreenResolution,
	GetCurrentDisplayGamma, SetCurrentDisplayGamma,
	GetOverallScalabilityLevel, SetOverallScalabilityLevel,
	GetResolutionScaleNormalized, SetResolutionScaleNormalized,
	GetGlobalIlluminationQuality, SetGlobalIlluminationQuality,
	GetShadowQuality, SetShadowQuality,
	GetAntiAliasingQuality, SetAntiAliasingQuality,
	GetViewDistanceQuality, SetViewDistanceQuality,
	GetTextureQuality, SetTextureQuality,
	GetVisualEffectQuality, SetVisualEffectQuality,
	GetReflectionQuality, SetReflectionQuality,
	GetPostProcessingQuality, SetPostProcessingQuality,
	IsVSyncEnabled, SetVSyncEnabled,
	GetFrameRateLimit, SetFrameRateLimit,
};
