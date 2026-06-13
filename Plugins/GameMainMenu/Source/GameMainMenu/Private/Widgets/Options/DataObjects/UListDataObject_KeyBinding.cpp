// HangHang All Rights Reserved

#include "Widgets/Options/DataObjects/UListDataObject_KeyBinding.h"
#include "LocalizationKeys.h"

#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "CommonInputBaseTypes.h"
#include "CommonInputSubsystem.h"
#include "Framework/Application/SlateApplication.h"
#include "FrontendDebugHelper.h"

void UUListDataObject_KeyBinding::AddKeyBindingSlot(const FKeyBindingSlotDescriptor& InSlot)
{
	KeyBindingSlots.Add(InSlot);
}

FKey UUListDataObject_KeyBinding::GetCurrentKeyForSlot(int32 SlotIndex) const
{
	if (!KeyBindingSlots.IsValidIndex(SlotIndex))
	{
		return EKeys::Invalid;
	}

	UEnhancedInputUserSettings* Settings = GetUserSettings();
	if (!Settings)
	{
		return EKeys::Invalid;
	}

	const FPlayerKeyMapping* Mapping = Settings->FindCurrentMappingForSlot(
		KeyBindingSlots[SlotIndex].PlayerMappableName,
		static_cast<EPlayerMappableKeySlot>(KeyBindingSlots[SlotIndex].MappableSlot));

	if (Mapping)
	{
		return Mapping->GetCurrentKey();
	}

	return EKeys::Invalid;
}

void UUListDataObject_KeyBinding::SetKeyForSlot(int32 SlotIndex, const FKey& NewKey)
{
	if (!KeyBindingSlots.IsValidIndex(SlotIndex) || !NewKey.IsValid())
	{
		return;
	}

	UEnhancedInputUserSettings* Settings = GetUserSettings();
	if (!Settings)
	{
		return;
	}

	FMapPlayerKeyArgs Args;
	Args.MappingName = KeyBindingSlots[SlotIndex].PlayerMappableName;
	Args.NewKey = NewKey;
	Args.Slot = static_cast<EPlayerMappableKeySlot>(KeyBindingSlots[SlotIndex].MappableSlot);

	FGameplayTagContainer FailureReason;
	Settings->MapPlayerKey(Args, FailureReason);

	NotifyListDataModified(this, EOptionsListDataModifyReason::DirectlyModified);
}

