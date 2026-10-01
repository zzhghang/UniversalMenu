// HangHang All Rights Reserved

#include "Widgets/MenuConfig/MenuEntryValueConfig.h"
#include "Widgets/MenuConfig/MenuEntryConfig.h"
#include "GameFramework/GameUserSettings.h"
#include "LocalizationKeys.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringCulture.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"

namespace
{
	void AddWindowModes(UListDataObject_StringEnum* Entry)
	{
		Entry->AddEnumOption(EWindowMode::Fullscreen, TXT_VAL_WINDOW_FULLSCREEN());
		Entry->AddEnumOption(EWindowMode::WindowedFullscreen, TXT_VAL_WINDOW_BORDERLESS());
		Entry->AddEnumOption(EWindowMode::Windowed, TXT_VAL_WINDOW_WINDOWED());
	}
}

#if WITH_EDITOR
void UMenuEntryValueConfig::PreEditChange(FProperty* PropertyAboutToChange)
{
	if (UMenuEntryConfig* Owner = Cast<UMenuEntryConfig>(GetOuter()); Owner && Owner->ValueConfig == this)
	{
		Owner->Modify(); // Include the legacy mirrors in the same undo transaction.
	}
	Super::PreEditChange(PropertyAboutToChange);
}

void UMenuEntryValueConfig::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (UMenuEntryConfig* Owner = Cast<UMenuEntryConfig>(GetOuter()); Owner && Owner->ValueConfig == this)
	{
		Owner->SynchronizeLegacyFields();
	}
}

void UMenuEntryValueConfig::PostEditUndo()
{
	Super::PostEditUndo();
	if (UMenuEntryConfig* Owner = Cast<UMenuEntryConfig>(GetOuter()); Owner && Owner->ValueConfig == this)
	{
		Owner->SynchronizeLegacyFields();
	}
}
#endif

UListDataObject_Base* UMenuBoolValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	UListDataObject_StringBool* Result = NewObject<UListDataObject_StringBool>();
	if (bDefaultValue) Result->SetTrueAsDefaultValue();
	else Result->SetFalseAsDefaultValue();
	return Result;
}

void UMenuBoolValueConfig::ImportLegacy(const UMenuEntryConfig& Entry) { bDefaultValue = Entry.bDefaultBool; }
void UMenuBoolValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const { Entry.bDefaultBool = bDefaultValue; }

UListDataObject_Base* UMenuScalarValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	UListDataObject_Scalar* Result = NewObject<UListDataObject_Scalar>();
	Result->SetDisplayValueRange(TRange<float>(ScalarMin, ScalarMax));
	Result->SetOutValueRange(TRange<float>(ScalarOutMin, ScalarOutMax));
	Result->SetSliderStepSize(StepSize);
	Result->SetDefaultValueFromString(DefaultValue);
	Result->SetCommonNumericType(NumericType);
	Result->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
	return Result;
}

void UMenuScalarValueConfig::ImportLegacy(const UMenuEntryConfig& Entry)
{
	DefaultValue = Entry.DefaultValue;
	ScalarMin = Entry.ScalarMin;
	ScalarMax = Entry.ScalarMax;
	ScalarOutMin = Entry.ScalarOutMin;
	ScalarOutMax = Entry.ScalarOutMax;
	StepSize = Entry.StepSize;
	NumericType = Entry.NumericType;
}

void UMenuScalarValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const
{
	Entry.DefaultValue = DefaultValue;
	Entry.ScalarMin = ScalarMin;
	Entry.ScalarMax = ScalarMax;
	Entry.ScalarOutMin = ScalarOutMin;
	Entry.ScalarOutMax = ScalarOutMax;
	Entry.StepSize = StepSize;
	Entry.NumericType = NumericType;
}

