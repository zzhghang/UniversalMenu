// HangHang All Rights Reserved
#pragma once

#include "CoreMinimal.h"

namespace FrontendResolution
{
	// Allow custom window sizes and multi-monitor modes, but reject corrupt persisted dimensions.
	inline bool IsUsable(const FIntPoint& Size)
	{
		return Size.X > 0 && Size.Y > 0 && Size.X <= 32768 && Size.Y <= 32768;
	}

	inline bool Parse(const FString& Value, FIntPoint& OutSize)
	{
		FString Coordinates = Value;
		if (!Coordinates.RemoveFromStart(TEXT("(X=")) || !Coordinates.RemoveFromEnd(TEXT(")"))) return false;
		FString Width, Height;
		if (!Coordinates.Split(TEXT(",Y="), &Width, &Height)) return false;
		const auto ParseDimension = [](const FString& Text, int32& Out)
		{
			const FString Trimmed = Text.TrimStartAndEnd();
			const TCHAR* Start = *Trimmed;
			TCHAR* End = nullptr;
			const int64 Number = FCString::Strtoi64(Start, &End, 10);
			if (End == Start || *End != '\0' || Number <= 0 || Number > 32768) return false;
			Out = static_cast<int32>(Number);
			return true;
		};
		FIntPoint Parsed = FIntPoint::ZeroValue;
		if (!ParseDimension(Width, Parsed.X) || !ParseDimension(Height, Parsed.Y)) return false;
		OutSize = Parsed;
		return true;
	}
}
