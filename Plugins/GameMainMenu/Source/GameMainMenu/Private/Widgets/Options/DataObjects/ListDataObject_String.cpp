// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_String.h"

#include "FrontendDebugHelper.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "LocalizationKeys.h"

void UListDataObject_String::AddDynamicString(const FString& InString, const FText& InText)
{
	StringArray.Add(InString);
	TextArray.Add(InText);
}

void UListDataObject_String::GotoPreOption()
{
	if (StringArray.IsEmpty() || TextArray.IsEmpty()) return;

	const int32 CurrentDisplayIndex = StringArray.IndexOfByKey(CurrentStringValue);
	const int32 PreviousDisplayIndex = CurrentDisplayIndex - 1;

	if (const bool bIsPreviousIndexValid = StringArray.IsValidIndex(PreviousDisplayIndex))
	{
		CurrentStringValue = StringArray[PreviousDisplayIndex];
	}
	else
	{
		CurrentStringValue = StringArray.Last();
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);

		NotifyListDataModified(this);
	}
}

void UListDataObject_String::GotoNextOption()
{
	if (StringArray.IsEmpty() || TextArray.IsEmpty()) return;

	const int32 CurrentDisplayIndex = StringArray.IndexOfByKey(CurrentStringValue);
	const int32 NextDisplayIndex = CurrentDisplayIndex + 1;

	if (const bool bIsNextIndexValid = StringArray.IsValidIndex(NextDisplayIndex))
	{
		CurrentStringValue = StringArray[NextDisplayIndex];
	}
	else
	{
		CurrentStringValue = StringArray[0];
	}

	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);

		NotifyListDataModified(this);
	}
}

void UListDataObject_String::OnRotatorInitiatedValueChanged(const FText& InNewSelectedText)
{
	const int32 FoundIndex = TextArray.IndexOfByPredicate(
	[InNewSelectedText](const FText& InText)->bool
	{
		return InText.EqualTo(InNewSelectedText);
	}
	);

	if (FoundIndex != INDEX_NONE && StringArray.IsValidIndex(FoundIndex))
	{
		CurrentTextValue = InNewSelectedText;
		CurrentStringValue = StringArray[FoundIndex];

		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);

			NotifyListDataModified(this);
		}
	}
}

void UListDataObject_String::OnDataObjectInitialized()
{
	if (!StringArray.IsEmpty()) CurrentStringValue = StringArray[0];

	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultStringValue();
	}

	if (DataDynamicGetter)
	{
		if (!DataDynamicGetter->GetValueAsString().IsEmpty())
		{
			CurrentStringValue = DataDynamicGetter->GetValueAsString();
		}
	}

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue)) CurrentTextValue = TXT_VAL_INVALID();
}

bool UListDataObject_String::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && CurrentStringValue != GetDefaultStringValue();
}

bool UListDataObject_String::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		CurrentStringValue = GetDefaultStringValue();

		TrySetDisplayTextFromStringValue(CurrentStringValue);

		if (DataDynamicSetter)
		{
			DataDynamicSetter->SetValueFromString(CurrentStringValue);
			
			NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);

			return true;
		}
	}

	return false;
}

bool UListDataObject_String::CanSetToForcedStringValue(const FString& InForcedStringValue) const
{
	return CurrentStringValue != InForcedStringValue;
}

void UListDataObject_String::OnSetForcedStringValue(const FString& InForcedStringValue)
{
	CurrentStringValue = InForcedStringValue;
	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (DataDynamicSetter)
	{
		DataDynamicSetter->SetValueFromString(CurrentStringValue);

		NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	}
}

bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	if (const int32 CurrentFoundIndex = StringArray.IndexOfByKey(InStringValue); TextArray.IsValidIndex(CurrentFoundIndex))
	{
		CurrentTextValue = TextArray[CurrentFoundIndex];
		return true;
	}
	return false;
}

//****** UListDataObject_StringBool  ******//

void UListDataObject_StringBool::OverrideTrueDisplayText(const FText& InTrueText)
{
	if (!StringArray.Contains(OnString)) AddDynamicString(OnString, InTrueText);
}

void UListDataObject_StringBool::OverrideFalseDisplayText(const FText& InFalseText)
{
	if (!StringArray.Contains(OffString)) AddDynamicString(OffString, InFalseText);
}

void UListDataObject_StringBool::SetTrueAsDefaultValue()
{
	SetDefaultValueFromString(OnString);
}

void UListDataObject_StringBool::SetFalseAsDefaultValue()
{
	SetDefaultValueFromString(OffString);
}

void UListDataObject_StringBool::OnDataObjectInitialized()
{
	TryInitializeBoolOptions();
	
	Super::OnDataObjectInitialized();
}

void UListDataObject_StringBool::TryInitializeBoolOptions()
{
	if (!StringArray.Contains(OnString))
	{
		AddDynamicString(OnString, TXT_VAL_ON());
	}
	if (!StringArray.Contains(OffString))
	{
		AddDynamicString(OffString, TXT_VAL_OFF());
	}
}



//****** UListDataObject_StringBool  ******//


//****** UListDataObject_StringInteger  ******//
void UListDataObject_StringInteger::AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText)
{
	AddDynamicString(LexToString(InIntegerValue), InDisplayText);
}

void UListDataObject_StringInteger::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	
	if (!TrySetDisplayTextFromStringValue(CurrentStringValue)) CurrentTextValue = TXT_VAL_CUSTOM();
}

void UListDataObject_StringInteger::OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	if (DataDynamicGetter)
	{
		if (CurrentStringValue == DataDynamicGetter->GetValueAsString()) return;
		
		CurrentStringValue = DataDynamicGetter->GetValueAsString();

		if (!TrySetDisplayTextFromStringValue(CurrentStringValue)) CurrentTextValue = TXT_VAL_CUSTOM();

		NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	}
	
	Super::OnEditDependencyDataModified(ModifyDependencyData, ModifyReason);
}
//****** UListDataObject_StringInteger  ******//

