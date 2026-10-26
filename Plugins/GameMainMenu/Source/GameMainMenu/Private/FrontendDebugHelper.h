// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "LogGameMainMenu.h"
#include "UObject/Class.h"

// Dev-only on-screen/log helper. Call sites must be inside #if !UE_BUILD_SHIPPING as well,
// because this namespace does not exist in Shipping builds.
#if !UE_BUILD_SHIPPING

namespace Debug
{
	inline void Print(const FString& Message, int32 InKey = -1, const FColor& Color = FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, 5.f, Color, Message);

			UE_LOG(LogGameMainMenu, Verbose, TEXT("%s"), *Message);
		}
	}

	inline void PrintFloat(const FString& Prefix, float Value, int32 Key = -1,
						   const FColor& Color = FColor::MakeRandomColor())
	{
		Print(FString::Printf(TEXT("%s: %f"), *Prefix, Value), Key, Color);
	}

	inline void PrintInt(const FString& Prefix, int32 Value, int32 Key = -1,
						 const FColor& Color = FColor::MakeRandomColor())
	{
		Print(FString::Printf(TEXT("%s: %d"), *Prefix, Value), Key, Color);
	}

	inline void PrintBool(const FString& Prefix, bool Value, int32 Key = -1,
						  const FColor& Color = FColor::MakeRandomColor())
	{
		Print(FString::Printf(TEXT("%s: %s"), *Prefix, Value ? TEXT("true") : TEXT("false")), Key, Color);
	}

	inline void PrintObject(const FString& Prefix, const UObject* Obj, int32 Key = -1,
							const FColor& Color = FColor::MakeRandomColor())
	{
		if (Obj)
		{
			Print(FString::Printf(TEXT("%s: %s (Class: %s)"),
				  *Prefix, *Obj->GetName(), *Obj->GetClass()->GetName()), Key, Color);
		}
		else
		{
			Print(Prefix + TEXT(": nullptr"), Key, FColor::Red);
		}
	}

	inline void PrintIf(bool Condition, const FString& Message, int32 Key = -1,
						const FColor& Color = FColor::MakeRandomColor())
	{
		if (Condition)
		{
			Print(Message, Key, Color);
		}
	}
}

#endif // !UE_BUILD_SHIPPING
