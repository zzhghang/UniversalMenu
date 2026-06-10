// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"

#include "Widgets/Options/OptionsDataInteractionHelper.h"

FCommonNumberFormattingOptions UListDataObject_Scalar::NoDecimal()
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;

	return Options;
}

FCommonNumberFormattingOptions UListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = NumFracDigit;

	return Options;
}

float UListDataObject_Scalar::GetCurrentValue() const
{
	if (DataDynamicGetter)
	{
		return FMath::GetMappedRangeValueClamped(
			OutValueRange,
			DisplayValueRange,
			StringToFloat(DataDynamicGetter->GetValueAsString())
		);
	}

	return 0.0f;
}

void UListDataObject_Scalar::SetCurrentValue(float NewValue)
{
	if (DataDynamicGetter)
	{
		const float ClampValue = FMath::GetMappedRangeValueClamped(
				DisplayValueRange,
				OutValueRange,
				NewValue
		);
		DataDynamicSetter->SetValueFromString(LexToString(ClampValue));

		NotifyListDataModified(this);
	}
}

bool UListDataObject_Scalar::CanResetBackToDefaultValue() const
{
	if (HasDefaultValue() && DataDynamicGetter)
	{
		const float DefaultValue = StringToFloat(GetDefaultStringValue());
		const float CurrentValue = StringToFloat(DataDynamicGetter->GetValueAsString());

		return !FMath::IsNearlyEqual(DefaultValue,CurrentValue,0.01f);
	}

	return false;
}

bool UListDataObject_Scalar::TryResetBackToDefaultValue()
{
	if (CanResetBackToDefaultValue())
	{
		if (DataDynamicGetter)
		{
			DataDynamicSetter->SetValueFromString(GetDefaultStringValue());

			NotifyListDataModified(this,EOptionsListDataModifyReason::ResetToDefault);
			
			return true;
		}
	}
	return false;
}

void UListDataObject_Scalar::OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData,
	EOptionsListDataModifyReason ModifyReason)
{
	NotifyListDataModified(this,EOptionsListDataModifyReason::DependencyModified);
	
	Super::OnEditDependencyDataModified(ModifyDependencyData, ModifyReason);
}


float UListDataObject_Scalar::StringToFloat(const FString& InString) const
{
	float OutConvertedValue = 0.f;

	LexFromString(OutConvertedValue,*InString);

	return OutConvertedValue;
}
