// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_DataListEntryMapping.generated.h"

class UW_ListEntry_Base;
class UListDataObject_Base;
/**
 * 
 */
UCLASS()
class GAMEMAINMENU_API UDA_DataListEntryMapping : public UDataAsset
{
	GENERATED_BODY()

public:
	
	TSubclassOf<UW_ListEntry_Base> FindEntryWidgetClassByDataObject(const UListDataObject_Base* InDataObject) const;

private:
	
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UListDataObject_Base>, TSubclassOf<UW_ListEntry_Base>> DataObjectListEntryMap;
	
};
