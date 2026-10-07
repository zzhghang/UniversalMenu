// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Value.h"
#include "CommonNumericTextBlock.h"
#include "ListDataObject_Scalar.generated.h"

/**
 * Numeric data object for sliders with display/out value ranges.
 *
 * DisplayValueRange is what the user sees/selects (e.g. 0-100%).
 * OutValueRange is the actual config value (e.g. 0.0-2.0 for volume scaling).
 * Values are mapped linearly between the two ranges.
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_Scalar : public UListDataObject_Value
{
	GENERATED_BODY()

public:

	/** Range shown in the UI slider. */
	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange);
	/** Range stored in config (mapped from DisplayValueRange). */
	LIST_DATA_ACCESSOR(TRange<float>, OutValueRange);
	LIST_DATA_ACCESSOR(float, SliderStepSize);
	LIST_DATA_ACCESSOR(ECommonNumericType, CommonNumericType);
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, CommonNumberFormattingOptions);

	/** Whole-number formatting (no decimal places). */
	static FCommonNumberFormattingOptions NoDecimal();
	/** Fractional formatting with specified decimal places. */
	static FCommonNumberFormattingOptions WithDecimal(int32 NumFracDigit);

	/** Read current value from UFrontendGameUserSettings via the getter helper. */
	float GetCurrentValue() const;
	/** Write value to UFrontendGameUserSettings via the setter helper. */
	void SetCurrentValue(float NewValue);
	virtual bool TryGetValueAsString(FString& OutValue) const override;
	virtual bool TrySetValueFromString(const FString& Value) override;
	
	
private:

	// ~Begin UListDataObject_Base Interface
	virtual bool CanResetBackToDefaultValue() const override;
	virtual void OnDataObjectInitialized() override;
	virtual bool TryResetBackToDefaultValue() override;
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData, EOptionsListDataModifyReason ModifyReason) override;
	// ~End UListDataObject_Base Interface

	float StringToFloat(const FString& InString) const;
	float LocalValue = 0.f;

	TRange<float> DisplayValueRange = TRange<float>(0.f,1.f);
	TRange<float> OutValueRange = TRange<float>(0.f,1.f);
	
	float SliderStepSize = 0.1f;
	
	ECommonNumericType CommonNumericType = ECommonNumericType::Number;
	
	FCommonNumberFormattingOptions CommonNumberFormattingOptions;
	
};
