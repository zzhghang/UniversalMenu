// HangHang All Rights Reserved


#include "FunctionLibrary/FrontendFunctionLibrary.h"

#include "FrontendSettings/FrontendDeveloperSettings.h"
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
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	ensureMsgf(FrontendDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("Can not find Image under the tag %s"), *InImageTag.ToString());

	return FrontendDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);
}

USoundMix* UFrontendFunctionLibrary::GetDefaultSoundMix()
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	if (FrontendDeveloperSettings->DefaultSoundMix.IsNull()) return nullptr;

	return FrontendDeveloperSettings->DefaultSoundMix.LoadSynchronous();
}

USoundClass* UFrontendFunctionLibrary::GetSoundClassByVolumeTag(FGameplayTag InVolumeTag)
{
	const UFrontendDeveloperSettings* FrontendDeveloperSettings = GetDefault<UFrontendDeveloperSettings>();

	const TSoftObjectPtr<USoundClass>* FoundSoundClass = FrontendDeveloperSettings->AudioVolumeSoundClassMap.Find(InVolumeTag);
	if (!FoundSoundClass || FoundSoundClass->IsNull()) return nullptr;

	return FoundSoundClass->LoadSynchronous();
}
