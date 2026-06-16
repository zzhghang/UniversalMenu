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

/** Callback phase for async widget push operations. */
enum class EAsyncPushWidgetState :  uint8
{
	OnCreateBeforePush, /**< Widget has been created but not yet pushed to the stack. */
	AfterPush           /**< Widget has been pushed and is now active in the stack. */
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonDescriptionTextUpdateDelegate, UFrontendCommonButtonBase*, BroadcastingButton,FText,DescriptionText);

/**
 * Frontend UI subsystem managing widget stack operations and primary layout access.
 *
 * Provides async loading and pushing of widgets to named stacks (modal, game menu, etc.)
 * and factory methods for confirm dialogs.
 */
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

	/** Async-load a widget class and push it to the specified widget stack. */
	void PushSoftWidgetToStackAsync(const FGameplayTag& InWidgetStackTag, TSoftClassPtr<UW_ActivatableBase> InSoftWidgetClass, TFunction<void(EAsyncPushWidgetState, UW_ActivatableBase*)> AsyncPushStackCallBack);

	/** Push a confirm dialog to the modal stack with title, message, and callback. */
	void PushConfirmWidgetToModalStackAsync(const EConfirmScreenType InConfirmType, const FText& InTitle, const FText& InMessage,TFunction<void(EConfirmScreenButtonType)> ButtonClickCallBack);

	UPROPERTY(BlueprintAssignable)
	FOnButtonDescriptionTextUpdateDelegate OnButtonDescriptionTextUpdate;


private:
	UPROPERTY(Transient)
	UW_PrimaryLayout* CreatedPrimaryLayout;
};
