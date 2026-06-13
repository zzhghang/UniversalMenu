// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"


class UFrontendGameUserSettings;

/**
 * Bridges UFrontendGameUserSettings getters/setters with PropertyPathHelpers string reflection.
 *
 * Given a UFUNCTION name (e.g. "GetOverallVolume"), uses FCachedPropertyPath
 * to call that function on UFrontendGameUserSettings and convert results to/from strings.
 */
class GAMEMAINMENU_API FOptionsDataInteractionHelper
{

public:
	/** @param InSetterOrGetterPath UFUNCTION name on UFrontendGameUserSettings (e.g. "GetOverallVolume"). */
	explicit FOptionsDataInteractionHelper(const FString& InSetterOrGetterPath);

	/** Reads the current value from UFrontendGameUserSettings as a string. */
	FString GetValueAsString() const;
	/** Writes a string value to the bound UFrontendGameUserSettings property. */
	void SetValueFromString(const FString& InStringValue) const;

private:

	FCachedPropertyPath CachedDynamicFunctionPath;

	TWeakObjectPtr<UFrontendGameUserSettings> CachedWeakGameUserSettings;


};
