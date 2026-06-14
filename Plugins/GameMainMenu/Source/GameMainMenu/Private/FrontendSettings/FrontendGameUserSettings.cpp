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

void UFrontendGameUserSettings::SetOverallVolume(const float InValue)
{
	OverallVolume = FMath::Clamp(InValue, 0.0f, 2.0f);

	UWorld* World = GetGameWorld();
	if (!World) return;

	if (USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix())
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FrontendGameplayTags::Frontend_Audio_Volume_Overall))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, OverallVolume);
		}
	}

}

void UFrontendGameUserSettings::SetMusicVolume(const float InValue)
{
	MusicVolume = FMath::Clamp(InValue, 0.0f, 2.0f);

	UWorld* World = GetGameWorld();
	if (!World) return;

	if (USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix())
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FrontendGameplayTags::Frontend_Audio_Volume_Music))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, MusicVolume);
		}
	}

}

void UFrontendGameUserSettings::SetSFXVolume(const float InValue)
{
	SFXVolume = FMath::Clamp(InValue, 0.0f, 2.0f);

	UWorld* World = GetGameWorld();
	if (!World) return;

	if (USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix())
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FrontendGameplayTags::Frontend_Audio_Volume_SFX))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, SFXVolume);
		}
	}

}

void UFrontendGameUserSettings::SetVoiceVolume(const float InValue)
{
	VoiceVolume = FMath::Clamp(InValue, 0.0f, 2.0f);

	UWorld* World = GetGameWorld();
	if (!World) return;

	if (USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix())
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FrontendGameplayTags::Frontend_Audio_Volume_Voice))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, VoiceVolume);
		}
	}

}

void UFrontendGameUserSettings::SetAmbientVolume(const float InValue)
{
	AmbientVolume = FMath::Clamp(InValue, 0.0f, 2.0f);

	UWorld* World = GetGameWorld();
	if (!World) return;

	if (USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix())
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FrontendGameplayTags::Frontend_Audio_Volume_Ambient))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, AmbientVolume);
		}
	}

}

void UFrontendGameUserSettings::SetUIVolume(const float InValue)
{
	UIVolume = FMath::Clamp(InValue, 0.0f, 2.0f);

	UWorld* World = GetGameWorld();
	if (!World) return;

	if (USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix())
	{
		if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FrontendGameplayTags::Frontend_Audio_Volume_UI))
		{
			UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, UIVolume);
		}
	}

}

void UFrontendGameUserSettings::SetCurrentCulture(const FString& InCulture)
{
	CurrentCulture = InCulture;

	FInternationalization::Get().SetCurrentCulture(InCulture);

	SaveSettings();
}

void UFrontendGameUserSettings::SetAllowBackgroundSound(const bool InValue)
{
	bAllowBackgroundSound = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetSubtitlesEnabled(const bool InValue)
{
	bSubtitlesEnabled = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetHRTFEnabled(const bool InValue)
{
	bHRTFEnabled = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetMonoOutput(const bool InValue)
{
	bMonoOutput = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetDynamicRangeCompression(const bool InValue)
{
	bDynamicRangeCompression = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetDialogueBoost(const bool InValue)
{
	bDialogueBoost = InValue;
	SaveSettings();
}

float UFrontendGameUserSettings::GetCurrentDisplayGamma() const
{
	if (GEngine) return GEngine->GetDisplayGamma();

	return 2.2f;
}

void UFrontendGameUserSettings::SetCurrentDisplayGamma(const float InValue)
{
	if (GEngine) GEngine->DisplayGamma = InValue;

	SaveSettings();
}

void UFrontendGameUserSettings::SetShowHealthBar(const bool InValue)
{
	bShowHealthBar = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetShowDamageNumbers(const bool InValue)
{
	bShowDamageNumbers = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetHighlightInteraction(const bool InValue)
{
	bHighlightInteraction = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetShowSubtitles(const bool InValue)
{
	bShowSubtitles = InValue;
	SaveSettings();
}

void UFrontendGameUserSettings::SetAutoSave(const bool InValue)
{
	bAutoSave = InValue;
	SaveSettings();
}
