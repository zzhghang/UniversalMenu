// HangHang All Rights Reserved


#include "Widgets/Multiplayer/ListEntries/W_ListEntry_Editable.h"

#include "Components/EditableText.h"
#include "Containers/Ticker.h"
#include "LocalizationKeys.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"

void UW_ListEntry_Editable::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedEditableData = CastChecked<UListDataObject_Editable>(InOwningListDataObject);

	if (Edt_Input)
	{
		Edt_Input->SetText(CachedEditableData->GetText());
		Edt_Input->SetHintText(CachedEditableData->GetHintText());
		Edt_Input->SetIsPassword(CachedEditableData->GetbIsPassword());
		Edt_Input->OnTextChanged.AddUniqueDynamic(this, &ThisClass::OnEditableTextChanged);
	}
}

void UW_ListEntry_Editable::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData,
	EOptionsListDataModifyReason OwningModifyReason)
{
	if (!Edt_Input || !CachedEditableData)
	{
		return;
	}

	const FText& DataText = CachedEditableData->GetText();
	if (!Edt_Input->GetText().EqualTo(DataText))
	{
		// ponytail: defer to next tick; SetText during OnTextChanged corrupts Slate state
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
			[WeakInput = TWeakObjectPtr<UEditableText>(Edt_Input), DataText](float) -> bool
			{
				if (WeakInput.IsValid())
				{
					WeakInput->SetText(DataText);
				}
				return false;
			}));
	}
}

void UW_ListEntry_Editable::OnToggleEditableState(bool bIsEditable)
{
	Super::OnToggleEditableState(bIsEditable);

	if (Edt_Input) Edt_Input->SetIsEnabled(bIsEditable);
}

void UW_ListEntry_Editable::OnEditableTextChanged(const FText& InText)
{
	if (!CachedEditableData)
	{
		return;
	}

	const FString Before = InText.ToString();
	CachedEditableData->SetText(InText);

	if (Before != CachedEditableData->GetText().ToString())
	{
		bHasFilteredSinceLastCommit = true;
	}
}

FReply UW_ListEntry_Editable::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Enter && Edt_Input && Edt_Input->HasKeyboardFocus())
	{
		FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
			[WeakThis = TWeakObjectPtr<UW_ListEntry_Editable>(this)](float) -> bool
			{
				if (WeakThis.IsValid())
				{
					WeakThis->TryCommitValidation();
				}
				return false;
			}));
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

void UW_ListEntry_Editable::TryCommitValidation()
{
	if (!Edt_Input || !CachedEditableData)
	{
		return;
	}

	const FText WidgetText = Edt_Input->GetText();
	const FText& Filtered = CachedEditableData->GetText();

	if (!WidgetText.EqualTo(Filtered))
	{
		Edt_Input->SetText(Filtered);
	}

	if (!bHasFilteredSinceLastCommit)
	{
		return;
	}

	bHasFilteredSinceLastCommit = false;

	FText Reason;
	if (!CachedEditableData->ValidateText(WidgetText, Reason))
	{
		if (UFrontendUISubsystem* Subsystem = UFrontendUISubsystem::Get(this))
		{
			Subsystem->PushConfirmWidgetToModalStackAsync(
				EConfirmScreenType::Ok,
				TXT_MP_VALIDATION_TITLE(),
				Reason,
				[](EConfirmScreenButtonType) {});
		}
	}
}
