// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Session.generated.h"

USTRUCT(BlueprintType)
struct GAMEMAINMENU_API FSessionListItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int32 SessionIndex = -1;

	UPROPERTY(BlueprintReadWrite)
	FText ServerName;

	UPROPERTY(BlueprintReadWrite)
	FText MapName;

	UPROPERTY(BlueprintReadWrite)
	int32 CurrentPlayers = 0;

	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers = 0;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPrivate = false;

	UPROPERTY(BlueprintReadWrite)
	int32 Ping = 0;

	UPROPERTY(BlueprintReadWrite)
	FBlueprintSessionResult SessionResult;
};

UCLASS()
class GAMEMAINMENU_API UListDataObject_Session : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void SetSessionData(const FSessionListItem& InData);

	const FSessionListItem& GetSessionData() const { return SessionData; }

	int32 GetSessionIndex() const { return SessionData.SessionIndex; }

private:
	FSessionListItem SessionData;
};
