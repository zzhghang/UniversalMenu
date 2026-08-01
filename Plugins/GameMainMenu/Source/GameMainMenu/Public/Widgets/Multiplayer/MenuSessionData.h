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
	FString GetSavedValue(const FName& DataId) const
	{
		if (const FString* Found = SavedValues.Find(DataId))
		{
			return *Found;
		}
		return FString();
	}

	void SetSavedValue(const FName& DataId, const FString& Value)
	{
		SavedValues.Add(DataId, Value);
	}

	UPROPERTY(SaveGame)
	TMap<FName, FString> SavedValues;
};
