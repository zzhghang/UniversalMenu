// HangHang All Rights Reserved

#include "Widgets/W_KeyRemapScreen.h"

#include "CommonInputSubsystem.h"
#include "CommonInputSettings.h"
#include "CommonInputBaseTypes.h"
#include "ICommonInputModule.h"
#include "CommonUITypes.h"
#include "CommonRichTextBlock.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/SlateApplication.h"

#include "LocalizationKeys.h"

/**
 * Slate input preprocessor that captures the next valid key/mouse press.
 *
 * Installed during key remapping to intercept all input. Filters by input type:
 * - MouseAndKeyboard mode: rejects gamepad keys and cancels if current input switches to gamepad.
 * - Gamepad mode: rejects keyboard/mouse keys and maps LeftMouseButton → the configured gamepad confirm key.
 * Escape always cancels.
 */
class FKeyRemapScreenInputPreprocessor : public IInputProcessor
{
public:

	FKeyRemapScreenInputPreprocessor(ECommonInputType InInputType, ULocalPlayer* InLocalPlayer)
		: CachedInputType(InInputType), CachedWeakLocalPlayer(InLocalPlayer)
	{}

	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeyPressedDelegate, const FKey& /*PressedKey*/)
	FOnInputPreProcessorKeyPressedDelegate OnInputPreProcessorKeyPressed;

	DECLARE_DELEGATE_OneParam(FOnInputPreProcessorKeySelectedCancledDelegate, const FString& /*CancelReason*/)
	FOnInputPreProcessorKeySelectedCancledDelegate OnInputPreProcessorKeySelectedCancel;

protected:

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override
	{
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		ProcessPressedKey(InKeyEvent.GetKey());
		return true;
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		ProcessPressedKey(MouseEvent.GetEffectingButton());
		return true;
	}

	void ProcessPressedKey(const FKey& InPressedKey)
	{
		// Escape always cancels the remap session.
		if (InPressedKey == EKeys::Escape)
		{
			OnInputPreProcessorKeySelectedCancel.ExecuteIfBound(TEXT("Key Remap Canceled by Escape Key"));
			return;
		}

		UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(CachedWeakLocalPlayer.Get());
		if (!ensure(InputSubsystem)) { return; }

		ECommonInputType CurrentInputType = InputSubsystem->GetCurrentInputType();

		switch (CachedInputType)
		{
		case ECommonInputType::MouseAndKeyboard:
			// Cancel if the user pressed a gamepad key, or if the active input type changed to gamepad.
			if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
			{
				OnInputPreProcessorKeySelectedCancel.ExecuteIfBound(TEXT("Key Remap Canceled by Gamepad Key Pressed"));
				return;
			}
			break;
		case ECommonInputType::Gamepad:
			// On gamepad, left mouse click is remapped to the gamepad confirm key (e.g. FaceButtonBottom).
			if (CurrentInputType == ECommonInputType::Gamepad && InPressedKey == EKeys::LeftMouseButton)
			{
				FCommonInputActionDataBase* InputActionData = ICommonInputModule::GetSettings().GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT(""));
				if (!ensure(InputActionData)) { return; }

				OnInputPreProcessorKeyPressed.ExecuteIfBound(InputActionData->GetDefaultGamepadInputTypeInfo().GetKey());
				return;
			}
			// Reject non-gamepad keys when remapping a gamepad slot.
			if (!InPressedKey.IsGamepadKey())
			{
				OnInputPreProcessorKeySelectedCancel.ExecuteIfBound(TEXT("Key Remap Canceled by MouseAndKeyboard Key Pressed"));
				return;
			}
			break;
		default:
			break;
		}

		// Valid key for the requested input type.
		OnInputPreProcessorKeyPressed.ExecuteIfBound(InPressedKey);
	}

private:

	ECommonInputType CachedInputType;
	TWeakObjectPtr<ULocalPlayer> CachedWeakLocalPlayer;
};

void UW_KeyRemapScreen::SetRemapInputType(ECommonInputType InInputType, ULocalPlayer* InLocalPlayer)
{
	CachedDesiredInputType = InInputType;
	CachedWeakLocalPlayer = InLocalPlayer;
}

void UW_KeyRemapScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	bDeactivationRequested = false;

	ULocalPlayer* LocalPlayer = CachedWeakLocalPlayer.IsValid() ? CachedWeakLocalPlayer.Get() : GetOwningLocalPlayer();

	CachedKeyRemapInputPreprocessor = MakeShared<FKeyRemapScreenInputPreprocessor>(CachedDesiredInputType, LocalPlayer);

	CachedKeyRemapInputPreprocessor->OnInputPreProcessorKeyPressed.BindUObject(this, &ThisClass::OnValidKeyPressedDetected);
	CachedKeyRemapInputPreprocessor->OnInputPreProcessorKeySelectedCancel.BindUObject(this, &ThisClass::OnKeySelectionCancel);

	FSlateApplication::Get().RegisterInputPreProcessor(CachedKeyRemapInputPreprocessor);

	if (Ctb_RemapMessage)
	{
		const bool bIsKeyboard = (CachedDesiredInputType == ECommonInputType::MouseAndKeyboard);

		const FText Prompt = bIsKeyboard
			? TXT_KEYREMAP_PROMPT_KEYBOARD()
			: TXT_KEYREMAP_PROMPT_GAMEPAD();

		Ctb_RemapMessage->SetText(Prompt);
	}
}

void UW_KeyRemapScreen::NativeOnDeactivated()
{
	if (CachedKeyRemapInputPreprocessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(CachedKeyRemapInputPreprocessor);
		CachedKeyRemapInputPreprocessor.Reset();
	}

	Super::NativeOnDeactivated();
}

void UW_KeyRemapScreen::OnValidKeyPressedDetected(const FKey& PressedKey)
{
	RequestDeactivationWidget(
		[this, PressedKey]()
		{
			OnKeyRemapScreenKeyPressed.ExecuteIfBound(PressedKey);
		});
}

void UW_KeyRemapScreen::OnKeySelectionCancel(const FString& CancelReason)
{
	RequestDeactivationWidget(
		[this, CancelReason]()
		{
			OnKeyRemapScreenKeyCancel.ExecuteIfBound(CancelReason);
		});
}

void UW_KeyRemapScreen::RequestDeactivationWidget(TFunction<void()> PreDeactivationCallback)
{
	// Several key events can arrive in the same frame (chords, key repeat); only the first one may deactivate.
	if (bDeactivationRequested)
	{
		return;
	}
	bDeactivationRequested = true;

	// Stop listening right away, so no further input can queue another callback before the ticker runs.
	if (CachedKeyRemapInputPreprocessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(CachedKeyRemapInputPreprocessor);
		CachedKeyRemapInputPreprocessor.Reset();
	}

	// Capture weakly instead of using CreateWeakLambda: FTSTicker runs the delegate through Execute(),
	// which skips the safety check, so a dead widget would still report a dangling this to the lambda.
	TWeakObjectPtr<UW_KeyRemapScreen> WeakThis(this);

	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda(
			[WeakThis, PreDeactivationCallback = MoveTemp(PreDeactivationCallback)](float DeltaTime)->bool
			{
				if (UW_KeyRemapScreen* RemapScreen = WeakThis.Get())
				{
					PreDeactivationCallback();

					RemapScreen->DeactivateWidget();
				}

				return false;
			}
		)
	);
}


