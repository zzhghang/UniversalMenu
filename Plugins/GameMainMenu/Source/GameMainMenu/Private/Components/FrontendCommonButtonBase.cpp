// HangHang All Rights Reserved


#include "Components/FrontendCommonButtonBase.h"
#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "Subsystems/FrontendUISubsystem.h"

void UFrontendCommonButtonBase::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	SetButtonText(ButtonDisplayText);
}

void UFrontendCommonButtonBase::NativeOnCurrentTextStyleChanged()
{
	Super::NativeOnCurrentTextStyleChanged();
	
	if (Ctb_ButtonText && GetCurrentTextStyleClass()) Ctb_ButtonText->SetStyle(GetCurrentTextStyleClass());
}

void UFrontendCommonButtonBase::NativeOnHovered()
{
	Super::NativeOnHovered();
	
	if (!ButtonDescriptionText.IsEmpty()) UFrontendUISubsystem::Get(this)->OnButtonDescriptionTextUpdate.Broadcast(this, ButtonDescriptionText);
}

void UFrontendCommonButtonBase::NativeOnUnhovered()
{
	Super::NativeOnUnhovered();
	
	UFrontendUISubsystem::Get(this)->OnButtonDescriptionTextUpdate.Broadcast(this, FText());
}

void UFrontendCommonButtonBase::SetButtonText(const FText InText) const
{
	if (Ctb_ButtonText && !InText.IsEmpty())
	{
		Ctb_ButtonText->SetText(bUseUpperCaseForButtonText ? InText.ToUpper(): InText);
	}
}

FText UFrontendCommonButtonBase::GetButtonDisplayText() const
{
	return Ctb_ButtonText ? Ctb_ButtonText->GetText() : FText();
}

void UFrontendCommonButtonBase::SetButtonIcon(const FSlateBrush& InIcon)
{
	if (Cli_Icon) Cli_Icon->SetBrush(InIcon);
}