UListDataObject_Base* UMenuStringValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	UListDataObject_String* Result = nullptr;
	if (Entry.BindingKey == TEXT("FullscreenMode"))
	{
		auto* EnumEntry = NewObject<UListDataObject_StringEnum>();
		if (AvailableOptions.IsEmpty()) AddWindowModes(EnumEntry);
		Result = EnumEntry;
	}
	else
	{
		const FString Key = Entry.BindingKey.ToString();
		// Preserve the Custom label and dependency refresh used by legacy quality entries.
		if (Key.Contains(TEXT("Quality")) || Key.Contains(TEXT("Scalability")))
			Result = NewObject<UListDataObject_StringInteger>();
		else
			Result = NewObject<UListDataObject_StringEnum>();
	}
	for (const auto& Pair : AvailableOptions) Result->AddDynamicString(Pair.Key, Pair.Value);
	if (!DefaultValue.IsEmpty()) Result->SetDefaultValueFromString(DefaultValue);
	return Result;
}

void UMenuStringValueConfig::ImportLegacy(const UMenuEntryConfig& Entry)
{
	DefaultValue = Entry.DefaultStringValue;
	AvailableOptions = Entry.AvailableOptions;
}

void UMenuStringValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const
{
	Entry.DefaultStringValue = DefaultValue;
	Entry.AvailableOptions = AvailableOptions;
}

UListDataObject_Base* UMenuIntegerValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	if (Entry.BindingKey == TEXT("FullscreenMode"))
	{
		auto* Result = NewObject<UListDataObject_StringEnum>();
		if (AvailableOptions.IsEmpty()) AddWindowModes(Result);
		else
		{
			for (const auto& Pair : AvailableOptions)
				Result->AddEnumOption(static_cast<EWindowMode::Type>(Pair.Key), Pair.Value);
		}
		Result->SetDefaultValueFromEnumOption(static_cast<EWindowMode::Type>(DefaultValue));
		return Result;
	}

	auto* Result = NewObject<UListDataObject_StringInteger>();
	for (const auto& Pair : AvailableOptions) Result->AddIntegerOption(Pair.Key, Pair.Value);
	Result->SetDefaultValueFromString(LexToString(DefaultValue));
	return Result;
}

void UMenuIntegerValueConfig::ImportLegacy(const UMenuEntryConfig& Entry)
{
	DefaultValue = Entry.DefaultIntValue;
	AvailableOptions = Entry.AvailableIntegerOptions;
}

void UMenuIntegerValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const
{
	Entry.DefaultIntValue = DefaultValue;
	Entry.AvailableIntegerOptions = AvailableOptions;
}

UListDataObject_Base* UMenuCultureValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	auto* Result = NewObject<UListDataObject_StringCulture>();
	if (!DefaultValue.IsEmpty()) Result->SetDefaultValueFromString(DefaultValue);
	return Result;
}

void UMenuCultureValueConfig::ImportLegacy(const UMenuEntryConfig& Entry) { DefaultValue = Entry.DefaultStringValue; }
void UMenuCultureValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const { Entry.DefaultStringValue = DefaultValue; }

UListDataObject_Base* UMenuResolutionValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	auto* Result = NewObject<UListDataObject_StringResolution>();
	Result->InitResolutionValues();
	if (!DefaultValue.IsEmpty()) Result->SetDefaultValueFromString(DefaultValue);
	return Result;
}

void UMenuResolutionValueConfig::ImportLegacy(const UMenuEntryConfig& Entry) { DefaultValue = Entry.DefaultStringValue; }
void UMenuResolutionValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const { Entry.DefaultStringValue = DefaultValue; }

UListDataObject_Base* UMenuEditableValueConfig::CreateDataObject(const UMenuEntryConfig& Entry) const
{
	auto* Result = NewObject<UListDataObject_Editable>();
	if (MaxCharCount > 0) Result->SetMaxCharCount(MaxCharCount);
	Result->SetbIsPassword(bIsPassword);
	Result->SetHintText(HintText);
	Result->SetCharFilter(CharFilter);
	return Result;
}

void UMenuEditableValueConfig::ImportLegacy(const UMenuEntryConfig& Entry)
{
	HintText = Entry.HintText;
	MaxCharCount = Entry.MaxCharCount;
	bIsPassword = Entry.bIsPassword;
	CharFilter = Entry.CharFilter;
}

void UMenuEditableValueConfig::ExportLegacy(UMenuEntryConfig& Entry) const
{
	Entry.HintText = HintText;
	Entry.MaxCharCount = MaxCharCount;
	Entry.bIsPassword = bIsPassword;
	Entry.CharFilter = CharFilter;
}
