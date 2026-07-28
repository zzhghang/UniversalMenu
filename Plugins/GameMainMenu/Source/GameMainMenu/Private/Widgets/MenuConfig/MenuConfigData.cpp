// HangHang All Rights Reserved


#include "Widgets/MenuConfig/MenuConfigData.h"

#include "GameFramework/GameUserSettings.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringCulture.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"

static bool IsQualityBindingKey(const FName& Key)
{
	const FString S = Key.ToString();
	return S.Contains(TEXT("Quality")) || S.Contains(TEXT("Scalability"));
}

// ============================================================================
// CreateEntryFromConfig
// ============================================================================

UListDataObject_Base* CreateEntryFromConfig(const UMenuEntryConfig* Data)
{
	if (!Data) return nullptr;

	switch (Data->ValueType)
	{
	case EMenuEntryValueType::Bool:
		{
			UListDataObject_StringBool* Entry = NewObject<UListDataObject_StringBool>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			if (Data->bDefaultBool)
				Entry->SetTrueAsDefaultValue();
			else
				Entry->SetFalseAsDefaultValue();
			return Entry;
		}

	case EMenuEntryValueType::Scalar:
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
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EMenuEntryValueType::String:
		{
			// BindingKey discriminates subtypes:
			//   "FullscreenMode"   -> UListDataObject_StringEnum (window mode)
			//   *Quality*/*Scalability* -> UListDataObject_StringInteger (quality presets)
			//   Otherwise -> UListDataObject_StringEnum (plain dropdown)

			if (Data->BindingKey == "FullscreenMode")
			{
				UListDataObject_StringEnum* Entry = NewObject<UListDataObject_StringEnum>();
				Entry->SetDataId(Data->DataId);
				Entry->SetDataDisplayName(Data->DisplayName);
				Entry->SetDescriptionRichText(Data->Description);
				// ponytail: window mode options hardcoded — same 3 modes every platform uses
				Entry->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));
				Entry->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));
				Entry->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
				Entry->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
				Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				return Entry;
			}

			if (IsQualityBindingKey(Data->BindingKey))
			{
				UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
				Entry->SetDataId(Data->DataId);
				Entry->SetDataDisplayName(Data->DisplayName);
				Entry->SetDescriptionRichText(Data->Description);
				Entry->AddIntegerOption(0, FText::FromString(TEXT("Low")));
				Entry->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
				Entry->AddIntegerOption(2, FText::FromString(TEXT("High")));
				Entry->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
				Entry->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
				if (!Data->DefaultStringValue.IsEmpty())
					Entry->SetDefaultValueFromString(Data->DefaultStringValue);
				else
					Entry->SetDefaultValueFromString(TEXT("3"));
				Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				return Entry;
			}

			// Generic string dropdown
			{
				UListDataObject_StringEnum* Entry = NewObject<UListDataObject_StringEnum>();
				Entry->SetDataId(Data->DataId);
				Entry->SetDataDisplayName(Data->DisplayName);
				Entry->SetDescriptionRichText(Data->Description);
				for (const auto& Pair : Data->AvailableOptions)
				{
					Entry->AddDynamicString(Pair.Key, Pair.Value);
				}
				if (!Data->DefaultStringValue.IsEmpty()) { Entry->SetDefaultValueFromString(Data->DefaultStringValue); }
				Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				return Entry;
			}
		}

	case EMenuEntryValueType::Integer:
		{
			// BindingKey discriminates subtypes:
			//   "FullscreenMode" -> UListDataObject_StringEnum (window mode)
			//   *Quality*/*Scalability* -> UListDataObject_StringInteger (quality presets)
			//   Otherwise -> UListDataObject_StringInteger (custom / empty options)

			if (Data->BindingKey == "FullscreenMode")
			{
				UListDataObject_StringEnum* Entry = NewObject<UListDataObject_StringEnum>();
				Entry->SetDataId(Data->DataId);
				Entry->SetDataDisplayName(Data->DisplayName);
				Entry->SetDescriptionRichText(Data->Description);
				Entry->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));
				Entry->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));
				Entry->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
				Entry->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
				Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				return Entry;
			}

			if (IsQualityBindingKey(Data->BindingKey))
			{
				UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
				Entry->SetDataId(Data->DataId);
				Entry->SetDataDisplayName(Data->DisplayName);
				Entry->SetDescriptionRichText(Data->Description);
				// ponytail: quality levels hardcoded — same 5 levels every preset uses
				Entry->AddIntegerOption(0, FText::FromString(TEXT("Low")));
				Entry->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
				Entry->AddIntegerOption(2, FText::FromString(TEXT("High")));
				Entry->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
				Entry->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
				const int32 DefaultVal = Data->DefaultIntValue != 0 ? Data->DefaultIntValue : 3;
				Entry->SetDefaultValueFromString(LexToString(DefaultVal));
				Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				return Entry;
			}

			// Generic integer dropdown (e.g. MaxPlayers)
			{
				UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
				Entry->SetDataId(Data->DataId);
				Entry->SetDataDisplayName(Data->DisplayName);
				Entry->SetDescriptionRichText(Data->Description);
				for (const auto& Pair : Data->AvailableIntegerOptions)
				{
					Entry->AddIntegerOption(Pair.Key, Pair.Value);
				}
				if (Data->DefaultIntValue != 0 || !Data->AvailableIntegerOptions.IsEmpty())
				{
					Entry->SetDefaultValueFromString(LexToString(Data->DefaultIntValue));
				}
				Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				return Entry;
			}
		}

	case EMenuEntryValueType::Culture:
		{
			UListDataObject_StringCulture* Entry = NewObject<UListDataObject_StringCulture>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			if (!Data->DefaultStringValue.IsEmpty()) { Entry->SetDefaultValueFromString(Data->DefaultStringValue); }
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EMenuEntryValueType::Resolution:
		{
			UListDataObject_StringResolution* Entry = NewObject<UListDataObject_StringResolution>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->InitResolutionValues();
			if (!Data->DefaultStringValue.IsEmpty()) { Entry->SetDefaultValueFromString(Data->DefaultStringValue); }
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EMenuEntryValueType::Editable:
		{
			UListDataObject_Editable* Entry = NewObject<UListDataObject_Editable>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			if (Data->MaxCharCount > 0) Entry->SetMaxCharCount(Data->MaxCharCount);
			Entry->SetbIsPassword(Data->bIsPassword);
			Entry->SetHintText(Data->HintText);
			Entry->SetCharFilter(Data->CharFilter);
			return Entry;
		}

	default:
		return nullptr;
	}
}

UListDataObject_Collection* CreateCollectionFromConfig(const FName& Id, const FText& DisplayName)
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}
