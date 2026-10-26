// HangHang All Rights Reserved


#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"

#include "CommonInputSubsystem.h"
#include "CommonInputTypeEnum.h"
#include "CommonTextBlock.h"
#include "Components/ListView.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

void UW_ListEntry_Base::NativeOnListEntryWidgetHovered(bool bIsHovered)
{
	BP_OnListEntryWidgetHovered(bIsHovered,GetListItem() ? IsListItemSelected() : false);

	if (bIsHovered)
	{
		BP_OnToggleEntryWidgetHighLightState(true);
	}
	else
	{
		BP_OnToggleEntryWidgetHighLightState(GetListItem() && IsListItemSelected() ? true : false);
	}
}

void UW_ListEntry_Base::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	OnOwningListDataObjectSet(CastChecked<UListDataObject_Base>(ListItemObject));
}

void UW_ListEntry_Base::NativeOnItemSelectionChanged(const bool bIsSelected)
{
	IUserObjectListEntry::NativeOnItemSelectionChanged(bIsSelected);
	
	BP_OnToggleEntryWidgetHighLightState(bIsSelected);
}

void UW_ListEntry_Base::NativeOnEntryReleased()
{
	// ListView recycles entry widgets. Without unbinding, this entry keeps receiving the
	// previous data object's events after it has been reused for another row.
	if (CachedOwningListDataObject)
	{
		CachedOwningListDataObject->OnListDataModified.RemoveAll(this);
		CachedOwningListDataObject->OnDependencyDataModified.RemoveAll(this);
		CachedOwningListDataObject = nullptr;
	}

	IUserObjectListEntry::NativeOnEntryReleased();
	
	NativeOnListEntryWidgetHovered(false);
}

FReply UW_ListEntry_Base::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem(); CommonInputSubsystem && CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		if (const UWidget* WidgetToFocus = BP_GetWidgetToFocusForGamepad())
		{
			if (const TSharedPtr<SWidget> SlateWidgetToFocus = WidgetToFocus->GetCachedWidget())
			{
				return FReply::Handled().SetUserFocus(SlateWidgetToFocus.ToSharedRef());
			}
		}
	}

	return Super::NativeOnFocusReceived(InGeometry,InFocusEvent);
}

void UW_ListEntry_Base::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	if (Ctb_EntryDisplayName) Ctb_EntryDisplayName->SetText(InOwningListDataObject->GetDataDisplayName());

	if (!InOwningListDataObject->OnListDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnListDataModified.AddUObject(this,&ThisClass::OnOwningListDataObjectModified);
	}

	if (!InOwningListDataObject->OnDependencyDataModified.IsBoundToObject(this))
	{
		InOwningListDataObject->OnDependencyDataModified.AddUObject(this,&ThisClass::OnOwningDependencyDataModified);
	}

	OnToggleEditableState(InOwningListDataObject->IsDataCurrentlyEditable());

	CachedOwningListDataObject = InOwningListDataObject;
}

void UW_ListEntry_Base::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData,
	EOptionsListDataModifyReason OwningModifyReason)
{
	
}

void UW_ListEntry_Base::OnOwningDependencyDataModified(UListDataObject_Base* DependencyModifyData,
	EOptionsListDataModifyReason DependencyModifyReason)
{
	if (CachedOwningListDataObject) OnToggleEditableState(CachedOwningListDataObject->IsDataCurrentlyEditable());
}

void UW_ListEntry_Base::OnToggleEditableState(bool bIsEditable)
{
	if (Ctb_EntryDisplayName) Ctb_EntryDisplayName->SetIsEnabled(bIsEditable);
}

void UW_ListEntry_Base::SelectedThisEntryWidget()
{
	CastChecked<UListView>(GetOwningListView())->SetSelectedItem(GetListItem());
}
