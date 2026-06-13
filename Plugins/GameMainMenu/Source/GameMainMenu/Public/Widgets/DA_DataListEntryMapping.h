// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_DataListEntryMapping.generated.h"

class UW_ListEntry_Base;
class UListDataObject_Base;
/**
 * DataAsset mapping UListDataObject types to their corresponding list entry widget classes.
 *
 * Used by UFrontendCommonListView to determine which widget to create for a given data item.
 * Walks up the class hierarchy when no exact match is found.
 */
UCLASS()
class GAMEMAINMENU_API UDA_DataListEntryMapping : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Find the entry widget class for a data object, walking up the class hierarchy. */
	TSubclassOf<UW_ListEntry_Base> FindEntryWidgetClassByDataObject(const UListDataObject_Base* InDataObject) const;

private:

	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UListDataObject_Base>, TSubclassOf<UW_ListEntry_Base>> DataObjectListEntryMap;

};
