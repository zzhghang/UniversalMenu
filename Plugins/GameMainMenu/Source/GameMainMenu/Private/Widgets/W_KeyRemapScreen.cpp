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
		if (InPressedKey == EKeys::Escape)
		{
			OnInputPreProcessorKeySelectedCancel.ExecuteIfBound(TEXT("Key Remap Canceled by Escape Key"));
			return;
		}

		UCommonInputSubsystem* InputSubsystem = UCommonInputSubsystem::Get(CachedWeakLocalPlayer.Get());
		check(InputSubsystem);

		ECommonInputType CurrentInputType = InputSubsystem->GetCurrentInputType();

		switch (CachedInputType)
		{
		case ECommonInputType::MouseAndKeyboard:
			if (InPressedKey.IsGamepadKey() || CurrentInputType == ECommonInputType::Gamepad)
			{
				OnInputPreProcessorKeySelectedCancel.ExecuteIfBound(TEXT("Key Remap Canceled by Gamepad Key Pressed"));
				return;
			}
			break;
		case ECommonInputType::Gamepad:
			if (CurrentInputType == ECommonInputType::Gamepad && InPressedKey == EKeys::LeftMouseButton)
			{
				FCommonInputActionDataBase* InputActionData = ICommonInputModule::GetSettings().GetDefaultClickAction().GetRow<FCommonInputActionDataBase>(TEXT(""));
				check(InputActionData);

				OnInputPreProcessorKeyPressed.ExecuteIfBound(InputActionData->GetDefaultGamepadInputTypeInfo().GetKey());
				return;
			}
			if (!InPressedKey.IsGamepadKey())
			{
				OnInputPreProcessorKeySelectedCancel.ExecuteIfBound(TEXT("Key Remap Canceled by MouseAndKeyboard Key Pressed"));
				return;
			}
			break;
		default:
			break;
		}

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
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateLambda(
			[PreDeactivationCallback,this](float DeltaTime)->bool
			{
				PreDeactivationCallback();

				DeactivateWidget();

				return false;
			}
		)
	);
}


