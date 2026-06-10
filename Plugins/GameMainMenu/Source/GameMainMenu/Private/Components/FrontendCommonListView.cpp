// HangHang All Rights Reserved


#include "Components/FrontendCommonListView.h"

#include "Editor/WidgetCompilerLog.h"
#include "Widgets/DA_DataListEntryMapping.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"


UUserWidget& UFrontendCommonListView::OnGenerateEntryWidgetInternal(UObject* Item,
                                                                    const TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable)
{
	if (IsDesignTime())
	{
		return Super::OnGenerateEntryWidgetInternal(Item,DesiredEntryClass,OwnerTable);
	}

	if (const TSubclassOf<UW_ListEntry_Base> FindWidgetClass = DataListEntryMapping->FindEntryWidgetClassByDataObject(CastChecked<UListDataObject_Base>(Item)))
	{
		return  GenerateTypedEntry<UW_ListEntry_Base>(FindWidgetClass, OwnerTable);
	}
	else
	{
		return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
	}
}

bool UFrontendCommonListView::OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem)
{
	return !FirstSelectedItem->IsA<UListDataObject_Collection>();
}


#if WITH_EDITOR	
void UFrontendCommonListView::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!DataListEntryMapping)
	{
		CompileLog.Error(FText::FromString(
			TEXT("DataListEntryMapping has no valid data asset assigned. ") +
			GetClass()->GetName() +
			TEXT("need a valid data asset to function property")
		));
	}
}
#endif