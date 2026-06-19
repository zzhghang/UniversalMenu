// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FrontendLoadingScreenInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFrontendLoadingScreenInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAMEMAINMENU_API IFrontendLoadingScreenInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenActivated();

	UFUNCTION(BlueprintNativeEvent)
	void OnLoadingScreenDeactivated();
};
