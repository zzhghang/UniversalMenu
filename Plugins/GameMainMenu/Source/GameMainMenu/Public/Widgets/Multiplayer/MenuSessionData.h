// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MenuSessionData.generated.h"

/**
 * Generic key-value persistence for MultiplayerScreen session data.
 * Stores all editable field values keyed by DataId, so DataAsset-driven
 * field additions/removals don't require C++ changes.
 */
UCLASS()
class GAMEMAINMENU_API UMenuSessionData : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	FString GetSavedValue(const FName& DataId) const
	{
		if (const FString* Found = SavedValues.Find(DataId))
		{
			return *Found;
		}
		return FString();
	}

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	FString GetSavedValueByString(const FString& DataId) const
	{
		if (const FString* Found = SavedValues.Find(FName(*DataId)))
		{
			return *Found;
		}
		return FString();
	}

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	void SetSavedValue(const FName& DataId, const FString& Value)
	{
		SavedValues.Add(DataId, Value);
	}

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	void SetSavedValueByString(const FString& DataId, const FString& Value)
	{
		SavedValues.Add(FName(*DataId), Value);
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	bool GetSavedValueAsBool(const FName& DataId, bool bDefault = false) const
	{
		const FString* Found = SavedValues.Find(DataId);
		return Found ? Found->Equals(TEXT("true"), ESearchCase::IgnoreCase) : bDefault;
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	bool GetSavedValueAsBoolByString(const FString& DataId, bool bDefault = false) const
	{
		const FString* Found = SavedValues.Find(FName(*DataId));
		return Found ? Found->Equals(TEXT("true"), ESearchCase::IgnoreCase) : bDefault;
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	int32 GetSavedValueAsInt(const FName& DataId, int32 Default = 0) const
	{
		const FString* Found = SavedValues.Find(DataId);
		return Found ? FCString::Atoi(**Found) : Default;
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	int32 GetSavedValueAsIntByString(const FString& DataId, int32 Default = 0) const
	{
		const FString* Found = SavedValues.Find(FName(*DataId));
		return Found ? FCString::Atoi(**Found) : Default;
	}

	UPROPERTY(SaveGame)
	TMap<FName, FString> SavedValues;
};
