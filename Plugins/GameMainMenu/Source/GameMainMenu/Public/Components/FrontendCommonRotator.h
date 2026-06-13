// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonRotator.h"
#include "FrontendCommonRotator.generated.h"

/**
 * Rotator widget that supports selection by display text rather than index.
 *
 * Used by options string entries (e.g. difficulty, quality presets) to
 * programmatically sync the rotator to a value from config.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UFrontendCommonRotator : public UCommonRotator
{
	GENERATED_BODY()

public:

	/** Select the rotator option matching the given display text. */
	void SetSelectedOptionByText(const FText& InTextOption);

};
