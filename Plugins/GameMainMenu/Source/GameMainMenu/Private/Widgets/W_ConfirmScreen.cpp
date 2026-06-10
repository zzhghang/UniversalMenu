// HangHang All Rights Reserved


#include "Widgets/W_ConfirmScreen.h"

#include "CommonTextBlock.h"
#include "Components/DynamicEntryBox.h"
#include "Components/FrontendCommonButtonBase.h"

UConfirmScreenInfoObject* UConfirmScreenInfoObject::CreateOkScreen(const FText& InTitle, const FText& InMessage)
{
	UConfirmScreenInfoObject* InfoObject = NewObject<UConfirmScreenInfoObject>();

	InfoObject->Title = InTitle;
	InfoObject->Message = InMessage;

	FConfirmScreenButtonInfo OkButtonInfo;
	OkButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Closed;
	OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("OK"));
	
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
	YesButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("YES"));
	
	FConfirmScreenButtonInfo NoButtonInfo;
	NoButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	NoButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("NO"));
	
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
	OkButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("OK"));

	FConfirmScreenButtonInfo CancelButtonInfo;
	CancelButtonInfo.ConfirmScreenButtonType = EConfirmScreenButtonType::Cancelled;
	CancelButtonInfo.ButtonTextToDisplay = FText::FromString(TEXT("CANCEL"));
	
	InfoObject->AvailableScreenButtons.Add(OkButtonInfo);
	InfoObject->AvailableScreenButtons.Add(CancelButtonInfo);

	return InfoObject;
}

void UW_ConfirmScreen::InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject,
	TFunction<void(EConfirmScreenButtonType)> ClickButtonCallBack)
{
	check(InScreenInfoObject && Ctb_Title && Ctb_Message && DynamicEntryBox_Buttons);

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
	
	check(!InScreenInfoObject->AvailableScreenButtons.IsEmpty());
	
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
