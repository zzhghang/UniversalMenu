// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "FrontendDeveloperSettings.generated.h"

class UW_ActivatableBase;
class USoundClass;
class USoundMix;

/**
 * Project-wide frontend settings accessible via Project Settings > Frontend UI Settings.
 *
 * Maps GameplayTags to widget classes, images, audio resources, and localization cultures.
 * Used by UFrontendFunctionLibrary to resolve tag-based resource lookups at runtime.
 */
UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Frontend UI Settings"))
class GAMEMAINMENU_API UFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	/** Maps widget GameplayTags to soft widget class references. */
	UPROPERTY(Config, EditAnywhere, Category = "Widget Ref", meta = (ForceInlineRow, Categories = "Frontend.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UW_ActivatableBase>> FrontendWidgetMap;

	/** Maps image GameplayTags to soft texture references (e.g. option description images). */
	UPROPERTY(Config, EditAnywhere, Category = "Options Image Ref", meta = (ForceInlineRow, Categories = "Frontend.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;

	/** Maps audio volume GameplayTags to sound classes for per-channel volume control. */
	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = (ForceInlineRow, Categories = "Frontend.Audio.Volume"))
	TMap<FGameplayTag, TSoftObjectPtr<USoundClass>> AudioVolumeSoundClassMap;

	/** Default sound mix used when adjusting audio volumes. */
	UPROPERTY(Config, EditAnywhere, Category = "Audio")
	TSoftObjectPtr<USoundMix> DefaultSoundMix;

	/** List of culture codes supported by the game (e.g. "en", "zh-Hans"). */
	UPROPERTY(Config, EditAnywhere, Category = "Localization")
	TArray<FString> SupportedCultures;

};
