// HangHang All Rights Reserved


#include "Widgets/W_ConfirmScreen.h"

#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Components/FrontendCommonButtonBase.h"
#include "LocalizationKeys.h"

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkScreen(const FText& InTitle, const FText& InMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->Title = InTitle;
	InfoObject->Message = InMessage;

	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	OkButtonInfo.ButtonTextToDisplay = TXT_UI_OK();

	InfoObject->AvailableScreenButtons.Add(OkButtonInfo);

	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateYesNoScreen(const FText& InTitle, const FText& InMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();
	
	InfoObject->Title = InTitle;
	InfoObject->Message = InMessage;
	
	FConfirmScreenButtonInfo YesButtonInfo;
	YesButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	YesButtonInfo.ButtonTextToDisplay = TXT_UI_YES();

	FConfirmScreenButtonInfo NoButtonInfo;
	NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	NoButtonInfo.ButtonTextToDisplay = TXT_UI_NO();
	
	InfoObject->AvailableScreenButtons.Add(YesButtonInfo);
	InfoObject->AvailableScreenButtons.Add(NoButtonInfo);
	
	return InfoObject;
}

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkCancelScreen(const FText& InTitle, const FText& InMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->Title = InTitle;
	InfoObject->Message = InMessage;

	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Confirmed;
	OkButtonInfo.ButtonTextToDisplay = TXT_UI_OK();

	FConfirmScreenButtonInfo CancelButtonInfo;
	CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	CancelButtonInfo.ButtonTextToDisplay = TXT_UI_CANCEL();
	
	InfoObject->AvailableScreenButtons.Add(OkButtonInfo);
	InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);

	return InfoObject;
}

void UW_ConfirmScreen::InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject,
	TFunction<void(EConfirmScreenButtonType)> ClickButtonCallBack)
{
	if (!ensure(InScreenInfoObject && Ctb_Title && Ctb_Message && DynamicEntryBox_Buttons)) { return; }

	Ctb_Title->SetText(InScreenInfoObject->Title);
	Ctb_Message->SetText(InScreenInfoObject->Message);
	
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0)
	{
		DynamicEntryBox_Buttons->Reset<UFrontendCommonButtonBase>(
			[](const UFrontendCommonButtonBase& ExistingButton)
			{
				ExistingButton.OnClicked().Clear();
			}
		);
	}
	
	if (!ensure(!InScreenInfoObject->AvailableScreenButtons.IsEmpty())) { return; }
	
	for (const FConfirmScreenButtonInfo& AvailableButtonInfo: InScreenInfoObject->AvailableScreenButtons)
	{
		const UFrontendCommonButtonBase* AddButton = DynamicEntryBox_Buttons->CreateEntry<UFrontendCommonButtonBase>();
		AddButton->SetButtonText(AvailableButtonInfo.ButtonTextToDisplay);
		AddButton->OnClicked().AddLambda(
			[ClickButtonCallBack, AvailableButtonInfo,this]()
			{
				ClickButtonCallBack(AvailableButtonInfo.ConfirmScreenButtonType);
				DeactivateWidget();
			}
		);
	}
}

UWidget* UW_ConfirmScreen::NativeGetDesiredFocusTarget() const
{
	if (DynamicEntryBox_Buttons->GetNumEntries() != 0) DynamicEntryBox_Buttons->GetAllEntries().Last()->SetFocus();

	return Super::NativeGetDesiredFocusTarget();
}
