// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FrontendPlayerController.generated.h"

/**
 * Player controller for frontend/menu scenes.
 *
 * On possess, runs the hardware benchmark if not previously completed,
 * applying auto-detected quality settings.
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

	/** Run hardware benchmark and apply results if never benchmarked before. */
	static void RunHardwareBenchmarkAndApply();


};
