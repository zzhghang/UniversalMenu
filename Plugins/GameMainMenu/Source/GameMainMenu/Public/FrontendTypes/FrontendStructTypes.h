#pragma once

#include "FrontendStructTypes.generated.h"

USTRUCT()
struct FOptionsDataEditConditionDescriptor
{
	GENERATED_BODY()

public:

	void SetEditConditionFunction(TFunction<bool()> InEditConditionFunction) {EditConditionFunction = InEditConditionFunction;}
	
	bool IsValid() const {return EditConditionFunction != nullptr;}

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

	// EPlayerMappableKeySlot value (0=First, 1=Second, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 MappableSlot = 0;
};