// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/OptionsConfigData.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringCulture.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "LocalizationKeys.h"

TSharedPtr<FOptionsDataInteractionHelper> CreateHelperFromEnum(EGameUserSettingFunction Func)
{
	if (Func == EGameUserSettingFunction::None) return nullptr;

	const UEnum* Enum = StaticEnum<EGameUserSettingFunction>();
	const FString FuncName = Enum->GetNameStringByValue(static_cast<int64>(Func));
	return MakeShared<FOptionsDataInteractionHelper>(FuncName);
}

UListDataObject_Base* CreateEntryFromData(UOptionEntryData* Data)
{
	if (!Data) return nullptr;

	const TSharedPtr<FOptionsDataInteractionHelper> Getter = CreateHelperFromEnum(Data->GetterFunc);
	const TSharedPtr<FOptionsDataInteractionHelper> Setter = CreateHelperFromEnum(Data->SetterFunc);

	switch (Data->ValueType)
	{
	case EOptionValueType::Bool:
		{
			UListDataObject_StringBool* Entry = NewObject<UListDataObject_StringBool>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			if (Data->DefaultValue.Equals(TEXT("true"), ESearchCase::IgnoreCase))
				Entry->SetTrueAsDefaultValue();
			else
				Entry->SetFalseAsDefaultValue();
			return Entry;
		}

	case EOptionValueType::Scalar:
		{
			UListDataObject_Scalar* Entry = NewObject<UListDataObject_Scalar>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->SetDisplayValueRange(TRange<float>(Data->ScalarMin, Data->ScalarMax));
			Entry->SetOutValueRange(TRange<float>(Data->ScalarOutMin, Data->ScalarOutMax));
			Entry->SetSliderStepSize(Data->StepSize);
			Entry->SetDefaultValueFromString(Data->DefaultValue);
			Entry->SetCommonNumericType(Data->NumericType);
			Entry->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::String:
	case EOptionValueType::Enum:
		{
			// Normalize numeric keys so "0.0" matches LexToString(0.0f) = "0.000000"
			auto IsNumericString = [](const FString& Str) -> bool
			{
				if (Str.IsEmpty()) return false;
				for (const TCHAR Ch : Str)
				{
					if (!FChar::IsDigit(Ch) && Ch != '.' && Ch != '-')
						return false;
				}
				return true;
			};
			auto NormalizeKey = [&](const FString& Key) -> FString
			{
				return IsNumericString(Key) ? LexToString(FCString::Atof(*Key)) : Key;
			};

			FString NormalizedDefault = NormalizeKey(Data->DefaultValue);

			UListDataObject_String* Entry = NewObject<UListDataObject_String>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			for (const auto& Pair : Data->AvailableOptions)
			{
				Entry->AddDynamicString(NormalizeKey(Pair.Key), Pair.Value);
			}
			Entry->SetDefaultValueFromString(NormalizedDefault);
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::QualityPreset:
		{
			UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->AddIntegerOption(0, TXT_VAL_QUALITY_LOW());
			Entry->AddIntegerOption(1, TXT_VAL_QUALITY_MEDIUM());
			Entry->AddIntegerOption(2, TXT_VAL_QUALITY_HIGH());
			Entry->AddIntegerOption(3, TXT_VAL_QUALITY_EPIC());
			Entry->AddIntegerOption(4, TXT_VAL_QUALITY_CINEMATIC());
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::Culture:
		{
			UListDataObject_StringCulture* Entry = NewObject<UListDataObject_StringCulture>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->SetDefaultValueFromString(Data->DefaultValue);
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::Resolution:
		{
			UListDataObject_StringResolution* Entry = NewObject<UListDataObject_StringResolution>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->InitResolutionValues();
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	default:
		return nullptr;
	}
}

UListDataObject_Collection* CreateCollectionFromData(const FName& Id, const FText& DisplayName)
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}
