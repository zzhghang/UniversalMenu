// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Value.h"
#include "CommonNumericTextBlock.h"
#include "ListDataObject_Scalar.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_Scalar : public UListDataObject_Value
{
	GENERATED_BODY()
	
public:

	LIST_DATA_ACCESSOR(TRange<float>, DisplayValueRange);
	LIST_DATA_ACCESSOR(TRange<float>, OutValueRange);
	LIST_DATA_ACCESSOR(float, SliderStepSize);
	LIST_DATA_ACCESSOR(ECommonNumericType, CommonNumericType);
	LIST_DATA_ACCESSOR(FCommonNumberFormattingOptions, CommonNumberFormattingOptions);

	static FCommonNumberFormattingOptions NoDecimal();
	static FCommonNumberFormattingOptions WithDecimal(int32 NumFracDigit);

	float GetCurrentValue() const;
	void SetCurrentValue(float NewValue);
	
	
private:

	// ~Begin UListDataObject_Base Interface
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData, EOptionsListDataModifyReason ModifyReason) override;
	// ~End UListDataObject_Base Interface

	float StringToFloat(const FString& InString) const;

	TRange<float> DisplayValueRange = TRange<float>(0.f,1.f);
	TRange<float> OutValueRange = TRange<float>(0.f,1.f);
	
	float SliderStepSize = 0.1f;
	
	ECommonNumericType CommonNumericType = ECommonNumericType::Number;
	
	FCommonNumberFormattingOptions CommonNumberFormattingOptions;
	
};
