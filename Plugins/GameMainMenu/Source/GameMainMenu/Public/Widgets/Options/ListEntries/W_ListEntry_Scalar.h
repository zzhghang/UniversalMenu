// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "W_ListEntry_Scalar.generated.h"

class UListDataObject_Scalar;
class UCommonNumericTextBlock;
class UAnalogSlider;

UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_Scalar : public UW_ListEntry_Base
{
	GENERATED_BODY()
	
protected:

	// ~ begin UUserWidget interface
	virtual void NativeOnInitialized() override;
	// ~ end UUserWidget interface

	//~Begin UWidget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData, EOptionsListDataModifyReason OwningModifyReason) override;
	//~End UWidget_ListEntry_Base Interface
	
	
private:

	UFUNCTION()
	void OnSliderValueChanged(float NewValue);

	UFUNCTION()
	void OnMouseCaptureBegin();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UCommonNumericTextBlock* Cnb_Value;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = "true"))
	UAnalogSlider* As_Slider;
	
	UPROPERTY(Transient)
	UListDataObject_Scalar* CachedOwningScalarDataObject;
	
};
