// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AA_PushSoftWidget.generated.h"

class UW_ActivatableBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPushSoftWidgetDelegate, UW_ActivatableBase*, PushedWidget);

UCLASS()
class GAMEMAINMENU_API UAA_PushSoftWidget : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,meta = (WorldContext = "WorldContextObject",HidePin = "WorldContextObject",BlueprintInternalUseOnly = "true",DisplayName = "Push Soft Wiget To Widget Stack"))
	static UAA_PushSoftWidget* PushSoftWidget(const UObject* WorldContextObject,APlayerController* OwningPlayerController,TSoftClassPtr<UW_ActivatableBase> SoftWidgetClass, UPARAM(meta = (Categories = "Frontend.WidgetStack")) FGameplayTag WidgetStackTag,bool bFocus = true);
	
	// begin UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	// end UBlueprintAsyncActionBase interface

	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnWidgetCreatedBeforePush;
	
	UPROPERTY(BlueprintAssignable)
	FOnPushSoftWidgetDelegate OnAfterPush;
	
private:
	
	TWeakObjectPtr<UWorld>  CachedOwningWorld;
	
	TWeakObjectPtr<APlayerController>  CachedOwningPlayerController;
	
	TSoftClassPtr<UW_ActivatableBase> CachedSoftWidgetClass;
	
	FGameplayTag CachedWidgetStackTag;
	
	bool bCachedFocus = false;
};
