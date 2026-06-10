// HangHang All Rights Reserved


#include "Widgets/Options/OptionsDataInteractionHelper.h"

#include "FrontendSettings/FrontendGameUserSettings.h"


FOptionsDataInteractionHelper::FOptionsDataInteractionHelper(const FString& InSetterOrGetterPath)
: CachedDynamicFunctionPath(InSetterOrGetterPath)
{
	CachedWeakGameUserSettings = UFrontendGameUserSettings::Get();
}

FString FOptionsDataInteractionHelper::GetValueAsString() const
{
	FString OutStringValue; 
	PropertyPathHelpers::GetPropertyValueAsString(CachedWeakGameUserSettings.Get(),CachedDynamicFunctionPath, OutStringValue);

	return OutStringValue;
}

void FOptionsDataInteractionHelper::SetValueFromString(const FString& InStringValue) const
{
	PropertyPathHelpers::SetPropertyValueFromString(CachedWeakGameUserSettings.Get(), CachedDynamicFunctionPath, InStringValue);
}
