// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h"
#include "FrontendLoadingScreenSettings.generated.h"

/**
 * 
 */
UCLASS(Config = GameMainMenu,DefaultConfig)
class GAMEMAINMENU_API UFrontendLoadingScreenSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	TSubclassOf<UUserWidget> GetSoftLoadScreenWidgetClassCheck() const;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	TSoftClassPtr<UUserWidget> LoadingScreenWidget;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	float HoldLoadingScreenTime = 3.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	bool bShowLoadingScreenInEditor = false;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	TSoftObjectPtr<ULoadingScreenPhaseMapping> PhaseMapping;
};
