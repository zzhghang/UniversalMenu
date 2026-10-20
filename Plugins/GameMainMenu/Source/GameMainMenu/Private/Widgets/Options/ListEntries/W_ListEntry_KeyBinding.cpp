// HangHang All Rights Reserved

#include "Widgets/Options/ListEntries/W_ListEntry_KeyBinding.h"
#include "Widgets/Options/DataObjects/UListDataObject_KeyBinding.h"
#include "Components/FrontendCommonButtonBase.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "Widgets/W_KeyRemapScreen.h"
#include "FrontendGameplayTags.h"
#include "FunctionLibrary/FrontendFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "W_ListEntry_KeyBinding"

void UW_ListEntry_KeyBinding::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Cbb_KeyboardSlot)
	{
		Cbb_KeyboardSlot->OnClicked().AddUObject(this, &ThisClass::OnKeyboardSlotClicked);
	}
	if (Cbb_GamepadSlot)
	{
		Cbb_GamepadSlot->OnClicked().AddUObject(this, &ThisClass::OnGamepadSlotClicked);
	}
}

void UW_ListEntry_KeyBinding::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	CachedOwningKeyBindingDataObject = CastChecked<UUListDataObject_KeyBinding>(InOwningListDataObject);

	const int32 SlotCount = CachedOwningKeyBindingDataObject->GetKeyBindingSlots().Num();
	for (int32 i = 0; i < SlotCount; ++i)
	{
		BP_OnSlotKeyUpdated(i);
	}
	UpdateButtonIcons();
}

void UW_ListEntry_KeyBinding::OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData,
	EOptionsListDataModifyReason OwningModifyReason)
{
	if (!CachedOwningKeyBindingDataObject) return;

	const int32 SlotCount = CachedOwningKeyBindingDataObject->GetKeyBindingSlots().Num();
	for (int32 i = 0; i < SlotCount; ++i)
	{
		BP_OnSlotKeyUpdated(i);
	}
	UpdateButtonIcons();
}

void UW_ListEntry_KeyBinding::OnToggleEditableState(bool bIsEditable)
{
	Super::OnToggleEditableState(bIsEditable);

	if (!bIsEditable)
	{
		CancelListening();
	}

	if (Cbb_KeyboardSlot)
	{
		Cbb_KeyboardSlot->SetIsEnabled(bIsEditable);
	}
	if (Cbb_GamepadSlot)
	{
		Cbb_GamepadSlot->SetIsEnabled(bIsEditable);
	}
}

void UW_ListEntry_KeyBinding::StartListeningForSlot(int32 SlotIndex)
{
	if (!CachedOwningKeyBindingDataObject)
	{
		return;
	}

	if (!CachedOwningKeyBindingDataObject->GetKeyBindingSlots().IsValidIndex(SlotIndex))
	{
		return;
	}

	CancelListening();

	ListeningSlotIndex = SlotIndex;
	BP_OnListeningStateChanged(SlotIndex, true);

	SetKeyboardFocus();
}

void UW_ListEntry_KeyBinding::CancelListening()
{
	if (ListeningSlotIndex == INDEX_NONE)
	{
		return;
	}

	const int32 PreviousSlot = ListeningSlotIndex;
	ListeningSlotIndex = INDEX_NONE;
	BP_OnListeningStateChanged(PreviousSlot, false);
}

FText UW_ListEntry_KeyBinding::GetKeyDisplayNameForSlot(int32 SlotIndex) const
{
	if (CachedOwningKeyBindingDataObject)
	{
		return CachedOwningKeyBindingDataObject->GetCurrentKeyDisplayName(SlotIndex);
	}
	return FText::GetEmpty();
}

FReply UW_ListEntry_KeyBinding::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return HandleListeningKeyInput(InKeyEvent);
}

FReply UW_ListEntry_KeyBinding::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (IsListening())
	{
		const FReply Reply = HandleListeningKeyInput(InKeyEvent);
		if (Reply.IsEventHandled())
		{
			return Reply;
		}
	}
	return FReply::Unhandled();
}

