// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "FrontendDeveloperSettings.generated.h"

class UW_ActivatableBase;
class USoundClass;
class USoundMix;

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Frontend UI Settings"))
class GAMEMAINMENU_API UFrontendDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, EditAnywhere, Category = "Widget Ref", meta = (ForceInlineRow, Categories = "Frontend.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UW_ActivatableBase>> FrontendWidgetMap;

	UPROPERTY(Config, EditAnywhere, Category = "Options Image Ref", meta = (ForceInlineRow, Categories = "Frontend.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;

	UPROPERTY(Config, EditAnywhere, Category = "Audio", meta = (ForceInlineRow, Categories = "Frontend.Audio.Volume"))
	TMap<FGameplayTag, TSoftObjectPtr<USoundClass>> AudioVolumeSoundClassMap;

	UPROPERTY(Config, EditAnywhere, Category = "Audio")
	TSoftObjectPtr<USoundMix> DefaultSoundMix;

	UPROPERTY(Config, EditAnywhere, Category = "Localization")
	TArray<FString> SupportedCultures;

};
