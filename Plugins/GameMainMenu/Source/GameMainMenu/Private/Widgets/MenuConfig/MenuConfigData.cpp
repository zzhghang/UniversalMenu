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

static void ApplyCommonConfig(UListDataObject_Base* Entry, const UMenuEntryConfig* Data)
{
	Entry->SetDynamicDetailsRichText(Data->DynamicDetails);
	if (!Data->DisableRichReason.IsEmpty())
		Entry->SetDisableRichText(Data->DisableRichReason);
}

// ============================================================================
// CreateEntryFromConfig
// ============================================================================

UListDataObject_Base* CreateEntryFromConfig(const UMenuEntryConfig* Data)
{
	if (!Data) return nullptr;

	UListDataObject_Base* Entry = nullptr;

	switch (Data->ValueType)
	{
	case EMenuEntryValueType::Bool:
		{
			UListDataObject_StringBool* TypedEntry = NewObject<UListDataObject_StringBool>();
			TypedEntry->SetDataId(Data->DataId);
			TypedEntry->SetDataDisplayName(Data->DisplayName);
			TypedEntry->SetDescriptionRichText(Data->Description);
			TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			if (Data->bDefaultBool)
				TypedEntry->SetTrueAsDefaultValue();
			else
				TypedEntry->SetFalseAsDefaultValue();
			Entry = TypedEntry;
		}
		break;

	case EMenuEntryValueType::Scalar:
		{
			UListDataObject_Scalar* TypedEntry = NewObject<UListDataObject_Scalar>();
			TypedEntry->SetDataId(Data->DataId);
			TypedEntry->SetDataDisplayName(Data->DisplayName);
			TypedEntry->SetDescriptionRichText(Data->Description);
			TypedEntry->SetDisplayValueRange(TRange<float>(Data->ScalarMin, Data->ScalarMax));
			TypedEntry->SetOutValueRange(TRange<float>(Data->ScalarOutMin, Data->ScalarOutMax));
			TypedEntry->SetSliderStepSize(Data->StepSize);
			TypedEntry->SetDefaultValueFromString(Data->DefaultValue);
			TypedEntry->SetCommonNumericType(Data->NumericType);
			TypedEntry->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			Entry = TypedEntry;
		}
		break;

	case EMenuEntryValueType::String:
		{
			if (Data->BindingKey == "FullscreenMode")
			{
				UListDataObject_StringEnum* TypedEntry = NewObject<UListDataObject_StringEnum>();
				TypedEntry->SetDataId(Data->DataId);
				TypedEntry->SetDataDisplayName(Data->DisplayName);
				TypedEntry->SetDescriptionRichText(Data->Description);
				// ponytail: window mode options hardcoded — same 3 modes every platform uses
				TypedEntry->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));
				TypedEntry->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));
				TypedEntry->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
				TypedEntry->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				Entry = TypedEntry;
			}
			else if (IsQualityBindingKey(Data->BindingKey))
			{
				UListDataObject_StringInteger* TypedEntry = NewObject<UListDataObject_StringInteger>();
				TypedEntry->SetDataId(Data->DataId);
				TypedEntry->SetDataDisplayName(Data->DisplayName);
				TypedEntry->SetDescriptionRichText(Data->Description);
				TypedEntry->AddIntegerOption(0, FText::FromString(TEXT("Low")));
				TypedEntry->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
				TypedEntry->AddIntegerOption(2, FText::FromString(TEXT("High")));
				TypedEntry->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
				TypedEntry->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
				if (!Data->DefaultStringValue.IsEmpty())
					TypedEntry->SetDefaultValueFromString(Data->DefaultStringValue);
				else
					TypedEntry->SetDefaultValueFromString(TEXT("3"));
				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				Entry = TypedEntry;
			}
			else
			{
				UListDataObject_StringEnum* TypedEntry = NewObject<UListDataObject_StringEnum>();
				TypedEntry->SetDataId(Data->DataId);
				TypedEntry->SetDataDisplayName(Data->DisplayName);
				TypedEntry->SetDescriptionRichText(Data->Description);
				for (const auto& Pair : Data->AvailableOptions)
				{
					TypedEntry->AddDynamicString(Pair.Key, Pair.Value);
				}
				if (!Data->DefaultStringValue.IsEmpty()) { TypedEntry->SetDefaultValueFromString(Data->DefaultStringValue); }
				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				Entry = TypedEntry;
			}
		}
		break;

	case EMenuEntryValueType::Integer:
		{
			if (Data->BindingKey == "FullscreenMode")
			{
				UListDataObject_StringEnum* TypedEntry = NewObject<UListDataObject_StringEnum>();
				TypedEntry->SetDataId(Data->DataId);
				TypedEntry->SetDataDisplayName(Data->DisplayName);
				TypedEntry->SetDescriptionRichText(Data->Description);
				TypedEntry->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));
				TypedEntry->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));
				TypedEntry->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));
				TypedEntry->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				Entry = TypedEntry;
			}
			else if (IsQualityBindingKey(Data->BindingKey))
			{
				UListDataObject_StringInteger* TypedEntry = NewObject<UListDataObject_StringInteger>();
				TypedEntry->SetDataId(Data->DataId);
				TypedEntry->SetDataDisplayName(Data->DisplayName);
				TypedEntry->SetDescriptionRichText(Data->Description);
				// ponytail: quality levels hardcoded — same 5 levels every preset uses
				TypedEntry->AddIntegerOption(0, FText::FromString(TEXT("Low")));
				TypedEntry->AddIntegerOption(1, FText::FromString(TEXT("Medium")));
				TypedEntry->AddIntegerOption(2, FText::FromString(TEXT("High")));
				TypedEntry->AddIntegerOption(3, FText::FromString(TEXT("Epic")));
				TypedEntry->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));
				const int32 DefaultVal = Data->DefaultIntValue != 0 ? Data->DefaultIntValue : 3;
				TypedEntry->SetDefaultValueFromString(LexToString(DefaultVal));
				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				Entry = TypedEntry;
			}
			else
			{
				UListDataObject_StringInteger* TypedEntry = NewObject<UListDataObject_StringInteger>();
				TypedEntry->SetDataId(Data->DataId);
				TypedEntry->SetDataDisplayName(Data->DisplayName);
				TypedEntry->SetDescriptionRichText(Data->Description);
				for (const auto& Pair : Data->AvailableIntegerOptions)
				{
					TypedEntry->AddIntegerOption(Pair.Key, Pair.Value);
				}
				if (Data->DefaultIntValue != 0 || !Data->AvailableIntegerOptions.IsEmpty())
				{
					TypedEntry->SetDefaultValueFromString(LexToString(Data->DefaultIntValue));
				}
				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
				Entry = TypedEntry;
			}
		}
		break;

	case EMenuEntryValueType::Culture:
		{
			UListDataObject_StringCulture* TypedEntry = NewObject<UListDataObject_StringCulture>();
			TypedEntry->SetDataId(Data->DataId);
			TypedEntry->SetDataDisplayName(Data->DisplayName);
			TypedEntry->SetDescriptionRichText(Data->Description);
			if (!Data->DefaultStringValue.IsEmpty()) { TypedEntry->SetDefaultValueFromString(Data->DefaultStringValue); }
			TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			Entry = TypedEntry;
		}
		break;

	case EMenuEntryValueType::Resolution:
		{
			UListDataObject_StringResolution* TypedEntry = NewObject<UListDataObject_StringResolution>();
			TypedEntry->SetDataId(Data->DataId);
			TypedEntry->SetDataDisplayName(Data->DisplayName);
			TypedEntry->SetDescriptionRichText(Data->Description);
			TypedEntry->InitResolutionValues();
			if (!Data->DefaultStringValue.IsEmpty()) { TypedEntry->SetDefaultValueFromString(Data->DefaultStringValue); }
			TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			Entry = TypedEntry;
		}
		break;

	case EMenuEntryValueType::Editable:
		{
			UListDataObject_Editable* TypedEntry = NewObject<UListDataObject_Editable>();
			TypedEntry->SetDataId(Data->DataId);
			TypedEntry->SetDataDisplayName(Data->DisplayName);
			TypedEntry->SetDescriptionRichText(Data->Description);
			if (Data->MaxCharCount > 0) TypedEntry->SetMaxCharCount(Data->MaxCharCount);
			TypedEntry->SetbIsPassword(Data->bIsPassword);
			TypedEntry->SetHintText(Data->HintText);
			TypedEntry->SetCharFilter(Data->CharFilter);
			Entry = TypedEntry;
		}
		break;

	default:
		break;
	}

	if (Entry)
	{
		ApplyCommonConfig(Entry, Data);
	}

	return Entry;
}

UListDataObject_Collection* CreateCollectionFromConfig(const FName& Id, const FText& DisplayName)
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}
