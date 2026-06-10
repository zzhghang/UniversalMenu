// HangHang All Rights Reserverd

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"
#include "W_PrimaryLayout.generated.h"

class UCommonActivatableWidgetContainerBase;


UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_PrimaryLayout : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	UCommonActivatableWidgetContainerBase* FindWidgetStackByTag(const FGameplayTag& WidgetStackTag) const;
	
protected:
	UFUNCTION(BlueprintCallable)
	void RegisterWidgetStack(UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag WidgetStackTag, UCommonActivatableWidgetContainerBase* WidgetStack);
	
	
private:
	UPROPERTY(Transient)
	TMap<FGameplayTag,UCommonActivatableWidgetContainerBase*> RegisteredWidgetStackMap; 
	
};
