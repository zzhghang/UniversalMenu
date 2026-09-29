// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Value.generated.h"


class FOptionsDataInteractionHelper;

/**
 * Abstract base for data objects that hold a config value with a read/write binding.
 *
 * Uses FOptionsDataInteractionHelper which dispatches to UFrontendGameUserSettings
 * UFUNCTIONs via TFunction — getter returns FString, setter accepts FString.
 */
UCLASS(Abstract)
class GAMEMAINMENU_API UListDataObject_Value : public UListDataObject_Base
{
	GENERATED_BODY()

public:

	/** Set the binding used to read and write the setting. */
	void SetBinding(const TSharedPtr<FOptionsDataInteractionHelper>& InBinding);

	void SetDefaultValueFromString(const FString& InStringValue) { DefaultStringValue = InStringValue;}

	// ~Begin UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override {return DefaultStringValue.IsSet();}
	// ~End UListDataObject_Base Interface

protected:

	FString GetDefaultStringValue() const { return DefaultStringValue.GetValue();}

	/** Shared read/write access to the current config value. */
	TSharedPtr<FOptionsDataInteractionHelper> Binding;

private:

	TOptional<FString> DefaultStringValue;

};
