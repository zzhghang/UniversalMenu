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
	
	const FString DynamicDetails = FString::Printf(
		TEXT("Data Object Class: <Bold>%s</>\n\nEntry Widget Class: <Bold>%s</>"),
		*InDataObject->GetClass()->GetName(),
		*InEntryWidgetClassName
	);

	Ctb_DynamicDetails->SetText(FText::FromString(DynamicDetails));

	/****************   DynamicDetails  ****************/

	//Ctb_DisabledReason->SetText(InDataObject->GetDisableRichText());
	Ctb_DisabledReason->SetText(InDataObject->IsDataCurrentlyEditable()? FText::GetEmpty() : InDataObject->GetDisableRichText());
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
