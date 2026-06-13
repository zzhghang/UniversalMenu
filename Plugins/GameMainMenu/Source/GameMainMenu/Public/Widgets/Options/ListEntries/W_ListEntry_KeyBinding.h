// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "W_ListEntry_KeyBinding.generated.h"

class UUListDataObject_KeyBinding;
class UFrontendCommonButtonBase;

/**
 * List entry widget for key binding rows.
 *
 * Displays two buttons (keyboard + gamepad slots). Clicking a slot opens
 * the UW_KeyRemapScreen overlay to capture a new key. Handles conflict
 * detection and rebinding via UUListDataObject_KeyBinding.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_KeyBinding : public UW_ListEntry_Base
{
	GENERATED_BODY()

public:

	/** Begin listening for a key press on the specified slot. Opens the key remap overlay. */
	UFUNCTION(BlueprintCallable)
	void StartListeningForSlot(int32 SlotIndex);

	/** Cancel the current key listening session. */
	UFUNCTION(BlueprintCallable)
	void CancelListening();

	UFUNCTION(BlueprintPure)
	bool IsListening() const { return ListeningSlotIndex != INDEX_NONE; }

	UFUNCTION(BlueprintPure)
	int32 GetListeningSlotIndex() const { return ListeningSlotIndex; }

	UFUNCTION(BlueprintPure)
	FText GetKeyDisplayNameForSlot(int32 SlotIndex) const;

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Listening State Changed"))
	void BP_OnListeningStateChanged(int32 SlotIndex, bool bIsListening);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Slot Key Updated"))
	void BP_OnSlotKeyUpdated(int32 SlotIndex);

	/** Update button icons based on currently bound keys. Calls SetButtonIcon internally. */
	void UpdateButtonIcons();

protected:

	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject) override;
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData, EOptionsListDataModifyReason OwningModifyReason) override;
	virtual void OnToggleEditableState(bool bIsEditable) override;

private:

	bool IsValidKeyForListeningSlot(const FKey& Key) const;

	FReply HandleListeningKeyInput(const FKeyEvent& InKeyEvent);

	void OnKeyboardSlotClicked();
	void OnGamepadSlotClicked();
	
	void OnKeyRemapPressed(const FKey& PressedKey);
	void OnKeyRemapCancel(const FString& CancelReason);

	UPROPERTY(Transient)
	UUListDataObject_KeyBinding* CachedOwningKeyBindingDataObject;

	int32 ListeningSlotIndex = INDEX_NONE;

	/***** Bind Widget — Keyboard Slot *****/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonButtonBase* Cbb_KeyboardSlot;

	/***** Bind Widget — Gamepad Slot *****/
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UFrontendCommonButtonBase* Cbb_GamepadSlot;
};
