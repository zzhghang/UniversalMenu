// HangHang All Rights Reserved

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "LocalizationKeys.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

namespace
{
	struct FBindingTestSettingsBase
	{
		int32 Quality = 2;
		int32 GetQuality() const { return Quality; }
	};

	// Inherited getter and derived setter exercise separate member-pointer class types.
	struct FBindingTestSettings : FBindingTestSettingsBase
	{
		float Volume = 0.5f;
		bool bEnabled = true;
		FString Culture = TEXT("en");
		EWindowMode::Type WindowMode = EWindowMode::Windowed;
		FIntPoint Resolution = FIntPoint(1920, 1080);
		int32 SetterCalls = 0;

		void SetQuality(int32 Value) { Quality = Value; ++SetterCalls; }
		float GetVolume() const { return Volume; }
		void SetVolume(float Value) { Volume = FMath::Clamp(Value, 0.f, 2.f); ++SetterCalls; }
		bool IsEnabled() const { return bEnabled; }
		void SetEnabled(bool Value) { bEnabled = Value; ++SetterCalls; }
		const FString& GetCulture() const { return Culture; }
		void SetCulture(const FString& Value) { Culture = Value; ++SetterCalls; }
		EWindowMode::Type GetWindowMode() const { return WindowMode; }
		void SetWindowMode(EWindowMode::Type Value) { WindowMode = Value; ++SetterCalls; }
		FIntPoint GetResolution() const { return Resolution; }
		void SetResolution(FIntPoint Value) { Resolution = Value; ++SetterCalls; }
	};

