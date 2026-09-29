// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GenericPlatform/GenericWindow.h"
#include <type_traits>

/**
 * Bridges typed settings accessors to the string values used by option entries.
 * Create supports bool, int32, float, FString, window modes and resolutions.
 * Custom lambdas can be supplied for other representations.
 */
class GAMEMAINMENU_API FOptionsDataInteractionHelper
{
public:
	using FGetter = TFunction<FString()>;
	using FSetter = TFunction<void(const FString&)>;

	FOptionsDataInteractionHelper(FGetter InGetter, FSetter InSetter)
		: Getter(MoveTemp(InGetter)), Setter(MoveTemp(InSetter)) {}

	/** Resolves the target on each call; getter/setter may belong to different base classes. */
	template <typename TProvider, typename TGetterClass, typename TResult, typename TSetterClass, typename TArgument>
	static TSharedRef<FOptionsDataInteractionHelper> Create(
		TProvider GetObject, TResult (TGetterClass::*InGetter)() const, void (TSetterClass::*InSetter)(TArgument))
	{
		using TValue = std::decay_t<TResult>;
		static_assert(std::is_same_v<TValue, std::decay_t<TArgument>>,
			"Option getter and setter must use the same value type.");
		static_assert(std::is_same_v<TValue, bool> || std::is_same_v<TValue, int32> ||
			std::is_same_v<TValue, float> || std::is_same_v<TValue, FString> ||
			std::is_same_v<TValue, EWindowMode::Type> || std::is_same_v<TValue, FIntPoint>,
			"Use custom binding lambdas for this option value type.");

		return MakeShared<FOptionsDataInteractionHelper>(
			[GetObject, InGetter]() -> FString
			{
				if (auto* Object = GetObject())
				{
					return ValueToString((Object->*InGetter)());
				}
				return FString();
			},
			[GetObject, InSetter](const FString& InValue)
			{
				TValue Value{};
				if (TryParseValue(InValue, Value))
				{
					if (auto* Object = GetObject())
					{
						(Object->*InSetter)(Value);
					}
				}
			});
	}

	FString GetValueAsString() const { return Getter ? Getter() : FString(); }
	void SetValueFromString(const FString& InValue) const { if (Setter) Setter(InValue); }

private:
	template <typename TValue>
	static FString ValueToString(const TValue& Value) { return LexToString(Value); }
	static FString ValueToString(EWindowMode::Type Value);
	static FString ValueToString(const FIntPoint& Value);

	static bool TryParseValue(const FString& InValue, FString& OutValue);
	static bool TryParseValue(const FString& InValue, bool& OutValue);
	static bool TryParseValue(const FString& InValue, int32& OutValue);
	static bool TryParseValue(const FString& InValue, float& OutValue);
	static bool TryParseValue(const FString& InValue, EWindowMode::Type& OutValue);
	static bool TryParseValue(const FString& InValue, FIntPoint& OutValue);

	FGetter Getter;
	FSetter Setter;
};
