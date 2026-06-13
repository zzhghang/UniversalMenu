// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AA_PushConfirmScreen.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConfirmScreenButtonClickedDelegate,EConfirmScreenButtonType,ClickedButtonType);

/**
 * Blueprint async action that pushes a confirm dialog to the modal stack.
 *
 * Call PushConfirmScreen with a confirm type (OK, Yes/No, OK/Cancel),
 * title, and message. Bind to OnConfirmScreenButtonClicked for the user's choice.
 */
UCLASS()
class GAMEMAINMENU_API UAA_PushConfirmScreen : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", HidePin = "WorldContextObject", BlueprintInternalUseOnly = "true", DisplayName = "Push Confirm Screen"))
	static UAA_PushConfirmScreen* PushConfirmScreen(const UObject* WorldContextObject,EConfirmScreenType InConfirmType,FText InTitle,FText InMessage);

	// begin UBlueprintAsyncActionBase interface
	virtual void Activate() override;
	// end UBlueprintAsyncActionBase interface

	/** Broadcast when the user clicks a button on the confirm dialog. */
	UPROPERTY(BlueprintAssignable)
	FOnConfirmScreenButtonClickedDelegate OnConfirmScreenButtonClicked;
	
private:

	TWeakObjectPtr<UWorld> CachedOwningWorld;
	
	EConfirmScreenType CachedConfirmType;
	
	FText CachedTitle;
	
	FText CachedMessage;
	
};
