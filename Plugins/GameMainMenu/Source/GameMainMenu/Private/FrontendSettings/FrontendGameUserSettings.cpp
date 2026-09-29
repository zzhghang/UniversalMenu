// HangHang All Rights Reserved


#include "FrontendSettings/FrontendGameUserSettings.h"

#include "FrontendGameplayTags.h"
#include "FunctionLibrary/FrontendFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Internationalization/Internationalization.h"

namespace
{
	UWorld* GetGameWorld()
	{
		if (!GEngine) return nullptr;

		for (const FWorldContext& Context : GEngine->GetWorldContexts())
		{
			if (Context.WorldType == EWorldType::Game || Context.WorldType == EWorldType::PIE)
			{
				return Context.World();
			}
		}

		return nullptr;
	}
}

UFrontendGameUserSettings::UFrontendGameUserSettings()
	: Difficulty(TEXT("Normal"))
	, bShowHealthBar(true)
	, bShowDamageNumbers(true)
	, bHighlightInteraction(true)
	, bShowSubtitles(true)
	, bAutoSave(true)
	, OverallVolume(1.f)
	, MusicVolume(1.f)
	, SFXVolume(1.f)
	, VoiceVolume(1.f)
	, AmbientVolume(1.f)
	, UIVolume(1.f)
	, bAllowBackgroundSound(false)
	, bSubtitlesEnabled(true)
	, bHRTFEnabled(true)
	, bMonoOutput(false)
	, bDynamicRangeCompression(false)
	, bDialogueBoost(true)
	, CurrentCulture(TEXT("en"))
{
}

UFrontendGameUserSettings* UFrontendGameUserSettings::Get()
{
	if (GEngine) return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());

	return nullptr;
}

void UFrontendGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();
	if (GEngine) GEngine->DisplayGamma = DisplayGamma;
	FInternationalization::Get().SetCurrentCulture(CurrentCulture);

	UWorld* World = GetGameWorld();
	if (!World) return;
	USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix();
	if (!SoundMix) return;

	const auto ApplyVolume = [World, SoundMix](const FGameplayTag& Tag, float Volume)
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(Tag))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, Volume);
		}
	};
	ApplyVolume(FrontendGameplayTags::Frontend_Audio_Volume_Overall, OverallVolume);
	ApplyVolume(FrontendGameplayTags::Frontend_Audio_Volume_Music, MusicVolume);
	ApplyVolume(FrontendGameplayTags::Frontend_Audio_Volume_SFX, SFXVolume);
	ApplyVolume(FrontendGameplayTags::Frontend_Audio_Volume_Voice, VoiceVolume);
	ApplyVolume(FrontendGameplayTags::Frontend_Audio_Volume_Ambient, AmbientVolume);
	ApplyVolume(FrontendGameplayTags::Frontend_Audio_Volume_UI, UIVolume);
}

void UFrontendGameUserSettings::SetOverallVolume(const float InValue)
{
	OverallVolume = FMath::Clamp(InValue, 0.0f, 2.0f);
}

void UFrontendGameUserSettings::SetMusicVolume(const float InValue)
{
	MusicVolume = FMath::Clamp(InValue, 0.0f, 2.0f);
}

void UFrontendGameUserSettings::SetSFXVolume(const float InValue)
{
	SFXVolume = FMath::Clamp(InValue, 0.0f, 2.0f);
}

void UFrontendGameUserSettings::SetVoiceVolume(const float InValue)
{
	VoiceVolume = FMath::Clamp(InValue, 0.0f, 2.0f);
}

void UFrontendGameUserSettings::SetAmbientVolume(const float InValue)
{
	AmbientVolume = FMath::Clamp(InValue, 0.0f, 2.0f);
}

void UFrontendGameUserSettings::SetUIVolume(const float InValue)
{
	UIVolume = FMath::Clamp(InValue, 0.0f, 2.0f);
}

void UFrontendGameUserSettings::SetCurrentCulture(const FString& InCulture)
{
	CurrentCulture = InCulture;
}

void UFrontendGameUserSettings::SetAllowBackgroundSound(const bool InValue)
{
	bAllowBackgroundSound = InValue;
}

void UFrontendGameUserSettings::SetSubtitlesEnabled(const bool InValue)
{
	bSubtitlesEnabled = InValue;
}

void UFrontendGameUserSettings::SetHRTFEnabled(const bool InValue)
{
	bHRTFEnabled = InValue;
}

void UFrontendGameUserSettings::SetMonoOutput(const bool InValue)
{
	bMonoOutput = InValue;
}

void UFrontendGameUserSettings::SetDynamicRangeCompression(const bool InValue)
{
	bDynamicRangeCompression = InValue;
}

void UFrontendGameUserSettings::SetDialogueBoost(const bool InValue)
{
	bDialogueBoost = InValue;
}

float UFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	return DisplayGamma;
}

void UFrontendGameUserSettings::SetCurrentDisplayGamma(const float InValue)
{
	DisplayGamma = InValue;
}

void UFrontendGameUserSettings::SetShowHealthBar(const bool InValue)
{
	bShowHealthBar = InValue;
}

void UFrontendGameUserSettings::SetShowDamageNumbers(const bool InValue)
{
	bShowDamageNumbers = InValue;
}

void UFrontendGameUserSettings::SetHighlightInteraction(const bool InValue)
{
	bHighlightInteraction = InValue;
}

void UFrontendGameUserSettings::SetShowSubtitles(const bool InValue)
{
	bShowSubtitles = InValue;
}

void UFrontendGameUserSettings::SetAutoSave(const bool InValue)
{
	bAutoSave = InValue;
}