void UUListDataObject_KeyBinding::ResetSlotToDefault(int32 SlotIndex)
{
	if (!KeyBindingSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	UEnhancedInputUserSettings* Settings = GetUserSettings();
	if (!Settings)
	{
		return;
	}

	FMapPlayerKeyArgs Args;
	Args.MappingName = KeyBindingSlots[SlotIndex].PlayerMappableName;
	Args.Slot = static_cast<EPlayerMappableKeySlot>(KeyBindingSlots[SlotIndex].MappableSlot);

	FGameplayTagContainer FailureReason;
	Settings->ResetAllPlayerKeysInRow(Args, FailureReason);

	NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
}

FText UUListDataObject_KeyBinding::GetCurrentKeyDisplayName(int32 SlotIndex) const
{
	const FKey Key = GetCurrentKeyForSlot(SlotIndex);
	if (Key.IsValid())
	{
		return Key.GetDisplayName();
	}
	return TXT_VAL_UNBOUND();
}

FSlateBrush UUListDataObject_KeyBinding::GetKeyboardIconFromCurrentKey() const
{
	FSlateBrush Brush;
	const FKey Key = GetKeyboardKey();
	if (Key.IsValid())
	{
		const bool bFound = UCommonInputPlatformSettings::Get()->TryGetInputBrush(Brush, Key, ECommonInputType::MouseAndKeyboard, NAME_None);
	}
	return Brush;
}

FSlateBrush UUListDataObject_KeyBinding::GetGamepadIconFromCurrentKey() const
{
	FSlateBrush Brush;
	const FKey Key = GetGamepadKey();
	if (Key.IsValid())
	{
		FName GamepadName = NAME_None;
		if (CachedOwningLocalPlayer.IsValid())
		{
			if (UCommonInputSubsystem* CommonInput = UCommonInputSubsystem::Get(CachedOwningLocalPlayer.Get()))
			{
				GamepadName = CommonInput->GetCurrentGamepadName();
			}
		}
		const bool bFound = UCommonInputPlatformSettings::Get()->TryGetInputBrush(Brush, Key, ECommonInputType::Gamepad, GamepadName);
	}
	return Brush;
}

FKey UUListDataObject_KeyBinding::GetKeyboardKey() const
{
	for (const FKeyBindingSlotDescriptor& Slot : KeyBindingSlots)
	{
		if (Slot.bAllowKeyboardAndMouse && !Slot.bAllowGamepad)
		{
			return GetCurrentKeyForSlot(&Slot - KeyBindingSlots.GetData());
		}
	}
	return EKeys::Invalid;
}

FKey UUListDataObject_KeyBinding::GetGamepadKey() const
{
	for (const FKeyBindingSlotDescriptor& Slot : KeyBindingSlots)
	{
		if (Slot.bAllowGamepad && !Slot.bAllowKeyboardAndMouse)
		{
			return GetCurrentKeyForSlot(&Slot - KeyBindingSlots.GetData());
		}
	}
	return EKeys::Invalid;
}

void UUListDataObject_KeyBinding::SetOwningLocalPlayer(ULocalPlayer* InLocalPlayer)
{
	CachedOwningLocalPlayer = InLocalPlayer;
}

void UUListDataObject_KeyBinding::BindNewInputKey(const FKey& NewKey)
{
	if (!NewKey.IsValid())
	{
		return;
	}

	const bool bIsGamepadKey = NewKey.IsGamepadKey();

	for (int32 i = 0; i < KeyBindingSlots.Num(); ++i)
	{
		const FKeyBindingSlotDescriptor& Slot = KeyBindingSlots[i];

		if (bIsGamepadKey && Slot.bAllowGamepad && !Slot.bAllowKeyboardAndMouse)
		{
			SetKeyForSlot(i, NewKey);
			return;
		}
		if (!bIsGamepadKey && Slot.bAllowKeyboardAndMouse && !Slot.bAllowGamepad)
		{
			SetKeyForSlot(i, NewKey);
			return;
		}
	}

	// Fallback: bind to any slot
	for (int32 i = 0; i < KeyBindingSlots.Num(); ++i)
	{
		const FKeyBindingSlotDescriptor& Slot = KeyBindingSlots[i];
		if ((bIsGamepadKey && Slot.bAllowGamepad) || (!bIsGamepadKey && Slot.bAllowKeyboardAndMouse))
		{
			SetKeyForSlot(i, NewKey);
			return;
		}
	}
}

bool UUListDataObject_KeyBinding::FindConflictingAction(const FKey& NewKey, FName& OutConflictActionName, FText& OutConflictDisplayName) const
{
	if (!NewKey.IsValid())
	{
		return false;
	}

	UEnhancedInputUserSettings* Settings = GetUserSettings();
	if (!Settings)
	{
		return false;
	}

	UEnhancedPlayerMappableKeyProfile* Profile = Settings->GetActiveKeyProfile();
	if (!Profile)
	{
		return false;
	}

	// Collect our own mapping identities to exclude from conflict check
	TSet<TPair<FName, uint8>> OurMappings;
	for (const FKeyBindingSlotDescriptor& Slot : KeyBindingSlots)
	{
		OurMappings.Add(TPair<FName, uint8>(Slot.PlayerMappableName, Slot.MappableSlot));
	}

	for (const auto& RowPair : Profile->GetPlayerMappingRows())
	{
		for (const FPlayerKeyMapping& Mapping : RowPair.Value.Mappings)
		{
			const TPair<FName, uint8> Identity(Mapping.GetMappingName(), static_cast<uint8>(Mapping.GetSlot()));
			if (OurMappings.Contains(Identity))
			{
				continue;
			}

			if (Mapping.GetCurrentKey() == NewKey)
			{
				OutConflictActionName = Mapping.GetMappingName();
				OutConflictDisplayName = Mapping.GetDisplayName().IsEmpty()
					? FText::FromName(Mapping.GetMappingName())
					: Mapping.GetDisplayName();
				return true;
			}
		}
	}

	return false;
}

bool UUListDataObject_KeyBinding::HasDefaultValue() const
{
	UEnhancedInputUserSettings* Settings = GetUserSettings();
	if (!Settings)
	{
		return true;
	}

	for (const FKeyBindingSlotDescriptor& BindSlot : KeyBindingSlots)
	{
		const FPlayerKeyMapping* Mapping = Settings->FindCurrentMappingForSlot(
			BindSlot.PlayerMappableName,
			static_cast<EPlayerMappableKeySlot>(BindSlot.MappableSlot));

		if (Mapping && Mapping->IsCustomized())
		{
			return false;
		}
	}
	return true;
}

bool UUListDataObject_KeyBinding::CanResetBackToDefaultValue() const
{
	return !HasDefaultValue();
}

bool UUListDataObject_KeyBinding::TryResetBackToDefaultValue()
{
	UEnhancedInputUserSettings* Settings = GetUserSettings();
	if (!Settings)
	{
		return false;
	}

	for (const FKeyBindingSlotDescriptor& BindSlot : KeyBindingSlots)
	{
		FMapPlayerKeyArgs Args;
		Args.MappingName = BindSlot.PlayerMappableName;
		Args.Slot = static_cast<EPlayerMappableKeySlot>(BindSlot.MappableSlot);

		FGameplayTagContainer FailureReason;
		Settings->ResetAllPlayerKeysInRow(Args, FailureReason);
	}

	NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
	return true;
}

void UUListDataObject_KeyBinding::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	bHasDataBeenInitialized = true;
}

UEnhancedInputUserSettings* UUListDataObject_KeyBinding::GetUserSettings() const
{
	if (!CachedOwningLocalPlayer.IsValid())
	{
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(CachedOwningLocalPlayer.Get());
	if (!Subsystem)
	{
		return nullptr;
	}

	return Subsystem->GetUserSettings();
}
