// HangHang All Rights Reserved

#include "Widgets/Options/OptionsDataInteractionHelper.h"

#include "GameFramework/GameUserSettings.h"
#include "FrontendSettings/FrontendResolution.h"
#include "Misc/DefaultValueHelper.h"

FString FOptionsDataInteractionHelper::ValueToString(EWindowMode::Type Value)
{
	return StaticEnum<EWindowMode::Type>()->GetNameStringByValue(static_cast<int64>(Value));
}

FString FOptionsDataInteractionHelper::ValueToString(const FIntPoint& Value)
{
	return FString::Printf(TEXT("(X=%d,Y=%d)"), Value.X, Value.Y);
}

bool FOptionsDataInteractionHelper::TryParseValue(const FString& InValue, FString& OutValue)
{
	OutValue = InValue;
	return true;
}

bool FOptionsDataInteractionHelper::TryParseValue(const FString& InValue, bool& OutValue)
{
	// Preserve the existing FString::ToBool representations.
	OutValue = InValue.ToBool();
	return true;
}

bool FOptionsDataInteractionHelper::TryParseValue(const FString& InValue, int32& OutValue)
{
	const FString TrimmedValue = InValue.TrimStartAndEnd();
	const TCHAR* Start = *TrimmedValue;
	TCHAR* End = nullptr;
	const int64 ParsedValue = FCString::Strtoi64(Start, &End, 10);
	if (End == Start || *End != '\0' || ParsedValue < MIN_int32 || ParsedValue > MAX_int32)
	{
		return false;
	}
	OutValue = static_cast<int32>(ParsedValue);
	return true;
}

bool FOptionsDataInteractionHelper::TryParseValue(const FString& InValue, float& OutValue)
{
	return FDefaultValueHelper::ParseFloat(InValue, OutValue) && FMath::IsFinite(OutValue);
}

bool FOptionsDataInteractionHelper::TryParseValue(const FString& InValue, EWindowMode::Type& OutValue)
{
	const int64 Value = StaticEnum<EWindowMode::Type>()->GetValueByNameString(InValue);
	if (Value != EWindowMode::Fullscreen && Value != EWindowMode::WindowedFullscreen && Value != EWindowMode::Windowed)
	{
		return false;
	}
	OutValue = static_cast<EWindowMode::Type>(Value);
	return true;
}

bool FOptionsDataInteractionHelper::TryParseValue(const FString& InValue, FIntPoint& OutValue)
{
	return FrontendResolution::Parse(InValue, OutValue);
}