FReply UW_ListEntry_KeyBinding::HandleListeningKeyInput(const FKeyEvent& InKeyEvent)
{
	if (!IsListening())
	{
		return FReply::Unhandled();
	}

	const FKey PressedKey = InKeyEvent.GetKey();

	// Escape always cancels listening.
	if (PressedKey == EKeys::Escape)
	{
		CancelListening();
		return FReply::Handled();
	}

	if (IsValidKeyForListeningSlot(PressedKey))
	{
		if (CachedOwningKeyBindingDataObject)
		{
			CachedOwningKeyBindingDataObject->SetKeyForSlot(ListeningSlotIndex, PressedKey);
		}
		CancelListening();
		return FReply::Handled();
	}

	// Event handled (consumed) but no action taken — prevents fallthrough.
	return FReply::Handled();
}

/** Returns false for modifier/lock keys and keys incompatible with the active slot's input type. */
bool UW_ListEntry_KeyBinding::IsValidKeyForListeningSlot(const FKey& Key) const
{
	if (!Key.IsValid())
	{
		return false;
	}

	static const TArray<FKey> AlwaysExcludedKeys = {
		EKeys::LeftShift, EKeys::RightShift,
		EKeys::LeftControl, EKeys::RightControl,
		EKeys::LeftAlt, EKeys::RightAlt,
		EKeys::LeftCommand, EKeys::RightCommand,
		EKeys::CapsLock, EKeys::NumLock, EKeys::ScrollLock,
		EKeys::Escape,
	};

	if (AlwaysExcludedKeys.Contains(Key))
	{
		return false;
	}

	if (!CachedOwningKeyBindingDataObject)
	{
		return true;
	}

	const TArray<FKeyBindingSlotDescriptor>& Slots = CachedOwningKeyBindingDataObject->GetKeyBindingSlots();
	if (!Slots.IsValidIndex(ListeningSlotIndex))
	{
		return true;
	}

	const FKeyBindingSlotDescriptor& ActiveSlot = Slots[ListeningSlotIndex];
	const bool bIsGamepadKey = Key.IsGamepadKey();

	if (ActiveSlot.bAllowGamepad && !ActiveSlot.bAllowKeyboardAndMouse)
	{
		return bIsGamepadKey;
	}

	if (ActiveSlot.bAllowKeyboardAndMouse && !ActiveSlot.bAllowGamepad)
	{
		return !bIsGamepadKey;
	}

	return true;
}

/**
 * Applies a captured key to the data object the remap was requested for.
 *
 * Only touches the data object and the UI subsystem captured when the remap was requested: the owning
 * entry widget is pooled by the ListView and may have been recycled onto another key binding action,
 * or been destroyed, before this runs.
 */
static void ApplyRemappedKeyForDataObject(const TWeakObjectPtr<UUListDataObject_KeyBinding>& InTargetDataObject,
	const TWeakObjectPtr<UFrontendUISubsystem>& InWeakUISubsystem, const FKey& InPressedKey)
{
	UUListDataObject_KeyBinding* TargetDataObject = InTargetDataObject.Get();
	if (!TargetDataObject)
	{
		return;
	}

	FName ConflictActionName;
	FText ConflictDisplayName;
	if (!TargetDataObject->FindConflictingAction(InPressedKey, ConflictActionName, ConflictDisplayName))
	{
		// No conflict — bind directly.
		TargetDataObject->BindNewInputKey(InPressedKey);
		return;
	}

	UFrontendUISubsystem* UISubsystem = InWeakUISubsystem.Get();
	if (!UISubsystem)
	{
		return;
	}

	UISubsystem->PushConfirmWidgetToModalStackAsync(
		EConfirmScreenType::YesNo,
		LOCTEXT("KeyConflict_Title", "Key Already Bound"),
		FText::Format(
			LOCTEXT("KeyConflict_Message", "'{0}' is already used by '{1}'. Do you want to rebind it?"),
			InPressedKey.GetDisplayName(),
			ConflictDisplayName),
		[InTargetDataObject, InPressedKey](const EConfirmScreenButtonType Result)
		{
			if (Result != EConfirmScreenButtonType::Confirmed)
			{
				return;
			}

			if (UUListDataObject_KeyBinding* ConfirmedDataObject = InTargetDataObject.Get())
			{
				ConfirmedDataObject->BindNewInputKey(InPressedKey);
			}
		});
}