	struct FFrameRateTestSettings
	{
		float FrameRate = 60.f;
		int32 SetterCalls = 0;
		float GetFrameRate() const { return FrameRate; }
		void SetFrameRate(float Value) { FrameRate = Value; ++SetterCalls; }
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsTypedBindingTest,
	"GameMainMenu.Options.Binding.TypedAccessors",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsTypedBindingTest::RunTest(const FString& Parameters)
{
	FBindingTestSettings Settings;
	FBindingTestSettings* Target = &Settings;
	const auto GetSettings = [&Target]() { return Target; };
	const auto Quality = FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::GetQuality, &FBindingTestSettings::SetQuality);
	const auto Volume = FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::GetVolume, &FBindingTestSettings::SetVolume);
	const auto Enabled = FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::IsEnabled, &FBindingTestSettings::SetEnabled);
	const auto Culture = FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::GetCulture, &FBindingTestSettings::SetCulture);

	TestEqual(TEXT("Integer getter"), Quality->GetValueAsString(), FString(TEXT("2")));
	TestEqual(TEXT("Float getter"), Volume->GetValueAsString(), LexToString(0.5f));
	TestEqual(TEXT("Bool getter retains true/false representation"), Enabled->GetValueAsString(), FString(TEXT("true")));
	TestEqual(TEXT("String reference getter"), Culture->GetValueAsString(), FString(TEXT("en")));
	TestEqual(TEXT("Creating and reading bindings does not write"), Settings.SetterCalls, 0);

	Quality->SetValueFromString(TEXT(" -1 "));
	Volume->SetValueFromString(TEXT("1.25e0"));
	Enabled->SetValueFromString(TEXT("false"));
	Culture->SetValueFromString(TEXT("zh-Hans"));
	TestEqual(TEXT("Integer setter accepts the custom-quality sentinel"), Settings.Quality, -1);
	TestEqual(TEXT("Float setter accepts scientific notation"), Settings.Volume, 1.25f);
	TestFalse(TEXT("Bool setter"), Settings.bEnabled);
	TestEqual(TEXT("String reference setter"), Settings.Culture, FString(TEXT("zh-Hans")));
	TestEqual(TEXT("All original setters execute"), Settings.SetterCalls, 4);

	for (const TCHAR* Invalid : { TEXT(""), TEXT("invalid"), TEXT("2junk"), TEXT("1.5"), TEXT("2147483648"), TEXT("-2147483649") })
	{
		Quality->SetValueFromString(Invalid);
	}
	for (const TCHAR* Invalid : { TEXT(""), TEXT("invalid"), TEXT("1.2junk"), TEXT("nan"), TEXT("inf"), TEXT("1e100") })
	{
		Volume->SetValueFromString(Invalid);
	}
	TestEqual(TEXT("Invalid numbers do not call setters"), Settings.SetterCalls, 4);
	TestEqual(TEXT("Invalid integer leaves settings intact"), Settings.Quality, -1);
	TestEqual(TEXT("Invalid float leaves settings intact"), Settings.Volume, 1.25f);

	Volume->SetValueFromString(TEXT("3.0"));
	TestEqual(TEXT("Original setter still clamps the value"), Settings.Volume, 2.f);
	Quality->SetValueFromString(TEXT("0"));
	Volume->SetValueFromString(TEXT("0"));
	Culture->SetValueFromString(TEXT(""));
	TestEqual(TEXT("Valid zero integer is written"), Settings.Quality, 0);
	TestEqual(TEXT("Valid zero float is written"), Settings.Volume, 0.f);
	TestTrue(TEXT("Empty string is a valid string value"), Settings.Culture.IsEmpty());

	const int32 CallsBeforeMissingTarget = Settings.SetterCalls;
	Target = nullptr;
	TestTrue(TEXT("Missing target reads safely"), Quality->GetValueAsString().IsEmpty());
	Quality->SetValueFromString(TEXT("3"));
	TestEqual(TEXT("Missing target skips writes"), Settings.SetterCalls, CallsBeforeMissingTarget);
	Target = &Settings;
	TestEqual(TEXT("Target is resolved again on each call"), Quality->GetValueAsString(), FString(TEXT("0")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsDataObjectBindingTest,
	"GameMainMenu.Options.Binding.DataObjectReadWriteReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsDataObjectBindingTest::RunTest(const FString& Parameters)
{
	FBindingTestSettings Settings;
	const auto GetSettings = [&Settings]() { return &Settings; };
	UListDataObject_StringBool* Toggle = NewObject<UListDataObject_StringBool>();
	Toggle->SetShouldApplyChangeImmediately(false);
	Toggle->SetBinding(FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::IsEnabled, &FBindingTestSettings::SetEnabled));
	Toggle->SetTrueAsDefaultValue();
	Toggle->InitializeDataObject();
	TestEqual(TEXT("Toggle initializes from its binding"), Toggle->GetCurrentStringValue(), FString(TEXT("true")));
	TestEqual(TEXT("Initialization does not call setters"), Settings.SetterCalls, 0);
	Toggle->GotoNextOption();
	TestFalse(TEXT("Cycling the toggle writes through its binding"), Settings.bEnabled);
	UListDataObject_Base* ToggleBase = Toggle;
	TestTrue(TEXT("Changed toggle can reset"), ToggleBase->CanResetBackToDefaultValue());
	TestTrue(TEXT("Toggle reset succeeds"), ToggleBase->TryResetBackToDefaultValue());
	TestTrue(TEXT("Toggle reset writes the default"), Settings.bEnabled);

	UListDataObject_Scalar* Slider = NewObject<UListDataObject_Scalar>();
	Slider->SetShouldApplyChangeImmediately(false);
	Slider->SetBinding(FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::GetVolume, &FBindingTestSettings::SetVolume));
	Slider->SetDisplayValueRange(TRange<float>(0.f, 100.f));
	Slider->SetOutValueRange(TRange<float>(0.f, 2.f));
	Slider->SetDefaultValueFromString(TEXT("1.0"));
	TestEqual(TEXT("Slider maps the bound value into its display range"), Slider->GetCurrentValue(), 25.f);
	Slider->SetCurrentValue(75.f);
	TestEqual(TEXT("Slider writes in the settings range"), Settings.Volume, 1.5f);
	UListDataObject_Base* SliderBase = Slider;
	TestTrue(TEXT("Changed slider can reset"), SliderBase->CanResetBackToDefaultValue());
	TestTrue(TEXT("Slider reset succeeds"), SliderBase->TryResetBackToDefaultValue());
	TestEqual(TEXT("Slider reset writes the default"), Settings.Volume, 1.f);
	TestFalse(TEXT("Reset slider no longer needs reset"), SliderBase->CanResetBackToDefaultValue());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsDisplayBindingTest,
	"GameMainMenu.Options.Binding.WindowModeAndResolution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsDisplayBindingTest::RunTest(const FString& Parameters)
{
	FBindingTestSettings Settings;
	const auto GetSettings = [&Settings]() { return &Settings; };
	const auto WindowMode = FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::GetWindowMode, &FBindingTestSettings::SetWindowMode);
	const auto Resolution = FOptionsDataInteractionHelper::Create(GetSettings,
		&FBindingTestSettings::GetResolution, &FBindingTestSettings::SetResolution);

	TestEqual(TEXT("Window mode uses its enum name"), WindowMode->GetValueAsString(), FString(TEXT("Windowed")));
	TestEqual(TEXT("Resolution retains its asset format"), Resolution->GetValueAsString(), FString(TEXT("(X=1920,Y=1080)")));
	TestEqual(TEXT("Reading does not write settings"), Settings.SetterCalls, 0);
	for (const TCHAR* Mode : { TEXT("Fullscreen"), TEXT("WindowedFullscreen"), TEXT("Windowed") })
	{
		WindowMode->SetValueFromString(Mode);
		TestEqual(TEXT("Window mode round trip"), WindowMode->GetValueAsString(), FString(Mode));
	}
	Resolution->SetValueFromString(TEXT("(X=2560,Y=1440)"));
	TestEqual(TEXT("Resolution setter receives the parsed dimensions"), Settings.Resolution, FIntPoint(2560, 1440));
	TestEqual(TEXT("Resolution round trip"), Resolution->GetValueAsString(), FString(TEXT("(X=2560,Y=1440)")));
	TestEqual(TEXT("Valid values call the original setters"), Settings.SetterCalls, 4);

	for (const TCHAR* Invalid : { TEXT(""), TEXT("UnknownMode"), TEXT("999"), TEXT("EWindowMode_MAX") })
	{
		WindowMode->SetValueFromString(Invalid);
	}
	for (const TCHAR* Invalid : { TEXT(""), TEXT("1920x1080"), TEXT("(X=1920)"), TEXT("(X=1920,Y=1080"),
		TEXT("(X=0,Y=1080)"), TEXT("(X=1920,Y=-1)"), TEXT("(X=1920,Y=1080)junk"),
		TEXT("(X=1.5,Y=1080)"), TEXT("(X=2147483648,Y=1080)") })
	{
		Resolution->SetValueFromString(Invalid);
	}
	TestEqual(TEXT("Invalid display values never call setters"), Settings.SetterCalls, 4);
	TestEqual(TEXT("Invalid window mode leaves settings intact"), Settings.WindowMode, EWindowMode::Windowed);
	TestEqual(TEXT("Invalid resolution leaves settings intact"), Settings.Resolution, FIntPoint(2560, 1440));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFrameRateOptionCompatibilityTest,
	"GameMainMenu.Options.Binding.FrameRateOptionCompatibility",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFrameRateOptionCompatibilityTest::RunTest(const FString& Parameters)
{
	for (const TCHAR* Suffix : { TEXT(""), TEXT(".0"), TEXT(".000000") })
	{
		UMenuEntryConfig* Config = NewObject<UMenuEntryConfig>();
		Config->ValueType = EMenuEntryValueType::String;
		Config->BindingKey = TEXT("FrameRateLimit");
		Config->DefaultStringValue = TEXT("0.0");
		Config->AvailableOptions.Add(FString(TEXT("0")) + Suffix, FText::FromString(TEXT("Unlimited")));
		Config->AvailableOptions.Add(FString(TEXT("30")) + Suffix, FText::FromString(TEXT("30 FPS")));
		Config->AvailableOptions.Add(TEXT("59.94"), FText::FromString(TEXT("59.94 FPS")));
		Config->AvailableOptions.Add(FString(TEXT("60")) + Suffix, FText::FromString(TEXT("60 FPS")));
		Config->AvailableOptions.Add(FString(TEXT("120")) + Suffix, FText::FromString(TEXT("120 FPS")));
		auto* Entry = Cast<UListDataObject_String>(CreateEntryFromConfig(Config));
		if (!TestNotNull(TEXT("Frame rate entry is created from the existing asset schema"), Entry)) return false;
		UListDataObject_Base* Base = Entry;
		FFrameRateTestSettings Settings;
		Entry->SetBinding(FOptionsDataInteractionHelper::Create([&Settings]() { return &Settings; },
			&FFrameRateTestSettings::GetFrameRate, &FFrameRateTestSettings::SetFrameRate));

		Entry->InitializeDataObject();
		TestEqual(TEXT("Float getter matches the configured 60 FPS label"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("60 FPS")));
		TestEqual(TEXT("Initialization preserves the bound value"), Entry->GetCurrentStringValue(), LexToString(60.f));
		TestEqual(TEXT("Initialization does not write settings"), Settings.SetterCalls, 0);
		Entry->SetDefaultValueFromString(TEXT("60.0"));
		TestFalse(TEXT("Equivalent numeric default does not require reset"), Base->CanResetBackToDefaultValue());
		Entry->SetDefaultValueFromString(Config->DefaultStringValue);

		// Use the configured order, including when TMap iteration is not numerically sorted.
		const TArray<FText>& Labels = Entry->GetOptionsTextArray();
		const int32 InitialIndex = Labels.IndexOfByPredicate([](const FText& Label) { return Label.ToString() == TEXT("60 FPS"); });
		if (!TestTrue(TEXT("60 FPS has an option index"), InitialIndex != INDEX_NONE)) return false;
		Entry->GotoNextOption();
		TestEqual(TEXT("Next starts at the numerically matched option"), Entry->GetCurrentTextValue().ToString(), Labels[(InitialIndex + 1) % Labels.Num()].ToString());
		TestEqual(TEXT("Next writes once"), Settings.SetterCalls, 1);
		Entry->GotoPreOption();
		TestEqual(TEXT("Previous returns to 60 FPS"), Settings.FrameRate, 60.f);
		Entry->InitializeDataObject();
		Entry->GotoPreOption();
		TestEqual(TEXT("Previous also handles the float getter's representation"), Entry->GetCurrentTextValue().ToString(), Labels[(InitialIndex + Labels.Num() - 1) % Labels.Num()].ToString());

		Entry->OnRotatorInitiatedValueChanged(FText::FromString(TEXT("120 FPS")));
		TestEqual(TEXT("Rotator selection writes the numeric value"), Settings.FrameRate, 120.f);
		Entry->InitializeDataObject();
		TestEqual(TEXT("Reopening retains the selected label"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("120 FPS")));
		TestTrue(TEXT("Reset succeeds with a differently formatted default"), Base->TryResetBackToDefaultValue());
		TestEqual(TEXT("Reset displays Unlimited"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("Unlimited")));
		TestEqual(TEXT("Reset writes zero"), Settings.FrameRate, 0.f);
		Entry->InitializeDataObject();
		TestEqual(TEXT("Saved zero displays Unlimited on reopening"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("Unlimited")));
		TestFalse(TEXT("Saved zero and default 0.0 are equivalent"), Base->CanResetBackToDefaultValue());

		Entry->SetValueFromString(TEXT("59.940000"));
		TestEqual(TEXT("Fractional frame rates match without rounding to integers"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("59.94 FPS")));
		TestEqual(TEXT("Fractional frame rate is preserved"), Settings.FrameRate, 59.94f);
		Settings.FrameRate = 60.01f;
		Entry->InitializeDataObject();
		TestEqual(TEXT("An unlisted rate does not silently match 60 FPS"), Entry->GetCurrentTextValue().ToString(), TXT_VAL_INVALID().ToString());

		// A malformed numeric key must not match the valid unlimited value (zero).
		Config->AvailableOptions.Empty();
		Config->AvailableOptions.Add(TEXT("not-a-number"), FText::FromString(TEXT("Bad option")));
		auto* InvalidEntry = CastChecked<UListDataObject_String>(CreateEntryFromConfig(Config));
		InvalidEntry->InitializeDataObject();
		TestEqual(TEXT("Invalid option key is not converted to zero"), InvalidEntry->GetCurrentTextValue().ToString(), TXT_VAL_INVALID().ToString());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStringOptionMatchingIsolationTest,
	"GameMainMenu.Options.Binding.StringOptionMatchingIsolation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStringOptionMatchingIsolationTest::RunTest(const FString& Parameters)
{
	UMenuEntryConfig* Config = NewObject<UMenuEntryConfig>();
	Config->ValueType = EMenuEntryValueType::String;
	Config->DefaultStringValue = TEXT("01");
	Config->AvailableOptions.Add(TEXT("01"), FText::FromString(TEXT("Map 01")));
	Config->AvailableOptions.Add(TEXT("1"), FText::FromString(TEXT("Map 1")));
	auto* Entry = CastChecked<UListDataObject_String>(CreateEntryFromConfig(Config));
	Entry->InitializeDataObject();
	Entry->SetValueFromString(TEXT("1"));
	TestEqual(TEXT("Ordinary string keys retain distinct labels"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("Map 1")));
	UListDataObject_Base* Base = Entry;
	TestTrue(TEXT("Ordinary string values 01 and 1 remain different"), Base->CanResetBackToDefaultValue());
	Entry->SetDefaultValueFromString(TEXT("1.0"));
	Entry->InitializeDataObject();
	TestEqual(TEXT("Numeric fallback is disabled for ordinary string entries"), Entry->GetCurrentTextValue().ToString(), TXT_VAL_INVALID().ToString());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
