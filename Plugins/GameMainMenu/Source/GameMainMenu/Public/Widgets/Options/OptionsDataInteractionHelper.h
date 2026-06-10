// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "PropertyPathHelpers.h"


class UFrontendGameUserSettings;

class GAMEMAINMENU_API FOptionsDataInteractionHelper
{
	
public:
	explicit FOptionsDataInteractionHelper(const FString& InSetterOrGetterPath);

	FString GetValueAsString() const;
	void SetValueFromString(const FString& InStringValue) const;

private:

	FCachedPropertyPath CachedDynamicFunctionPath;

	TWeakObjectPtr<UFrontendGameUserSettings> CachedWeakGameUserSettings;
	
	
};
