// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "W_ListEntry_String.generated.h"

class UListDataObject_String;
class UFrontendCommonRotator;
class UFrontendCommonButtonBase;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_String : public UW_ListEntry_Base
{
	GENERATED_BODY()
	
protected:

	// ~ begin UUserWidget interface
	virtual void NativeOnInitialized() override;
	// ~ end UUserWidget interface

	//~Begin UWidget_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData, EOptionsListDataModifyReason OwningModifyReason) override;
	
	virtual void OnToggleEditableState(bool bIsEditable) override;
	//~End UWidget_ListEntry_Base Interface
	
private:
	
	void OnPreviousOptionButtonClicked();
	
	void OnNextOptionButtonClicked();

	void OnRotatorValueChanged(int32 Value, bool bUseInitiated);
	
private:
	
	/***** Bind Widget *****/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonButtonBase* Cbb_PreviousOption;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonRotator* Cr_Options;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonButtonBase* Cbb_NextOption;
	/***** Bind Widget *****/
	
	UPROPERTY(Transient)
	UListDataObject_String* CachedOwningStringDataObject;
	
};
