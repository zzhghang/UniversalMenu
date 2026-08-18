// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FindSessionsCallbackProxy.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "W_ListEntry_Session.generated.h"

class UHorizontalBox;
class UCommonTextBlock;

UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_Session : public UW_ListEntry_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Session Entry")
	FBlueprintSessionResult GetSessionResult() const { return CachedSessionResult; }

protected:
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnToggleEditableState(bool bIsEditable) override {}

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FBlueprintSessionResult CachedSessionResult;

	/***** Bind Widget *****/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UHorizontalBox* Hb_SessionColumns;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UCommonTextBlock* Tb_ServerName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UCommonTextBlock* Tb_MapName;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UCommonTextBlock* Tb_Players;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UCommonTextBlock* Tb_Private;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UCommonTextBlock* Tb_Ping;
	/***** Bind Widget *****/
};
