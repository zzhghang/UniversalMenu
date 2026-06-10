// HangHang All Rights Reserved


#include "Widgets/Options/ListEntries/W_ListEntry_String.h"

#include "CommonInputSubsystem.h"
#include "Components/FrontendCommonButtonBase.h"
#include "Components/FrontendCommonRotator.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

void UW_ListEntry_String::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Cbb_PreviousOption->OnClicked().AddUObject(this,&ThisClass::OnPreviousOptionButtonClicked);
	Cbb_NextOption->OnClicked().AddUObject(this,&ThisClass::OnNextOptionButtonClicked);

	Cr_Options->OnClicked().AddLambda([this](){SelectedThisEntryWidget();});
	Cr_Options->OnRotatedEvent.AddUObject(this,&ThisClass::OnRotatorValueChanged);
}

void UW_ListEntry_String::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningStringDataObject = CastChecked<UListDataObject_String>(InOwningListDataObject);

	Cr_Options->PopulateTextLabels(CachedOwningStringDataObject->GetOptionsTextArray());

	Cr_Options->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentTextValue());
}

void UW_ListEntry_String::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData,
	EOptionsListDataModifyReason OwningModifyReason)
{
	if (CachedOwningStringDataObject) Cr_Options->SetSelectedOptionByText(CachedOwningStringDataObject->GetCurrentTextValue());
}

void UW_ListEntry_String::OnToggleEditableState(bool bIsEditable)
{
	Super::OnToggleEditableState(bIsEditable);
	
	Cbb_PreviousOption->SetIsEnabled(bIsEditable);
	Cbb_NextOption->SetIsEnabled(bIsEditable);
	Cr_Options->SetIsEnabled(bIsEditable);
}

void UW_ListEntry_String::OnPreviousOptionButtonClicked()
{
	if (CachedOwningStringDataObject) CachedOwningStringDataObject->GotoPreOption();

	SelectedThisEntryWidget();
}

void UW_ListEntry_String::OnNextOptionButtonClicked()
{
	if (CachedOwningStringDataObject) CachedOwningStringDataObject->GotoNextOption();
	
	SelectedThisEntryWidget();
}

void UW_ListEntry_String::OnRotatorValueChanged(int32 Value, bool bUseInitiated)
{
	if (!CachedOwningStringDataObject) return;

	const UCommonInputSubsystem* CommonInputSubsystem = GetInputSubsystem();

	if (!CommonInputSubsystem || !bUseInitiated) return;

	if (CommonInputSubsystem->GetCurrentInputType() == ECommonInputType::Gamepad)
	{
		CachedOwningStringDataObject->OnRotatorInitiatedValueChanged(Cr_Options->GetSelectedText());
	}
}
