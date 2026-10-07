// HangHang All Rights Reserved
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Misc/DefaultValueHelper.h"

FCommonNumberFormattingOptions UListDataObject_Scalar::NoDecimal()
{
	return WithDecimal(0);
}

FCommonNumberFormattingOptions UListDataObject_Scalar::WithDecimal(int32 NumFracDigit)
{
	FCommonNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = NumFracDigit;
	return Options;
}

float UListDataObject_Scalar::GetCurrentValue() const
{
	return FMath::GetMappedRangeValueClamped(OutValueRange, DisplayValueRange,
		Binding ? StringToFloat(Binding->GetValueAsString()) : LocalValue);
}

void UListDataObject_Scalar::SetCurrentValue(float NewValue)
{
	if (!FMath::IsFinite(NewValue)) return;
	TrySetValueFromString(LexToString(FMath::GetMappedRangeValueClamped(DisplayValueRange, OutValueRange, NewValue)));
}

void UListDataObject_Scalar::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();
	LocalValue = OutValueRange.GetLowerBoundValue();
	float Default = 0.f;
	if (HasDefaultValue() && FDefaultValueHelper::ParseFloat(GetDefaultStringValue(), Default) && FMath::IsFinite(Default))
		LocalValue = FMath::Clamp(Default, OutValueRange.GetLowerBoundValue(), OutValueRange.GetUpperBoundValue());
}

bool UListDataObject_Scalar::TryGetValueAsString(FString& OutValue) const
{
	OutValue = Binding ? Binding->GetValueAsString() : LexToString(LocalValue);
	return true;
}

bool UListDataObject_Scalar::TrySetValueFromString(const FString& Value)
{
	float Parsed = 0.f;
	if (!FDefaultValueHelper::ParseFloat(Value, Parsed) || !FMath::IsFinite(Parsed)) return false;
	LocalValue = FMath::Clamp(Parsed, OutValueRange.GetLowerBoundValue(), OutValueRange.GetUpperBoundValue());
	if (Binding) Binding->SetValueFromString(LexToString(LocalValue));
	NotifyListDataModified(this);
	return true;
}

bool UListDataObject_Scalar::CanResetBackToDefaultValue() const
{
	if (!HasDefaultValue()) return false;
	float Default = 0.f;
	if (!FDefaultValueHelper::ParseFloat(GetDefaultStringValue(), Default) || !FMath::IsFinite(Default)) return false;
	Default = FMath::Clamp(Default, OutValueRange.GetLowerBoundValue(), OutValueRange.GetUpperBoundValue());
	return !FMath::IsNearlyEqual(Default, Binding ? StringToFloat(Binding->GetValueAsString()) : LocalValue, 0.01f);
}

bool UListDataObject_Scalar::TryResetBackToDefaultValue()
{
	if (!CanResetBackToDefaultValue()) return false;
	const float Default = StringToFloat(GetDefaultStringValue());
	LocalValue = FMath::Clamp(Default, OutValueRange.GetLowerBoundValue(), OutValueRange.GetUpperBoundValue());
	if (Binding) Binding->SetValueFromString(Default == LocalValue ? GetDefaultStringValue() : LexToString(LocalValue));
	NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
	return true;
}

void UListDataObject_Scalar::OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData, EOptionsListDataModifyReason ModifyReason)
{
	NotifyListDataModified(this, EOptionsListDataModifyReason::DependencyModified);
	Super::OnEditDependencyDataModified(ModifyDependencyData, ModifyReason);
}

float UListDataObject_Scalar::StringToFloat(const FString& InString) const
{
	float Value = 0.f;
	LexFromString(Value, *InString);
	return Value;
}
