// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

#include "FrontendSettings/FrontendGameUserSettings.h"

void UListDataObject_Base::InitializeDataObject()
{
	OnDataObjectInitialized();
}

void UListDataObject_Base::AddEditConditionDescriptor(const FOptionsDataEditConditionDescriptor& InEditConditionDesc)
{
	EditConditionDescArray.Add(InEditConditionDesc);
}

void UListDataObject_Base::AddEditDependencyData(UListDataObject_Base* InDependencyDataObject)
{
	if (!InDependencyDataObject->OnListDataModified.IsBoundToObject(this))
	{
		InDependencyDataObject->OnListDataModified.AddUObject(this,&ThisClass::OnEditDependencyDataModified);
	}
}

bool UListDataObject_Base::IsDataCurrentlyEditable()
{
	bool bIsEditable = true;
	
	if (EditConditionDescArray.IsEmpty()) return bIsEditable;

	FString CachedDisableRichReason;
	for (const FOptionsDataEditConditionDescriptor& EditConditionDesc : EditConditionDescArray)
	{
		if (!EditConditionDesc.IsValid() || EditConditionDesc.IsEditConditionMet() == true)
		{
			continue;
		}
		
		bIsEditable = false;

		CachedDisableRichReason.Append(EditConditionDesc.GetDisableRichReason());

		SetDisableRichText(FText::FromString(CachedDisableRichReason));

		if (EditConditionDesc.HasDisabledForcedStringValue())
		{
			if (const FString ForceStringValue = EditConditionDesc.GetDisabledForcedStringValue(); CanSetToForcedStringValue(ForceStringValue))
			{
				OnSetForcedStringValue(ForceStringValue);
			}
		}
	}

	return bIsEditable;
}

void UListDataObject_Base::OnDataObjectInitialized()
{
	
}

void UListDataObject_Base::NotifyListDataModified(UListDataObject_Base* ModifyData,
	const EOptionsListDataModifyReason ModifyReason)
{
	OnListDataModified.Broadcast(ModifyData,ModifyReason);
	
	if (bShouldApplyChangeImmediately) UFrontendGameUserSettings::Get()->ApplySettings(true);

}

void UListDataObject_Base::OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData,
	const EOptionsListDataModifyReason ModifyReason)
{
	OnDependencyDataModified.Broadcast(ModifyDependencyData,ModifyReason);
}
