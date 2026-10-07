// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_String.h"

#include "FrontendDebugHelper.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "LocalizationKeys.h"
#include "Misc/DefaultValueHelper.h"

void UListDataObject_String::AddDynamicString(const FString& InString, const FText& InText)
{
	StringArray.Add(InString);
	TextArray.Add(InText);
}

bool UListDataObject_String::AreOptionValuesEqual(const FString& Left, const FString& Right) const
{
	if (Left == Right) return true;
	if (!bUseNumericOptionMatching) return false;

	// Compare in the float domain used by the settings binding. Do not round distinct frame rates.
	float LeftValue = 0.f;
	float RightValue = 0.f;
	return FDefaultValueHelper::ParseFloat(Left, LeftValue) && FMath::IsFinite(LeftValue) &&
		FDefaultValueHelper::ParseFloat(Right, RightValue) && FMath::IsFinite(RightValue) &&
		LeftValue == RightValue;
}

int32 UListDataObject_String::FindOptionIndex(const FString& InValue) const
{
	// Prefer the exact key if an asset contains more than one representation of a number.
	const int32 ExactIndex = StringArray.IndexOfByKey(InValue);
	if (ExactIndex != INDEX_NONE || !bUseNumericOptionMatching) return ExactIndex;

	return StringArray.IndexOfByPredicate([this, &InValue](const FString& Option)
	{
		return AreOptionValuesEqual(Option, InValue);
	});
}

void UListDataObject_String::GotoPreOption()
{
	if (StringArray.IsEmpty() || TextArray.IsEmpty()) return;

	const int32 CurrentDisplayIndex = FindOptionIndex(CurrentStringValue);
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

	if (Binding)
	{
		Binding->SetValueFromString(CurrentStringValue);
	}

	NotifyListDataModified(this);
}

void UListDataObject_String::GotoNextOption()
{
	if (StringArray.IsEmpty() || TextArray.IsEmpty()) return;

	const int32 CurrentDisplayIndex = FindOptionIndex(CurrentStringValue);
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

	if (Binding)
	{
		Binding->SetValueFromString(CurrentStringValue);
	}

	NotifyListDataModified(this);
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

		if (Binding)
		{
			Binding->SetValueFromString(CurrentStringValue);
		}

		NotifyListDataModified(this);
	}
}

void UListDataObject_String::OnDataObjectInitialized()
{
	if (!StringArray.IsEmpty()) CurrentStringValue = StringArray[0];

	if (HasDefaultValue())
	{
		CurrentStringValue = GetDefaultStringValue();
	}

	if (Binding)
	{
		if (!Binding->GetValueAsString().IsEmpty())
		{
			CurrentStringValue = Binding->GetValueAsString();
		}
	}

	if (!TrySetDisplayTextFromStringValue(CurrentStringValue)) CurrentTextValue = TXT_VAL_INVALID();
}

bool UListDataObject_String::CanResetBackToDefaultValue() const
{
	return HasDefaultValue() && !AreOptionValuesEqual(CurrentStringValue, GetDefaultStringValue());
}

bool UListDataObject_String::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		CurrentStringValue = GetDefaultStringValue();

		TrySetDisplayTextFromStringValue(CurrentStringValue);

		if (Binding)
		{
			Binding->SetValueFromString(CurrentStringValue);
		}
		NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);
		return true;
	}

	return false;
}

bool UListDataObject_String::CanSetToForcedStringValue(const FString& InForcedStringValue) const
{
	return !AreOptionValuesEqual(CurrentStringValue, InForcedStringValue);
}

void UListDataObject_String::OnSetForcedStringValue(const FString& InForcedStringValue)
{
	CurrentStringValue = InForcedStringValue;
	TrySetDisplayTextFromStringValue(CurrentStringValue);

	if (Binding)
	{
		Binding->SetValueFromString(CurrentStringValue);
	}
	NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
}

bool UListDataObject_String::TrySetValueFromString(const FString& Value)
{
	const int32 Index = FindOptionIndex(Value);
	if (Index == INDEX_NONE) return false;
	OnSetForcedStringValue(StringArray[Index]);
	return true;
}

bool UListDataObject_String::TrySetDisplayTextFromStringValue(const FString& InStringValue)
{
	if (const int32 CurrentFoundIndex = FindOptionIndex(InStringValue); TextArray.IsValidIndex(CurrentFoundIndex))
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
	if (Binding)
	{
		if (CurrentStringValue == Binding->GetValueAsString()) return;

		CurrentStringValue = Binding->GetValueAsString();

		if (!TrySetDisplayTextFromStringValue(CurrentStringValue)) CurrentTextValue = TXT_VAL_CUSTOM();

		NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	}

	Super::OnEditDependencyDataModified(ModifyDependencyData, ModifyReason);
}
//****** UListDataObject_StringInteger  ******//
