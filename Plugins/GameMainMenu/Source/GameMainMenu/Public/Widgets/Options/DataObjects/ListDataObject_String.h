// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Value.h"
#include "ListDataObject_String.generated.h"

/**
 * 
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_String : public UListDataObject_Value
{
	GENERATED_BODY()
	
public:

	void AddDynamicString(const FString& InString, const FText& InText);

	void GotoPreOption();
	
	void GotoNextOption();

	void OnRotatorInitiatedValueChanged(const FText& InNewSelectedText);

	FORCEINLINE const TArray<FText>& GetOptionsTextArray() const { return TextArray;}
	
	FORCEINLINE const FText& GetCurrentTextValue() const { return CurrentTextValue;}

	FORCEINLINE const FString& GetCurrentStringValue() const { return CurrentStringValue;}
	
protected:
	
	// ~Begin UListDataObject_Base Interface
	virtual void OnDataObjectInitialized() override;
	
	virtual bool CanResetBackToDefaultValue() const override;
	
	virtual bool TryResetBackToDefaultValue() override;
	
	virtual bool CanSetToForcedStringValue(const FString& InForcedStringValue) const override;
	
	virtual void OnSetForcedStringValue(const FString& InForcedStringValue) override;
	// ~End UListDataObject_Base Interface
	
	bool TrySetDisplayTextFromStringValue(const FString& InStringValue);
	
protected:
	
	/** Current value stored in the config. */
	FString CurrentStringValue;
	/** Current display text shown in the UI. */
	FText CurrentTextValue;

	/** Values saved to configuration (internal representation). */
	TArray<FString> StringArray;
	/** Display texts shown in UI (localized). */
	TArray<FText> TextArray;
	
};

UCLASS()
class GAMEMAINMENU_API UListDataObject_StringBool : public UListDataObject_String
{
	GENERATED_BODY()
	
public:

	void OverrideTrueDisplayText(const FText& InTrueText);
	void OverrideFalseDisplayText(const FText& InFalseText);

	void SetTrueAsDefaultValue();
	void SetFalseAsDefaultValue();

protected:

	// ~Begin UListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	// ~End UListDataObject_String Interface

private:

	void TryInitializeBoolOptions();

	const FString OnString = TEXT("true");
	const FString OffString = TEXT("false");
};

UCLASS()
class GAMEMAINMENU_API UListDataObject_StringEnum : public UListDataObject_String
{
	GENERATED_BODY()

public:

	template <typename EnumType>
	void AddEnumOption(EnumType InEnumValue,const FText& InDisplayText)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString EnumString = StaticEnumOption->GetNameStringByValue(InEnumValue);

		AddDynamicString(EnumString,InDisplayText);
	}

	template <typename EnumType>
	EnumType GetCurrentValueAsEnum() const
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const int64 EnumValue = StaticEnumOption->GetValueByNameString(CurrentStringValue);
		if (!ensureMsgf(EnumValue != INDEX_NONE, TEXT("GetCurrentValueAsEnum: CurrentStringValue '%s' is not a valid name for enum %s"),
			*CurrentStringValue, *StaticEnumOption->GetName()))
		{
			return EnumType();
		}
		return static_cast<EnumType>(EnumValue);
	}

	template <typename EnumType>
	void SetDefaultValueFromEnumOption(EnumType InEnumValue)
	{
		const UEnum* StaticEnumOption = StaticEnum<EnumType>();
		const FString EnumString = StaticEnumOption->GetNameStringByValue(InEnumValue);

		SetDefaultValueFromString(EnumString);
	}
};


UCLASS()
class GAMEMAINMENU_API UListDataObject_StringInteger : public UListDataObject_String
{
	GENERATED_BODY()

public:

	void AddIntegerOption(int32 InIntegerValue, const FText& InDisplayText);

protected:

	// ~Begin UListDataObject_String Interface
	virtual void OnDataObjectInitialized() override;
	virtual void OnEditDependencyDataModified(UListDataObject_Base* ModifyDependencyData, EOptionsListDataModifyReason ModifyReason) override;
	// ~End UListDataObject_String Interface
};
