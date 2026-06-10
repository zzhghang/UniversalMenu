// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FrontendUISubsystem.generated.h"


class UFrontendCommonButtonBase;
struct FGameplayTag;
class UW_ActivatableBase;
class UW_PrimaryLayout;

enum class EAsyncPushWidgetState :  uint8
{
	OnCreateBeforePush, 
	AfterPush          
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdateDelegate, UFrontendCommonButtonBase*, BroadcastingButton,FText,DescriptionText);

UCLASS()
class GAMEMAINMENU_API UFrontendUISubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UFrontendUISubsystem* Get(const UObject* WorldContextObject);
	
	// Begin USubsystem Interface
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	// End USubsystem Interface
	
	UFUNCTION(BlueprintCallable)
	void RegisterCreatedPrimaryLayoutWidget(UW_PrimaryLayout* CreatedWidget) { CreatedPrimaryLayout = CreatedWidget; }
	
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UW_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UW_ActivatableBase*)> AsyncPushStackCallBack) const;
	
	void PushConfirmWidgetToModalStackAsync(const EConfirmScreenType InConfirmType, const FText& InTitle, const FText& InMessage,TFunction<void(EConfirmScreenButtonType)> ButtonClickCallBack) const;
	
	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdateDelegate OnButtonDescriptionTextUpdate;

	
private:
	UPROPERTY(Transient)
	UW_PrimaryLayout* CreatedPrimaryLayout;
};
