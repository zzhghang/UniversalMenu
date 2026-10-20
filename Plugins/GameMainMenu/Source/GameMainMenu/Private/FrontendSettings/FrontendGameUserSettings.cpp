// HangHang All Rights Reserved


#include "FrontendSettings/FrontendGameUserSettings.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "FrontendSettings/FrontendResolution.h"

#include "FrontendGameplayTags.h"
#include "FunctionLibrary/FrontendFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Internationalization/Culture.h"
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
	, AutoPickup(TEXT("Off"))
	, SprintMode(TEXT("Hold"))
	, CrouchMode(TEXT("Hold"))
	, bShowMinimap(true)
	, bShowAmmoCount(true)
	, CrosshairSize(1.f)
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
{
}

UFrontendGameUserSettings* UFrontendGameUserSettings::Get()
{
	if (GEngine) return CastChecked<UFrontendGameUserSettings>(GEngine->GetGameUserSettings());

	return nullptr;
}

void UFrontendGameUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
	RepairResolutionSettings();
}

void UFrontendGameUserSettings::ValidateSettings()
{
	RepairResolutionSettings();
	Super::ValidateSettings();
	RepairResolutionSettings();
}

void UFrontendGameUserSettings::RepairResolutionSettings()
{
	FIntPoint Size = GetScreenResolution();
	if (!FrontendResolution::IsUsable(Size))
	{
		Size = GetLastConfirmedScreenResolution();
		if (!FrontendResolution::IsUsable(Size)) Size = GetDesktopResolution();
		if (!FrontendResolution::IsUsable(Size)) Size = FIntPoint(1920, 1080);
		SetScreenResolution(Size);
	}
	if (!FrontendResolution::IsUsable(GetLastConfirmedScreenResolution()))
	{
		LastUserConfirmedResolutionSizeX = Size.X;
		LastUserConfirmedResolutionSizeY = Size.Y;
	}
	if (!FrontendResolution::IsUsable(FIntPoint(DesiredScreenWidth, DesiredScreenHeight)))
	{
		DesiredScreenWidth = Size.X;
		DesiredScreenHeight = Size.Y;
	}
	if (!FrontendResolution::IsUsable(FIntPoint(LastUserConfirmedDesiredScreenWidth, LastUserConfirmedDesiredScreenHeight)))
	{
		LastUserConfirmedDesiredScreenWidth = DesiredScreenWidth;
		LastUserConfirmedDesiredScreenHeight = DesiredScreenHeight;
	}
}

void UFrontendGameUserSettings::ApplyNonResolutionSettings()
{
	Super::ApplyNonResolutionSettings();

	ApplyDisplayGammaSetting();
	ApplyCurrentCultureSetting();

	ApplyVolumeSetting(FrontendGameplayTags::Frontend_Audio_Volume_Overall, OverallVolume);
	ApplyVolumeSetting(FrontendGameplayTags::Frontend_Audio_Volume_Music, MusicVolume);
	ApplyVolumeSetting(FrontendGameplayTags::Frontend_Audio_Volume_SFX, SFXVolume);
	ApplyVolumeSetting(FrontendGameplayTags::Frontend_Audio_Volume_Voice, VoiceVolume);
	ApplyVolumeSetting(FrontendGameplayTags::Frontend_Audio_Volume_Ambient, AmbientVolume);
	ApplyVolumeSetting(FrontendGameplayTags::Frontend_Audio_Volume_UI, UIVolume);
}

void UFrontendGameUserSettings::ApplyVolumeSetting(const FGameplayTag& VolumeTag, float Volume)
{
	UWorld* World = GetGameWorld();
	if (!World) return;

	USoundMix* SoundMix = UFrontendFunctionLibrary::GetDefaultSoundMix();
	if (!SoundMix) return;

	if (USoundClass* SoundClass = UFrontendFunctionLibrary::GetSoundClassByVolumeTag(VolumeTag))
	{
		UGameplayStatics::SetSoundMixClassOverride(World, SoundMix, SoundClass, Volume);
	}
}

void UFrontendGameUserSettings::ApplyDisplayGammaSetting()
{
	if (GEngine) GEngine->DisplayGamma = DisplayGamma;
}

FString UFrontendGameUserSettings::GetCurrentCulture() const
{
	// An unset language follows the operating system. Report the culture that would be applied rather
	// than an empty string, so the options screen selects the language the game actually runs in.
	return CurrentCulture.IsEmpty() ? ResolveSystemCulture() : CurrentCulture;
}

FString UFrontendGameUserSettings::ResolveSystemCulture() const
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	if (!FrontendDeveloperSettings || FrontendDeveloperSettings->SupportedCultures.IsEmpty()) return FString();

	// Culture selection needs the initialized internationalization system; without it, keep the
	// language the engine already picked.
	if (!FInternationalization::IsAvailable()) return FString();

	const TArray<FString>& SupportedCultures = FrontendDeveloperSettings->SupportedCultures;
	FInternationalization& Internationalization = FInternationalization::Get();

	// The engine takes the language from the OS language ("zh") and the locale from the OS locale
	// ("zh-CN"). The locale carries the region that decides the script a shipped culture uses
	// ("zh-Hans"), so it is the better source; GetPrioritizedCultureNames expands a name into the
	// cultures to try, most specific first.
	TArray<FString> Candidates = Internationalization.GetPrioritizedCultureNames(Internationalization.GetCurrentLocale()->GetName());
	Candidates.Append(Internationalization.GetPrioritizedCultureNames(Internationalization.GetCurrentLanguage()->GetName()));

	for (const FString& Candidate : Candidates)
	{
		if (const FString* Match = SupportedCultures.FindByPredicate([&Candidate](const FString& SupportedCulture)
			{
				return SupportedCulture.Equals(Candidate, ESearchCase::IgnoreCase);
			}))
		{
			return *Match;
		}
	}

	// The OS language is not shipped, so use the primary language, which the config lists first.
	return SupportedCultures[0];
}

void UFrontendGameUserSettings::ApplyCurrentCultureSetting()
{
	const FString CultureToApply = GetCurrentCulture();
	if (CultureToApply.IsEmpty()) return;

	// Only switch when the runtime language actually differs: settings are re-applied on every
	// immediate change, and switching cultures refreshes all localized text.
	FInternationalization& Internationalization = FInternationalization::Get();
	if (Internationalization.GetCurrentCulture()->GetName() == CultureToApply) return;

	Internationalization.SetCurrentCulture(CultureToApply);
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

void UFrontendGameUserSettings::SetAutoPickup(const FString& InValue)
{
	AutoPickup = InValue;
}

void UFrontendGameUserSettings::SetSprintMode(const FString& InValue)
{
	SprintMode = InValue;
}

void UFrontendGameUserSettings::SetCrouchMode(const FString& InValue)
{
	CrouchMode = InValue;
}

void UFrontendGameUserSettings::SetShowMinimap(bool InValue)
{
	bShowMinimap = InValue;
}

void UFrontendGameUserSettings::SetShowAmmoCount(bool InValue)
{
	bShowAmmoCount = InValue;
}

void UFrontendGameUserSettings::SetCrosshairSize(float InValue)
{
	CrosshairSize = FMath::IsFinite(InValue) ? FMath::Clamp(InValue, 0.5f, 2.f) : 1.f;
}
