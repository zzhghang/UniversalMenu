// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontendPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMAINMENU_API AFrontendPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	
	// Begin APlayerController Interface
	virtual void OnPossess(APawn* InPawn) override;
	// End APlayerController Interface

private:

	static void RunHardwareBenchmarkAndApply();
	
	
};
