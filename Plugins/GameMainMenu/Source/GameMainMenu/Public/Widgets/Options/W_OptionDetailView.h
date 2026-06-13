// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_OptionDetailView.generated.h"

class UListDataObject_Base;
class UCommonTextBlock;
class UCommonLazyImage;
class UCommonRichTextBlock;
/**
 * Side panel that displays details for the currently selected/hovered options entry.
 *
 * Shows the entry name, description image, rich-text description,
 * and diagnostic info (data object class, entry widget class).
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_OptionDetailView : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Populate detail fields from a data object. @param InEntryWidgetClassName optional debug string for the entry widget. */
	void UpdateDetailsViewInfo(UListDataObject_Base* InDataObject,const FString& InEntryWidgetClassName = FString());

	/** Clear all detail text and hide the description image. */
	void ClearDetailsViewInfo();

protected:

	// ~ begin UUserWidget interface
	virtual void NativeOnInitialized() override;
	// ~ end UUserWidget interface
	
	
private:

	/********   bind widget  ********/
	
	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ctb_Title;

	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* Ctb_DesImage;

	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* Ctb_Des;

	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* Ctb_DynamicDetails;

	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* Ctb_DisabledReason;
	
	/********   bind widget  ********/
	
};
