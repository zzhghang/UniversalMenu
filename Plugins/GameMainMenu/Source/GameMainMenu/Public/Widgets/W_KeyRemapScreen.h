// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonInputTypeEnum.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_KeyRemapScreen.generated.h"

class UCommonRichTextBlock;
class FKeyRemapScreenInputPreprocessor;

/**
 * Modal overlay that captures a single key/mouse press for key rebinding.
 *
 * Installs a Slate input preprocessor to intercept all input until a valid
 * key is pressed or the user cancels (Escape / incompatible input type).
 * Results are reported via OnKeyRemapScreenKeyPressed and OnKeyRemapScreenKeyCancel delegates.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_KeyRemapScreen : public UW_ActivatableBase
{
	GENERATED_BODY()

public:

	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyPressedDelegate, const FKey& /*PressedKey*/)
	/** Broadcast when a valid key is captured by the input preprocessor. */
	FOnKeyRemapScreenKeyPressedDelegate OnKeyRemapScreenKeyPressed;

	DECLARE_DELEGATE_OneParam(FOnKeyRemapScreenKeyCancledDelegate, const FString& /*CancelReason*/)
	/** Broadcast when the user cancels (Escape or incompatible input type). */
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
