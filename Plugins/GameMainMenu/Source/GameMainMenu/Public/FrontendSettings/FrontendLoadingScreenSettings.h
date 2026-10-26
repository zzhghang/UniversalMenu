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

	/** Safety net: force-hide the loading screen after it has been requested for this many seconds. <= 0 disables the timeout. */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings", meta = (ClampMin = "0.0"))
	float MaxLoadingScreenTime = 120.0f;

	/** How long to keep ticking for late local player controllers before giving up on the deactivated notification. <= 0 waits forever. */
	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings", meta = (ClampMin = "0.0"))
	float MaxWaitForPlayerControllerTime = 30.0f;

	UPROPERTY(Config, EditAnywhere, Category = "Loading Screen Settings")
	TSoftObjectPtr<ULoadingScreenPhaseMapping> PhaseMapping;
};
