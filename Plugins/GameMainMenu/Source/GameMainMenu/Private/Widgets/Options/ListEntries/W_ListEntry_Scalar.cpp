// HangHang All Rights Reserved


#include "Widgets/Options/ListEntries/W_ListEntry_Scalar.h"

#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "AnalogSlider.h"

void UW_ListEntry_Scalar::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	As_Slider->OnValueChanged.AddUniqueDynamic(this,&ThisClass::OnSliderValueChanged);
	As_Slider->OnMouseCaptureBegin.AddUniqueDynamic(this,&ThisClass::OnMouseCaptureBegin);
}

void UW_ListEntry_Scalar::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);
	
	CachedOwningScalarDataObject = CastChecked<UListDataObject_Scalar>(InOwningListDataObject);

	Cnb_Value->SetNumericType(CachedOwningScalarDataObject->GetCommonNumericType());
	Cnb_Value->FormattingSpecification = CachedOwningScalarDataObject->GetCommonNumberFormattingOptions();
	Cnb_Value->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());

	As_Slider->SetMinValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetLowerBoundValue());
	As_Slider->SetMaxValue(CachedOwningScalarDataObject->GetDisplayValueRange().GetUpperBoundValue());
	As_Slider->SetStepSize(CachedOwningScalarDataObject->GetSliderStepSize());
	As_Slider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
}

void UW_ListEntry_Scalar::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData,
	EOptionsListDataModifyReason OwningModifyReason)
{
	if (CachedOwningScalarDataObject)
	{
		Cnb_Value->SetCurrentValue(CachedOwningScalarDataObject->GetCurrentValue());
		As_Slider->SetValue(CachedOwningScalarDataObject->GetCurrentValue());
	}
}

void UW_ListEntry_Scalar::OnSliderValueChanged(float NewValue)
{
	if (CachedOwningScalarDataObject)
	{
		CachedOwningScalarDataObject->SetCurrentValue(NewValue);
	}
}

void UW_ListEntry_Scalar::OnMouseCaptureBegin()
{
	SelectedThisEntryWidget();
}