void UW_ListEntry_KeyBinding::OnKeyboardSlotClicked()
{
	if (!CachedOwningKeyBindingDataObject)
	{
		return;
	}

	const TArray<FKeyBindingSlotDescriptor>& Slots = CachedOwningKeyBindingDataObject->GetKeyBindingSlots();
	int32 FoundSlotIndex = INDEX_NONE;
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].bAllowKeyboardAndMouse && !Slots[i].bAllowGamepad)
		{
			FoundSlotIndex = i;
			break;
		}
	}
	if (FoundSlotIndex == INDEX_NONE) return;

	OpenRemapForDataObject(CachedOwningKeyBindingDataObject, ECommonInputType::MouseAndKeyboard);
}

void UW_ListEntry_KeyBinding::OnGamepadSlotClicked()
{
	if (!CachedOwningKeyBindingDataObject) return;

	const TArray<FKeyBindingSlotDescriptor>& Slots = CachedOwningKeyBindingDataObject->GetKeyBindingSlots();
	int32 FoundSlotIndex = INDEX_NONE;
	for (int32 i = 0; i < Slots.Num(); ++i)
	{
		if (Slots[i].bAllowGamepad && !Slots[i].bAllowKeyboardAndMouse)
		{
			FoundSlotIndex = i;
			break;
		}
	}
	if (FoundSlotIndex == INDEX_NONE) return;

	OpenRemapForDataObject(CachedOwningKeyBindingDataObject, ECommonInputType::Gamepad);
}

void UW_ListEntry_KeyBinding::OpenRemapForDataObject(UUListDataObject_KeyBinding* InTargetDataObject,
	ECommonInputType InInputType)
{
	if (!InTargetDataObject)
	{
		return;
	}

	UFrontendUISubsystem* UISubsystem = UFrontendUISubsystem::Get(this);
	if (!UISubsystem) return;

	// Capture the target now, while this entry still owns it: the request below completes asynchronously,
	// and this entry can be recycled onto another key binding action before the callbacks run.
	const TWeakObjectPtr<UUListDataObject_KeyBinding> WeakTargetDataObject = InTargetDataObject;
	const TWeakObjectPtr<UFrontendUISubsystem> WeakUISubsystem = UISubsystem;
	const TWeakObjectPtr<ULocalPlayer> WeakLocalPlayer = GetOwningLocalPlayer();
	const TWeakObjectPtr<APlayerController> WeakOwningPlayer = GetOwningPlayer();

	UISubsystem->PushSoftWidgetToStackAsync(
		FrontendGameplayTags::Frontend_WidgetStack_Modal,
		UFrontendFunctionLibrary::GetFrontendSoftWidgetClassByTag(FrontendGameplayTags::Frontend_Widget_KeyRemap),
		[WeakTargetDataObject, WeakUISubsystem, WeakLocalPlayer, WeakOwningPlayer, InInputType](
			const EAsyncPushWidgetState InPushState, UW_ActivatableBase* PushWidget)
		{
			if (InPushState != EAsyncPushWidgetState::OnCreateBeforePush || !PushWidget)
			{
				return;
			}

			UW_KeyRemapScreen* CreateKeyRemapScreen = CastChecked<UW_KeyRemapScreen>(PushWidget);

			CreateKeyRemapScreen->OnKeyRemapScreenKeyPressed.BindLambda(
				[WeakTargetDataObject, WeakUISubsystem](const FKey& PressedKey)
				{
					ApplyRemappedKeyForDataObject(WeakTargetDataObject, WeakUISubsystem, PressedKey);
				});

			CreateKeyRemapScreen->SetRemapInputType(InInputType, WeakLocalPlayer.Get());

			if (APlayerController* OwningPlayer = WeakOwningPlayer.Get())
			{
				PushWidget->SetOwningPlayer(OwningPlayer);
			}
		});
}

void UW_ListEntry_KeyBinding::UpdateButtonIcons()
{
	if (!CachedOwningKeyBindingDataObject) return;

	Cbb_KeyboardSlot->SetButtonIcon(CachedOwningKeyBindingDataObject->GetKeyboardIconFromCurrentKey());
	Cbb_GamepadSlot->SetButtonIcon(CachedOwningKeyBindingDataObject->GetGamepadIconFromCurrentKey());
}

#undef LOCTEXT_NAMESPACE
