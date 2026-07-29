// HangHang All Rights Reserved


#include "Widgets/Options/W_OptionDetailView.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "CommonRichTextBlock.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"

void UW_OptionDetailView::UpdateDetailsViewInfo(UListDataObject_Base* InDataObject,
                                                const FString& InEntryWidgetClassName)
{
	if (!InDataObject) return;

	Ctb_Title->SetText(InDataObject->GetDataDisplayName());

	if (!InDataObject->GetSoftDescriptionImage().IsNull())
	{
		Ctb_DesImage->SetBrushFromLazyTexture(InDataObject->GetSoftDescriptionImage());
		Ctb_DesImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		Ctb_DesImage->SetVisibility(ESlateVisibility::Collapsed);
	}

	Ctb_Des->SetText(InDataObject->GetDescriptionRichText());
	
	/****************   DynamicDetails  ****************/
	
		Ctb_DynamicDetails->SetText(InDataObject->GetDynamicDetailsRichText());
	/****************   DynamicDetails  ****************/
	
	/****************   DisabledReason  ****************/

	Ctb_DisabledReason->SetText(InDataObject->IsDataCurrentlyEditable()? FText::GetEmpty() : InDataObject->GetDisableRichText());
	
	/****************   DisabledReason  ****************/
	
}

void UW_OptionDetailView::ClearDetailsViewInfo()
{
	Ctb_Title->SetText(FText::GetEmpty());
	Ctb_DesImage->SetVisibility(ESlateVisibility::Collapsed);
	Ctb_Des->SetText(FText::GetEmpty());
	Ctb_DynamicDetails->SetText(FText::GetEmpty());
	Ctb_DisabledReason->SetText(FText::GetEmpty());
}

void UW_OptionDetailView::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ClearDetailsViewInfo();
}
