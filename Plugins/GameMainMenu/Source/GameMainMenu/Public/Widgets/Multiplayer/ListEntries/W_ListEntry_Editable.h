// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "W_ListEntry_Editable.generated.h"

class UEditableText;
class UListDataObject_Editable;

/**
 * List entry with an EditableText control for room name / password input.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_Editable : public UW_ListEntry_Base
{
	GENERATED_BODY()

protected:
	// ~Begin UW_ListEntry_Base Interface
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData, EOptionsListDataModifyReason OwningModifyReason) override;
	virtual void OnToggleEditableState(bool bIsEditable) override;
	// ~End UW_ListEntry_Base Interface

	// ~Begin UUserWidget Interface
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	// ~End UUserWidget Interface

private:
	void TryCommitValidation();

	UFUNCTION()
	void OnEditableTextChanged(const FText& InText);

	/***** Bind Widget *****/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UEditableText* Edt_Input;
	/***** Bind Widget *****/

	UPROPERTY(Transient)
	UListDataObject_Editable* CachedEditableData;

	/** True if SetText has filtered characters since the last commit. */
	bool bHasFilteredSinceLastCommit = false;
};
