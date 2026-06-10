// HangHang All Rights Reserved


#include "Components/FrontendCommonTabListWidgetBase.h"

#include "Components/FrontendCommonButtonBase.h"
#include "Editor/WidgetCompilerLog.h"


void UFrontendCommonTabListWidgetBase::RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName)
{
	RegisterTab(InTabId,TabButtonEntryWidgetClass,nullptr);

	if (const UFrontendCommonButtonBase* MenuButton = Cast<UFrontendCommonButtonBase>(GetTabButtonBaseByID(InTabId)))
	{
		MenuButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR	
void UFrontendCommonTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);

	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(
			TEXT("TabButtonEntryWidgetClass has no valid entry specified. ") +
			GetClass()->GetName() + 
			TEXT("need a valid entry widget class to function property")
		));
	}
}
#endif
