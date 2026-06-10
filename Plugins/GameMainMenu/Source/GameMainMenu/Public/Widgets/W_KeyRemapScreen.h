// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_KeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FKeyRemapScreenInputPreprocessor;

UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_KeyRemapScreen : public UW_ActivatableBase
{
	GENERATED_BODY()

public:

	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/)
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;

	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyCancledDelegate, const FString& /*CancelReason*/)
	FOnKeyRemapScreenKeyCancledDelegate OnKeyRemapScreenKeyCancel;

	void SetRemapInputType(ECommonInputType InInputType, ULocalPlayer* InLocalPlayer);

protected:

	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

private:

	void OnValidKeyPressedDetected(const FKey& PressedKey);
	void OnKeySelectionCancel(const FString& CancelReason);

	void RequestDeactivationWidget(TFunction<void()> PreDeactivationCallback);

	TSharedPtr<FKeyRemapScreenInputPreprocessor> CachedKeyRemapInputPreprocessor;

	ECommonInputType CachedDesiredInputType = ECommonInputType::MouseAndKeyboard;
	TWeakObjectPtr<ULocalPlayer> CachedWeakLocalPlayer;

	/************** bind widget  **************/
	UPROPERTY(meta = (BindWidget))
	UCommonRichTextBlock* Ctb_RemapMessage;
	/************** bind widget  **************/
};
