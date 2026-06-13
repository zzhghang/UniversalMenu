#pragma once

#include "FrontendStructTypes.generated.h"

/**
 * Describes a condition under which an options entry is non-editable.
 *
 * Used for conditional editing: e.g. VSync only available in Fullscreen,
 * or Resolution only modifiable outside WindowedFullscreen mode.
 * When the condition is not met, the entry is disabled with a rich-text reason
 * and optionally forced to a specific string value.
 */
USTRUCT()
struct FOptionsDataEditConditionDescriptor
{
	GENERATED_BODY()

public:

	void SetEditConditionFunction(TFunction<bool()> InEditConditionFunction) {EditConditionFunction = InEditConditionFunction;}

	bool IsValid() const {return EditConditionFunction != nullptr;}

	/** Returns true if the edit condition is met (entry should be editable). */
	bool IsEditConditionMet() const
	{
		if (IsValid())
		{
			return EditConditionFunction();
		}
		return true;
	}

	FString GetDisableRichReason() const {return DisableRichReason;}

	void SetDisableRichReason(const FString& InDisableRichReason) {DisableRichReason = InDisableRichReason;}

	bool HasDisabledForcedStringValue() const {return DisabledForcedStringValue.IsSet();}

	FString GetDisabledForcedStringValue() const {return DisabledForcedStringValue.GetValue();}

	void SetDisabledForcedStringValue(const FString& InDisabledForcedStringValue) {DisabledForcedStringValue = InDisabledForcedStringValue;}

private:

	TFunction<bool()> EditConditionFunction;

	FString DisableRichReason;

	TOptional<FString> DisabledForcedStringValue;

};

/** Describes a single key binding slot (e.g. keyboard primary, gamepad primary). */
USTRUCT(BlueprintType)
struct FKeyBindingSlotDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName PlayerMappableName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SlotDisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowKeyboardAndMouse = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowGamepad = false;

	/** EPlayerMappableKeySlot value (0=First, 1=Second, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MappableSlot = 0;
};