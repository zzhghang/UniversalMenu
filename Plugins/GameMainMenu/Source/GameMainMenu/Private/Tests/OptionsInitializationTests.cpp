// HangHang All Rights Reserved

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsInitialSettingsTest,
	"GameMainMenu.Options.Initialization.ReadsExistingSettings",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsInitialSettingsTest::RunTest(const FString& Parameters)
{
	const UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();
	if (!TestNotNull(TEXT("Frontend settings are available"), Settings)) return false;

	UMenuConfigData* Config = NewObject<UMenuConfigData>();
	UMenuTabConfig* Tab = NewObject<UMenuTabConfig>(Config);
	Tab->TabId = TEXT("InitializationTest");
	Config->Tabs.Add(Tab);
	UMenuCategoryConfig* Category = NewObject<UMenuCategoryConfig>(Tab);
	Category->CategoryId = TEXT("Settings");
	Tab->Categories.Add(Category);

	const auto AddEntry = [Category](const TCHAR* Id, EMenuEntryValueType Type, const TCHAR* Binding)
	{
		UMenuEntryConfig* Entry = NewObject<UMenuEntryConfig>(Category);
		Entry->DataId = Id;
		Entry->ValueType = Type;
		Entry->BindingKey = Binding;
		Entry->bApplyImmediately = false;
		Category->Entries.Add(Entry);
		return Entry;
	};

	// Use different asset defaults without modifying or saving the user's settings.
	const FString SavedDifficulty = Settings->GetDifficulty();
	UMenuEntryConfig* Difficulty = AddEntry(TEXT("Difficulty"), EMenuEntryValueType::String, TEXT("Difficulty"));
	Difficulty->DefaultStringValue = SavedDifficulty + TEXT("_AssetDefault");
	Difficulty->AvailableOptions.Add(SavedDifficulty, FText::FromString(SavedDifficulty));
	Difficulty->AvailableOptions.Add(Difficulty->DefaultStringValue, FText::FromString(TEXT("Asset default")));

	const bool bSavedDamageNumbers = Settings->GetShowDamageNumbers();
	UMenuEntryConfig* DamageNumbers = AddEntry(TEXT("DamageNumbers"), EMenuEntryValueType::Bool, TEXT("ShowDamageNumbers"));
	DamageNumbers->bDefaultBool = !bSavedDamageNumbers;

	const int32 SavedShadowQuality = Settings->GetShadowQuality();
	UMenuEntryConfig* Shadow = AddEntry(TEXT("Shadow"), EMenuEntryValueType::Integer, TEXT("ShadowQuality"));
	Shadow->DefaultIntValue = SavedShadowQuality == 3 ? 1 : 3;

	const FString SavedCulture = Settings->GetCurrentCulture();
	UMenuEntryConfig* Culture = AddEntry(TEXT("Culture"), EMenuEntryValueType::Culture, TEXT("CurrentCulture"));
	Culture->DefaultStringValue = SavedCulture == TEXT("en") ? TEXT("fr") : TEXT("en");

	UMenuEntryConfig* Unbound = AddEntry(TEXT("Unbound"), EMenuEntryValueType::String, TEXT("None"));
	Unbound->DefaultStringValue = TEXT("AssetDefault");
	Unbound->AvailableOptions.Add(TEXT("Other"), FText::FromString(TEXT("Other")));
	Unbound->AvailableOptions.Add(TEXT("AssetDefault"), FText::FromString(TEXT("Asset default")));

	UOptionsDataRegistry* Registry = NewObject<UOptionsDataRegistry>();
	Registry->SetOptionsConfig(Config);
	Registry->InitOptionsDataRegistry(nullptr);

	const TMap<FName, FString> ExpectedValues = {
		{ TEXT("Difficulty"), SavedDifficulty },
		{ TEXT("DamageNumbers"), LexToString(bSavedDamageNumbers) },
		{ TEXT("Shadow"), LexToString(SavedShadowQuality) },
		{ TEXT("Culture"), SavedCulture },
		{ TEXT("Unbound"), TEXT("AssetDefault") }
	};
	int32 CheckedEntries = 0;
	for (UListDataObject_Base* Item : Registry->GetListSourceItemBySelectedTabId(Tab->TabId))
	{
		if (const UListDataObject_String* Value = Cast<UListDataObject_String>(Item))
		{
			const FString* Expected = ExpectedValues.Find(Value->GetDataId());
			if (TestNotNull(TEXT("Entry has an expected value"), Expected))
			{
				TestEqual(Value->GetDataId().ToString(), Value->GetCurrentStringValue(), *Expected);
				++CheckedEntries;
			}
		}
	}
	TestEqual(TEXT("All configured values were checked"), CheckedEntries, ExpectedValues.Num());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FResolutionInitialSettingsTest,
	"GameMainMenu.Options.Initialization.ResolutionReadsBinding",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FResolutionInitialSettingsTest::RunTest(const FString& Parameters)
{
	UListDataObject_StringResolution* Resolution = NewObject<UListDataObject_StringResolution>();
	Resolution->AddDynamicString(TEXT("(X=1280,Y=720)"), FText::FromString(TEXT("1280 x 720")));
	Resolution->AddDynamicString(TEXT("(X=1920,Y=1080)"), FText::FromString(TEXT("1920 x 1080")));
	Resolution->SetDefaultValueFromString(TEXT("(X=1280,Y=720)"));
	int32 SetterCalls = 0;
	const TSharedPtr<FOptionsDataInteractionHelper> Binding = MakeShared<FOptionsDataInteractionHelper>(
		[]() { return FString(TEXT("(X=1920,Y=1080)")); },
		[&SetterCalls](const FString&) { ++SetterCalls; });
	Resolution->SetBinding(Binding);
	UListDataObject_Collection* Category = NewObject<UListDataObject_Collection>();
	Category->AddChildListData(Resolution);

	TestEqual(TEXT("Resolution uses the bound value"), Resolution->GetCurrentStringValue(), FString(TEXT("(X=1920,Y=1080)")));
	TestEqual(TEXT("Resolution displays the bound value"), Resolution->GetCurrentTextValue().ToString(), FString(TEXT("1920 x 1080")));
	TestEqual(TEXT("Initialization does not write settings"), SetterCalls, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FResolutionFallbackTest,
	"GameMainMenu.Options.Initialization.ResolutionFallbacks",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FResolutionFallbackTest::RunTest(const FString& Parameters)
{
	auto* Resolution = NewObject<UListDataObject_StringResolution>();
	Resolution->InitResolutionValues({}, FIntPoint(2560, 1440));
	Resolution->InitializeDataObject();
	TestEqual(TEXT("Empty enumeration uses desktop fallback"), Resolution->GetCurrentTextValue().ToString(), FString(TEXT("2560 x 1440")));
	TestEqual(TEXT("Maximum getter returns a value, not a field name"), Resolution->GetMaxAllowedResolution(), FString(TEXT("(X=2560,Y=1440)")));
	Resolution->InitResolutionValues({}, FIntPoint::ZeroValue);
	Resolution->InitializeDataObject();
	TestEqual(TEXT("Headless fallback is usable"), Resolution->GetCurrentTextValue().ToString(), FString(TEXT("1920 x 1080")));
	Resolution->InitResolutionValues({FIntPoint(1920,1080), FIntPoint(1280,720), FIntPoint(1920,1080), FIntPoint(622989440,407), FIntPoint(0,0)}, FIntPoint::ZeroValue);
	TestEqual(TEXT("Invalid and duplicate modes removed"), Resolution->GetOptionsTextArray().Num(), 2);
	TestEqual(TEXT("Maximum is selected from actual modes"), Resolution->GetMaxAllowedResolution(), FString(TEXT("(X=1920,Y=1080)")));
	Resolution->SetDefaultValueFromString(TEXT("MaxAllowedResolution"));
	int32 SetterCalls = 0;
	FString BoundValue = TEXT("(X=622989440,Y=407)");
	Resolution->SetBinding(MakeShared<FOptionsDataInteractionHelper>([&BoundValue]() { return BoundValue; }, [&SetterCalls](const FString&) { ++SetterCalls; }));
	Resolution->InitializeDataObject();
	TestEqual(TEXT("Corrupt resolution cannot become a display label"), Resolution->GetCurrentTextValue().ToString(), FString(TEXT("1920 x 1080")));
	TestEqual(TEXT("Displaying fallback never writes settings"), SetterCalls, 0);
	BoundValue = TEXT("(X=1537,Y=911)");
	Resolution->InitializeDataObject();
	TestEqual(TEXT("Custom editor window reads draft, not global settings"), Resolution->GetCurrentTextValue().ToString(), FString(TEXT("1537 x 911")));
	Resolution->GotoNextOption();
	Resolution->GotoPreOption();
	TestEqual(TEXT("Custom window size participates in navigation"), Resolution->GetCurrentStringValue(), BoundValue);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FResolutionRepairTest,
	"GameMainMenu.Options.Initialization.CorruptResolutionRepair",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FResolutionRepairTest::RunTest(const FString& Parameters)
{
	auto* Settings = NewObject<UFrontendGameUserSettings>();
	Settings->SetScreenResolution(FIntPoint(2560,1440));
	Settings->ConfirmVideoMode();
	Settings->SetScreenResolution(FIntPoint(622989440,407));
	Settings->RepairResolutionSettings();
	TestEqual(TEXT("Corrupt persisted dimensions use last confirmed resolution"), Settings->GetScreenResolution(), FIntPoint(2560,1440));
	Settings->SetScreenResolution(FIntPoint(1537,911));
	Settings->RepairResolutionSettings();
	TestEqual(TEXT("Valid custom window size is preserved"), Settings->GetScreenResolution(), FIntPoint(1537,911));
	const auto Binding = FOptionsDataInteractionHelper::Create([Settings]() { return Settings; }, &UFrontendGameUserSettings::GetScreenResolution, &UFrontendGameUserSettings::SetScreenResolution);
	for (const TCHAR* Invalid : {TEXT("MaxAllowedResolution"), TEXT("(X=622989440,Y=407)"), TEXT("(X=0,Y=1080)"), TEXT("(X=1920,Y=-1)"), TEXT("(X=2147483648,Y=1080)")})
		Binding->SetValueFromString(Invalid);
	TestEqual(TEXT("Binding refuses malformed and corrupt dimensions"), Settings->GetScreenResolution(), FIntPoint(1537,911));
	Binding->SetValueFromString(TEXT("(X=1920,Y=1080)"));
	TestEqual(TEXT("Binding still accepts valid dimensions"), Settings->GetScreenResolution(), FIntPoint(1920,1080));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
