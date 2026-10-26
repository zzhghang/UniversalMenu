// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrontendFunctionLibrary.generated.h"

class UW_ActivatableBase;
class USoundMix;
class USoundClass;

/**
 * Static utility library for frontend resource resolution.
 *
 * All functions read from UFrontendDeveloperSettings to resolve
 * GameplayTags to concrete asset references at runtime.
 */
UCLASS()
class GAMEMAINMENU_API UFrontendFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Resolve a widget tag to its soft class reference from developer settings. */
	UFUNCTION(BlueprintPure,Category="Frontend Function Library")
	static TSoftClassPtr<UW_ActivatableBase> GetFrontendSoftWidgetClassByTag(UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag);

	/** Resolve an image tag to its soft texture reference; returns null when the tag is not mapped. */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftObjectPtr<UTexture2D> GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "Frontend.Image")) FGameplayTag InImageTag);

	/** Get the default sound mix from developer settings. */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static USoundMix* GetDefaultSoundMix();

	/** Resolve a volume tag to its sound class from developer settings. */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static USoundClass* GetSoundClassByVolumeTag(UPARAM(meta = (Categories = "Frontend.Audio.Volume")) FGameplayTag InVolumeTag);

	/** Get the project version from Project Settings. */
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static FString GetProjectVersion();

};
