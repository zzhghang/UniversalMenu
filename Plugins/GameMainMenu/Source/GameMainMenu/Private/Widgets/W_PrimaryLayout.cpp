// HangHang All Rights Reserverd


#include "Widgets/W_PrimaryLayout.h"

#include "FrontendDebugHelper.h"

UCommonActivatableWidgetContainerBase* UW_PrimaryLayout::FindWidgetStackByTag(const FGameplayTag& WidgetStackTag) const
{
	ensureMsgf(RegisteredWidgetStackMap.Contains(WidgetStackTag), TEXT("Can not find widget stack by the tag %s"), *WidgetStackTag.ToString());
	
	return RegisteredWidgetStackMap.FindRef(WidgetStackTag);
}

void UW_PrimaryLayout::RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag WidgetStackTag,
                                           UCommonActivatableWidgetContainerBase* WidgetStack)
{
	if (!IsDesignTime())
	{
		if (!RegisteredWidgetStackMap.Contains(WidgetStackTag))
		{
			RegisteredWidgetStackMap.Add(WidgetStackTag, WidgetStack);
		}
	}
}
