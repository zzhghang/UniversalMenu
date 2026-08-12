// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"

void UListDataObject_Collection::AddChildListData(UListDataObject_Base* InChildListData)
{
	InChildListData->InitializeDataObject();

	InChildListData->SetParentData(this);

	ChildListDataArray.Add(InChildListData);
}

TArray<UListDataObject_Base*> UListDataObject_Collection::GetAllChildListData() const
{
	return ChildListDataArray;
}

void UListDataObject_Collection::RemoveChildListData(UListDataObject_Base* InChildListData)
{
	ChildListDataArray.Remove(InChildListData);
}

bool UListDataObject_Collection::HasAnyChildListData() const
{
	return !ChildListDataArray.IsEmpty();
}
