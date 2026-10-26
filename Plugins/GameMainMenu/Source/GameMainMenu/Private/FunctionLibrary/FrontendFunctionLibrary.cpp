// HangHang All Rights Reserved


#include "FunctionLibrary/FrontendFunctionLibrary.h"

#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "GeneralProjectSettings.h"
#include "LogGameMainMenu.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundMix.h"

TSoftClassPtr<UW_ActivatableBase> UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag)
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	ensureMsgf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag), TEXT("Can not find widget under the tag %s"), *InWidgetTag.ToString());
	
	return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);
}

TSoftObjectPtr<UTexture2D> UFrontendFunctionLibrary::GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "Frontend.Image")) FGameplayTag InImageTag)
{
	if (!InImageTag.IsValid()) return nullptr;

	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();
	if (!FrontendDeveloperSettings) return nullptr;

	if (const TSoftObjectPtr<UTexture2D>* FoundImage = FrontendDeveloperSettings->OptionsScreenSoftImageMap.Find(InImageTag))
	{
		return *FoundImage;
	}

	// Description images are optional; a missing mapping is a configuration warning, not a fatal error.
	UE_LOG(LogGameMainMenu, Warning, TEXT("Can not find Image under the tag %s in OptionsScreenSoftImageMap"), *InImageTag.ToString());
	return nullptr;
}

USoundMix* UFrontendFunctionLibrary::GetDefaultSoundMix()
{
	UFrontendDeveloperSettings* FrontendDeveloperSettings = GetMutableDefault<UFrontendDeveloperSettings>();

	if (FrontendDeveloperSettings->DefaultSoundMix.IsNull()) return nullptr;

	// Volume changes are applied on every slider event, so keep the resolved asset instead of
	// re-resolving the soft reference each time (and only re-resolve if it was re-pointed).
	USoundMix* CachedSoundMix = FrontendDeveloperSettings->CachedDefaultSoundMix;
	if (!CachedSoundMix || CachedSoundMix != FrontendDeveloperSettings->DefaultSoundMix.Get())
	{
		CachedSoundMix = FrontendDeveloperSettings->DefaultSoundMix.LoadSynchronous();
		FrontendDeveloperSettings->CachedDefaultSoundMix = CachedSoundMix;
	}

	return CachedSoundMix;
}

USoundClass* UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FGameplayTag InVolumeTag)
{
	UFrontendDeveloperSettings* FrontendDeveloperSettings = GetMutableDefault<UFrontendDeveloperSettings>();

	const TSoftObjectPtr<USoundClass>* FoundSoundClass = FrontendDeveloperSettings->AudioVolumeSoundClassMap.Find(InVolumeTag);
	if (!FoundSoundClass || FoundSoundClass->IsNull()) return nullptr;

	TObjectPtr<USoundClass>& CachedSoundClass = FrontendDeveloperSettings->CachedAudioVolumeSoundClasses.FindOrAdd(InVolumeTag);
	if (!CachedSoundClass || CachedSoundClass != FoundSoundClass->Get())
	{
		CachedSoundClass = FoundSoundClass->LoadSynchronous();
	}

	return CachedSoundClass;
}

FString UFrontendFunctionLibrary::GetProjectVersion()
{
	return GetDefault<UGeneralProjectSettings>()->ProjectVersion;
}
