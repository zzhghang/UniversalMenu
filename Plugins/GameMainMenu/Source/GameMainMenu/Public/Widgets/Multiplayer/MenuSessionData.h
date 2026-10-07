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
	/** Includes session-only values, which are never serialized to disk. Empty is a valid saved value. */
	bool TryGetSavedValue(FName Key, FString& OutValue) const
	{
		const FString* Found = SessionValues.Find(Key);
		if (!Found) Found = SavedValues.Find(Key);
		if (!Found) return false;
		OutValue = *Found;
		return true;
	}

	void RemoveSavedValue(FName Key) { SavedValues.Remove(Key); SessionValues.Remove(Key); }
	void SetSessionValue(FName Key, const FString& Value) { SavedValues.Remove(Key); SessionValues.Add(Key, Value); }
	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	FString GetSavedValue(const FName& DataId) const
	{
		FString Value;
		TryGetSavedValue(DataId, Value);
		return Value;
	}

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	FString GetSavedValueByString(const FString& DataId) const
	{
		return GetSavedValue(FName(*DataId));
	}

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	void SetSavedValue(const FName& DataId, const FString& Value)
	{
		SavedValues.Add(DataId, Value);
		SessionValues.Remove(DataId);
	}

	UFUNCTION(BlueprintCallable, Category = "MenuSession")
	void SetSavedValueByString(const FString& DataId, const FString& Value)
	{
		SetSavedValue(FName(*DataId), Value);
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	bool GetSavedValueAsBool(const FName& DataId, bool bDefault = false) const
	{
		FString Value;
		if (!TryGetSavedValue(DataId, Value)) return bDefault;
		if (Value.Equals(TEXT("true"), ESearchCase::IgnoreCase)) return true;
		if (Value.Equals(TEXT("false"), ESearchCase::IgnoreCase)) return false;
		return bDefault;
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	bool GetSavedValueAsBoolByString(const FString& DataId, bool bDefault = false) const
	{
		return GetSavedValueAsBool(FName(*DataId), bDefault);
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	int32 GetSavedValueAsInt(const FName& DataId, int32 Default = 0) const
	{
		FString Value;
		int32 Parsed = 0;
		return TryGetSavedValue(DataId, Value) && LexTryParseString(Parsed, *Value) ? Parsed : Default;
	}

	UFUNCTION(BlueprintPure, Category = "MenuSession")
	int32 GetSavedValueAsIntByString(const FString& DataId, int32 Default = 0) const
	{
		return GetSavedValueAsInt(FName(*DataId), Default);
	}

	UPROPERTY(SaveGame)
	TMap<FName, FString> SavedValues;

	UPROPERTY(Transient)
	TMap<FName, FString> SessionValues;
};
