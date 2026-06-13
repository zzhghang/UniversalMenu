// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Value.generated.h"


class FOptionsDataInteractionHelper;

/**
 * Abstract base for data objects that hold a config value with getter/setter helpers.
 *
 * Uses FOptionsDataInteractionHelper to bridge UFrontendGameUserSettings
 * getter/setter UFUNCTIONs with string-based value read/write.
 */
UCLASS(Abstract)
class GAMEMAINMENU_API UListDataObject_Value : public UListDataObject_Base
{
	GENERATED_BODY()

public:

	/** Set the getter helper used to read from UFrontendGameUserSettings. */
	void SetDataDynamicGetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicGetter);

	/** Set the setter helper used to write to UFrontendGameUserSettings. */
	void SetDataDynamicSetter(const TSharedPtr<FOptionsDataInteractionHelper>& InDynamicSetter);

	void SetDefaultValueFromString(const FString& InStringValue) { DefaultStringValue = InStringValue;}

	// ~Begin UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override {return DefaultStringValue.IsSet();}
	// ~End UListDataObject_Base Interface

protected:

	FString GetDefaultStringValue() const { return DefaultStringValue.GetValue();}

	/** Helper for reading the current config value via PropertyPathHelpers. */
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicGetter;

	/** Helper for writing the config value via PropertyPathHelpers. */
	TSharedPtr<FOptionsDataInteractionHelper> DataDynamicSetter;

private:

	TOptional<FString> DefaultStringValue;

};
