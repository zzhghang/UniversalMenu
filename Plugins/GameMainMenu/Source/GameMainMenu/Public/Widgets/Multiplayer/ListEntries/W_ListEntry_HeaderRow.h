// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "W_ListEntry_HeaderRow.generated.h"

class UHorizontalBox;
class UListDataObject_HeaderRow;

/**
 * List entry that renders a multi-column text header from UListDataObject_HeaderRow config.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_HeaderRow : public UW_ListEntry_Base
{
	GENERATED_BODY()

protected:
	// ~Begin UW_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnToggleEditableState(bool bIsEditable) override {}
	// ~End UW_ListEntry_Base Interface

private:
	UPROPERTY(EditDefaultsOnly, Category = "Header Row")
	TSubclassOf<class UCommonTextStyle> HeaderTextStyle;

	/***** Bind Widget *****/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UHorizontalBox* Hb_HeaderColumns;
	/***** Bind Widget *****/
};
