// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "W_ActivatableBase.generated.h"

class AFrontendPlayerController;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ActivatableBase : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintPure)
	AFrontendPlayerController* GetPC();

private:

	TWeakObjectPtr<AFrontendPlayerController> CashedOwningPlayerController;
	
};
