// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "FrontendTypes/FrontendStructTypes.h"
#include "UListDataObject_KeyBinding.generated.h"

class UEnhancedInputUserSettings;

/**
 * Data object for a key binding entry in the options control tab.
 *
 * Wraps one or more FKeyBindingSlotDescriptor (e.g. keyboard + gamepad slots)
 * and delegates to UEnhancedInputUserSettings for key mapping operations.
 */
UCLASS()
class GAMEMAINMENU_API UUListDataObject_KeyBinding : public UListDataObject_Base
{
	GENERATED_BODY()

public:

	/** Add a key binding slot for a specific input type (keyboard or gamepad). */
	void AddKeyBindingSlot(const FKeyBindingSlotDescriptor& InSlot);

	const TArray<FKeyBindingSlotDescriptor>& GetKeyBindingSlots() const { return KeyBindingSlots; }

	FKey GetCurrentKeyForSlot(int32 SlotIndex) const;

	void SetKeyForSlot(int32 SlotIndex, const FKey& NewKey);

	void ResetSlotToDefault(int32 SlotIndex);

	FText GetCurrentKeyDisplayName(int32 SlotIndex) const;

	FSlateBrush GetKeyboardIconFromCurrentKey() const;

	FSlateBrush GetGamepadIconFromCurrentKey() const;

	/** Returns the first keyboard/mouse slot's current key, or EKeys::Invalid */
	UFUNCTION(BlueprintPure)
	FKey GetKeyboardKey() const;

	/** Returns the first gamepad slot's current key, or EKeys::Invalid */
	UFUNCTION(BlueprintPure)
	FKey GetGamepadKey() const;

	void SetOwningLocalPlayer(ULocalPlayer* InLocalPlayer);

	/** Bind NewKey to the first compatible slot (gamepad or keyboard). */
	void BindNewInputKey(const FKey& NewKey);

	/** Check if NewKey is already bound to a different action. Returns the conflicting action's info. */
	bool FindConflictingAction(const FKey& NewKey, FName& OutConflictActionName, FText& OutConflictDisplayName) const;

	ULocalPlayer* GetOwningLocalPlayer() const { return CachedOwningLocalPlayer.Get(); }

	// ~Begin UListDataObject_Base Interface
	virtual bool HasDefaultValue() const override;
	virtual bool CanResetBackToDefaultValue() const override;
	virtual bool TryResetBackToDefaultValue() override;
	// ~End UListDataObject_Base Interface

protected:

	virtual void OnDataObjectInitialized() override;

private:

	UEnhancedInputUserSettings* GetUserSettings() const;

	UPROPERTY(Transient)
	TArray<FKeyBindingSlotDescriptor> KeyBindingSlots;

	TWeakObjectPtr<ULocalPlayer> CachedOwningLocalPlayer;

	bool bHasDataBeenInitialized = false;
};
