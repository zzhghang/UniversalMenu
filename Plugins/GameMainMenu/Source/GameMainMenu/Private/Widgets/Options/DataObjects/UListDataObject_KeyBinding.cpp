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

	const FPlayerKeyMapping* Mapping = FindMappingForSlot(SlotIndex);

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

	FPlayerKeyMapping* Mapping = FindMappingForSlot(SlotIndex);
	if (!Mapping)
	{
		return;
	}

	Mapping->SetCurrentKey(NewKey);

	NotifyListDataModified(this, EOptionsListDataModifyReason::DirectlyModified);
}

void UUListDataObject_KeyBinding::ResetSlotToDefault(int32 SlotIndex)
{
	if (!KeyBindingSlots.IsValidIndex(SlotIndex))
	{
		return;
	}

	FPlayerKeyMapping* Mapping = FindMappingForSlot(SlotIndex);
	if (!Mapping)
	{
		return;
	}

	Mapping->ResetToDefault();

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

	UEnhancedPlayerMappableKeyProfile* Profile = KeyProfile;
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
	if (!KeyProfile)
	{
		return true;
	}

	for (int32 SlotIndex = 0; SlotIndex < KeyBindingSlots.Num(); ++SlotIndex)
	{
		const FPlayerKeyMapping* Mapping = FindMappingForSlot(SlotIndex);

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
	if (!KeyProfile)
	{
		return false;
	}

	for (int32 SlotIndex = 0; SlotIndex < KeyBindingSlots.Num(); ++SlotIndex)
	{
		if (FPlayerKeyMapping* Mapping = FindMappingForSlot(SlotIndex)) Mapping->ResetToDefault();
	}

	NotifyListDataModified(this, EOptionsListDataModifyReason::ResetToDefault);
	return true;
}

void UUListDataObject_KeyBinding::OnDataObjectInitialized()
{
	Super::OnDataObjectInitialized();

	bHasDataBeenInitialized = true;
}

FPlayerKeyMapping* UUListDataObject_KeyBinding::FindMappingForSlot(int32 SlotIndex) const
{
	if (!KeyProfile || !KeyBindingSlots.IsValidIndex(SlotIndex))
	{
		return nullptr;
	}

	const FKeyBindingSlotDescriptor& Slot = KeyBindingSlots[SlotIndex];
	if (FKeyMappingRow* Row = KeyProfile->FindKeyMappingRowMutable(Slot.PlayerMappableName))
	{
		for (FPlayerKeyMapping& Mapping : Row->Mappings)
		{
			if (Mapping.GetSlot() == static_cast<EPlayerMappableKeySlot>(Slot.MappableSlot)) return &Mapping;
		}
	}
	return nullptr;
}
