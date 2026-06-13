// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "W_ActivatableBase.generated.h"

class AFrontendPlayerController;
/**
 * Base activatable widget with cached player controller access.
 *
 * All frontend widgets derive from this to get convenient GetPC()
 * without repeated GetOwningPlayer calls.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:

	/** Returns the owning FrontendPlayerController, cached on first access. */
	UFUNCTION(BlueprintPure)
	AFrontendPlayerController* GetPC();

private:

	TWeakObjectPtr<AFrontendPlayerController> CashedOwningPlayerController;

};
