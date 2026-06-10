// HangHang All Rights Reserved


#include "Widgets/W_ActivatableBase.h"

#include "Controllers/FrontendPlayerController.h"

AFrontendPlayerController* UW_ActivatableBase::GetPC()
{
	if (!CashedOwningPlayerController.IsValid()) CashedOwningPlayerController = GetOwningPlayer<AFrontendPlayerController>();
	
	return CashedOwningPlayerController.IsValid() ? CashedOwningPlayerController.Get() : nullptr;
}
