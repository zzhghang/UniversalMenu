// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FrontendFunctionLibrary.generated.h"

class UW_ActivatableBase;
class USoundMix;
class USoundClass;

UCLASS()
class GAMEMAINMENU_API UFrontendFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure,Category="Frontend Function Library")
	static TSoftClassPtr<UW_ActivatableBase> GetFrontendSoftWidgetClassByTag(UPARAM(meta = (Categories = "Frontend.Widget")) FGameplayTag InWidgetTag);

	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftObjectPtr<UTexture2D> GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "Frontend.Image")) FGameplayTag InImageTag);

	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static USoundMix* GetDefaultSoundMix();

	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static USoundClass* GetSoundClassByVolumeTag(UPARAM(meta = (Categories = "Frontend.Audio.Volume")) FGameplayTag InVolumeTag);

};
