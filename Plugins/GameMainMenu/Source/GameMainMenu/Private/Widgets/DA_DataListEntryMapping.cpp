// HangHang All Rights Reserved


#include "Widgets/DA_DataListEntryMapping.h"

#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

TSubclassOf<UW_ListEntry_Base> UDA_DataListEntryMapping::FindEntryWidgetClassByDataObject(
	const UListDataObject_Base* InDataObject) const
{
	check(InDataObject);

	for (UClass* DataObjectClass = InDataObject->GetClass(); DataObjectClass; DataObjectClass = DataObjectClass->GetSuperClass())
	{
		if (TSubclassOf<UListDataObject_Base> ConvertedDataObjectClass = TSubclassOf<UListDataObject_Base>(DataObjectClass))
		{
			if (DataObjectListEntryMap.Contains(ConvertedDataObjectClass))
			{
				return DataObjectListEntryMap.FindRef(ConvertedDataObjectClass);
			}
		}
	}

	return TSubclassOf<UW_ListEntry_Base>();
}
