// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"

/**
 * Bridges UFrontendGameUserSettings getters/setters with direct function dispatch.
 *
 * Stores TFunction getter (returns FString) and setter (accepts FString).
 * Created via CreateHelperFromEnum which maps EGameUserSettingFunction to
 * the corresponding UFUNCTION on UFrontendGameUserSettings.
 */
class GAMEMAINMENU_API FOptionsDataInteractionHelper
{
public:
	using FGetter = TFunction<FString()>;
	using FSetter = TFunction<void(const FString&)>;

	FOptionsDataInteractionHelper(FGetter InGetter, FSetter InSetter)
		: Getter(MoveTemp(InGetter)), Setter(MoveTemp(InSetter)) {}

	FString GetValueAsString() const { return Getter ? Getter() : FString(); }
	void SetValueFromString(const FString& InValue) const { if (Setter) Setter(InValue); }

private:
	FGetter Getter;
	FSetter Setter;
};
