// HangHang All Rights Reserved
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Engine/Engine.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsImmediateSettingsTest,
	"GameMainMenu.Options.Immediate.AppliesOnlyEditedSetting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsImmediateSettingsTest::RunTest(const FString& Parameters)
{
	auto* Live = UFrontendGameUserSettings::Get();
	if (!TestNotNull(TEXT("Live settings exist"), Live)) return false;
	const float OriginalGamma = GEngine->GetDisplayGamma();
	const FString OriginalCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
	ON_SCOPE_EXIT
	{
		// Immediate application is exercised on a disposable settings object. Restore runtime effects.
		Live->ApplyNonResolutionSettings();
		GEngine->DisplayGamma = OriginalGamma;
		FInternationalization::Get().SetCurrentCulture(OriginalCulture);
	};
	auto* Target = DuplicateObject<UFrontendGameUserSettings>(Live, GetTransientPackage());
	Target->ScalabilityQuality = Live->ScalabilityQuality;
	Target->SetOverallScalabilityLevel(3);
	Target->SetOverallVolume(1.f);
	Target->SetMusicVolume(1.f);
	Target->SetDifficulty(TEXT("Normal"));
	Target->SetCurrentDisplayGamma(2.2f);
	Target->SetShowMinimap(true);
	auto* Config = NewObject<UMenuConfigData>();
	auto* Tab = NewObject<UMenuTabConfig>(Config);
	Tab->TabId = TEXT("Tab-Video");
	Config->Tabs.Add(Tab);
	auto* Category = NewObject<UMenuCategoryConfig>(Tab);
	Category->CategoryId = TEXT("ImmediateTest");
	Tab->Categories.Add(Category);
	const auto Add = [Category](FName Id, FName Key, bool bImmediate)
	{
		auto* Entry = NewObject<UMenuEntryConfig>(Category);
		Entry->DataId = Id;
		Entry->BindingKey = Key;
		Entry->bApplyImmediately = bImmediate;
		Category->Entries.Add(Entry);
		return Entry;
	};
	const auto AddScalar = [&Add](FName Id, FName Key, bool bImmediate, float Max, const TCHAR* Default)
	{
		auto* Entry = Add(Id, Key, bImmediate);
		auto* Value = NewObject<UMenuScalarValueConfig>(Entry);
		Value->ScalarMax = Value->ScalarOutMax = Max;
		Value->DefaultValue = Default;
		Entry->ValueConfig = Value;
		return Entry;
	};
	AddScalar(TEXT("Gamma"), TEXT("DisplayGamma"), true, 4.f, TEXT("2.2"));
	AddScalar(TEXT("Volume"), TEXT("OverallVolume"), true, 2.f, TEXT("1.0"));
	AddScalar(TEXT("Music"), TEXT("MusicVolume"), false, 2.f, TEXT("1.0"));
	auto* Difficulty = Add(TEXT("Difficulty"), TEXT("Difficulty"), false);
	auto* DifficultyValue = NewObject<UMenuStringValueConfig>(Difficulty);
	Difficulty->ValueConfig = DifficultyValue;
	DifficultyValue->DefaultValue = TEXT("Normal");
	DifficultyValue->AvailableOptions.Add(TEXT("Normal"), FText::FromString(TEXT("Normal")));
	DifficultyValue->AvailableOptions.Add(TEXT("Hard"), FText::FromString(TEXT("Hard")));
	auto* Minimap = Add(TEXT("Minimap"), TEXT("ShowMinimap"), true);
	auto* MinimapValue = NewObject<UMenuBoolValueConfig>(Minimap);
	MinimapValue->bDefaultValue = true;
	Minimap->ValueConfig = MinimapValue;
	for (const TCHAR* Id : {TEXT("OverallQuality"), TEXT("Shadow")})
	{
		const bool bShadow = FString(Id) == TEXT("Shadow");
		auto* Entry = Add(Id, bShadow ? TEXT("ShadowQuality") : TEXT("OverallScalabilityLevel"), bShadow);
		auto* Value = NewObject<UMenuIntegerValueConfig>(Entry);
		Value->DefaultValue = 3;
		for (int32 Level = 0; Level <= 3; ++Level) Value->AvailableOptions.Add(Level, FText::AsNumber(Level));
		Entry->ValueConfig = Value;
		if (bShadow) Entry->EditDependencyIds.Add(TEXT("OverallQuality"));
	}
	auto* Registry = NewObject<UOptionsDataRegistry>();
	Registry->SetOptionsConfig(Config);
	Registry->InitOptionsDataRegistry(nullptr, Target);
	TMap<FName, UListDataObject_Base*> Items;
	for (auto* Item : Registry->GetListSourceItemBySelectedTabId(Tab->TabId)) Items.Add(Item->GetDataId(), Item);
	TestEqual(TEXT("Initialization does not apply runtime gamma"), GEngine->GetDisplayGamma(), OriginalGamma);
	CastChecked<UListDataObject_String>(Items[TEXT("Difficulty")])->OnRotatorInitiatedValueChanged(FText::FromString(TEXT("Hard")));
	CastChecked<UListDataObject_Scalar>(Items[TEXT("Music")])->SetCurrentValue(0.25f);
	CastChecked<UListDataObject_String>(Items[TEXT("OverallQuality")])->OnRotatorInitiatedValueChanged(FText::AsNumber(2));
	TestEqual(TEXT("Deferred preset does not apply immediate shadow via dependency refresh"), Target->GetShadowQuality(), 3);
	CastChecked<UListDataObject_Scalar>(Items[TEXT("Gamma")])->SetCurrentValue(3.f);
	TestEqual(TEXT("Immediate slider transfers only its setting"), Target->GetCurrentDisplayGamma(), 3.f);
	TestEqual(TEXT("Immediate gamma is applied to runtime"), GEngine->GetDisplayGamma(), 3.f);
	TestEqual(TEXT("Deferred music stays unapplied"), Target->GetMusicVolume(), 1.f);
	TestEqual(TEXT("Deferred difficulty stays unapplied"), Target->GetDifficulty(), FString(TEXT("Normal")));
	TestEqual(TEXT("Deferred preset stays unapplied"), Target->GetTextureQuality(), 3);
	CastChecked<UListDataObject_Scalar>(Items[TEXT("Volume")])->SetCurrentValue(0.4f);
	TestEqual(TEXT("Immediate volume updates the target"), Target->GetOverallVolume(), 0.4f);
	CastChecked<UListDataObject_String>(Items[TEXT("Minimap")])->GotoNextOption();
	TestFalse(TEXT("Immediate Bool applies on selection"), Target->GetShowMinimap());
	TestTrue(TEXT("Immediate Bool resets"), Items[TEXT("Minimap")]->TryResetBackToDefaultValue());
	TestTrue(TEXT("Reset immediately restores Bool"), Target->GetShowMinimap());
	TestTrue(TEXT("Immediate slider resets"), Items[TEXT("Gamma")]->TryResetBackToDefaultValue());
	TestEqual(TEXT("Reset immediately restores runtime gamma"), GEngine->GetDisplayGamma(), 2.2f);
	CastChecked<UListDataObject_String>(Items[TEXT("Shadow")])->OnRotatorInitiatedValueChanged(FText::AsNumber(1));
	TestEqual(TEXT("Immediate quality selection applies"), Target->GetShadowQuality(), 1);
	TestEqual(TEXT("Other draft quality values do not leak"), Target->GetTextureQuality(), 3);
	Registry->CommitPendingSettings(Target);
	TestEqual(TEXT("Back transfers deferred music"), Target->GetMusicVolume(), 0.25f);
	TestEqual(TEXT("Back transfers deferred difficulty"), Target->GetDifficulty(), FString(TEXT("Hard")));
	TestEqual(TEXT("Back preserves immediate quality"), Target->GetShadowQuality(), 1);
	TestEqual(TEXT("Back transfers other preset qualities"), Target->GetTextureQuality(), 2);
	TestEqual(TEXT("Back preserves immediate volume"), Target->GetOverallVolume(), 0.4f);
	// A directly edited immediate preset intentionally updates its entire quality group.
	for (auto* Entry : Category->Entries)
		if (Entry->DataId == TEXT("OverallQuality")) Entry->bApplyImmediately = true;
	auto* PresetRegistry = NewObject<UOptionsDataRegistry>();
	PresetRegistry->SetOptionsConfig(Config);
	PresetRegistry->InitOptionsDataRegistry(nullptr, Target);
	for (auto* Item : PresetRegistry->GetListSourceItemBySelectedTabId(Tab->TabId))
		if (Item->GetDataId() == TEXT("OverallQuality"))
			CastChecked<UListDataObject_String>(Item)->OnRotatorInitiatedValueChanged(FText::AsNumber(3));
	TestEqual(TEXT("Immediate preset applies shadow"), Target->GetShadowQuality(), 3);
	TestEqual(TEXT("Immediate preset applies foliage too"), Target->GetFoliageQuality(), 3);
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsContinuousEditCostTest,
	"GameMainMenu.Options.Immediate.ContinuousEditSkipsFullApply",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsContinuousEditCostTest::RunTest(const FString& Parameters)
{
	auto* Live = UFrontendGameUserSettings::Get();
	if (!TestNotNull(TEXT("Live settings exist"), Live)) return false;
	const float OriginalGamma = GEngine->GetDisplayGamma();
	ON_SCOPE_EXIT
	{
		Live->ApplyNonResolutionSettings();
		GEngine->DisplayGamma = OriginalGamma;
	};
	auto* Target = DuplicateObject<UFrontendGameUserSettings>(Live, GetTransientPackage());
	Target->ScalabilityQuality = Live->ScalabilityQuality;
	Target->SetOverallVolume(1.f);
	Target->SetCurrentDisplayGamma(2.2f);
	Target->SetShowMinimap(true);
	auto* Config = NewObject<UMenuConfigData>();
	auto* Tab = NewObject<UMenuTabConfig>(Config);
	Tab->TabId = TEXT("Tab-Video");
	Config->Tabs.Add(Tab);
	auto* Category = NewObject<UMenuCategoryConfig>(Tab);
	Category->CategoryId = TEXT("ContinuousEditTest");
	Tab->Categories.Add(Category);
	const auto Add = [Category](FName Id, FName Key)
	{
		auto* Entry = NewObject<UMenuEntryConfig>(Category);
		Entry->DataId = Id;
		Entry->BindingKey = Key;
		Entry->bApplyImmediately = true;
		Category->Entries.Add(Entry);
		return Entry;
	};
	const auto AddScalar = [&Add](FName Id, FName Key, float Max, const TCHAR* Default)
	{
		auto* Entry = Add(Id, Key);
		auto* Value = NewObject<UMenuScalarValueConfig>(Entry);
		Value->ScalarMax = Value->ScalarOutMax = Max;
		Value->DefaultValue = Default;
		Entry->ValueConfig = Value;
		return Entry;
	};
	AddScalar(TEXT("Gamma"), TEXT("DisplayGamma"), 4.f, TEXT("2.2"));
	AddScalar(TEXT("Volume"), TEXT("OverallVolume"), 2.f, TEXT("1.0"));
	auto* Minimap = Add(TEXT("Minimap"), TEXT("ShowMinimap"));
	auto* MinimapValue = NewObject<UMenuBoolValueConfig>(Minimap);
	MinimapValue->bDefaultValue = true;
	Minimap->ValueConfig = MinimapValue;

	auto* Registry = NewObject<UOptionsDataRegistry>();
	Registry->SetOptionsConfig(Config);
	Registry->InitOptionsDataRegistry(nullptr, Target);
	TMap<FName, UListDataObject_Base*> Items;
	for (auto* Item : Registry->GetListSourceItemBySelectedTabId(Tab->TabId)) Items.Add(Item->GetDataId(), Item);

	// Only a full apply validates and repairs settings, so a corrupt resolution reports whether one ran.
	const FIntPoint CorruptResolution(622989440, 407);
	Target->SetScreenResolution(CorruptResolution);

	CastChecked<UListDataObject_Scalar>(Items[TEXT("Gamma")])->SetCurrentValue(3.f);
	TestEqual(TEXT("Continuous gamma edit is committed"), Target->GetCurrentDisplayGamma(), 3.f);
	TestEqual(TEXT("Continuous gamma edit still reaches the runtime"), GEngine->GetDisplayGamma(), 3.f);
	TestEqual(TEXT("Continuous gamma edit does not run a full apply"), Target->GetScreenResolution(), CorruptResolution);

	CastChecked<UListDataObject_Scalar>(Items[TEXT("Volume")])->SetCurrentValue(0.4f);
	TestEqual(TEXT("Continuous volume edit is committed"), Target->GetOverallVolume(), 0.4f);
	TestEqual(TEXT("Continuous volume edit does not run a full apply"), Target->GetScreenResolution(), CorruptResolution);

	// Positive control: settings without a targeted effect behave as before.
	Target->SetShowMinimap(true);
	CastChecked<UListDataObject_String>(Items[TEXT("Minimap")])->GotoNextOption();
	TestFalse(TEXT("Immediate bool is still applied"), Target->GetShowMinimap());
	TestNotEqual(TEXT("Settings without a targeted effect still run a full apply"), Target->GetScreenResolution(), CorruptResolution);
	return true;
}
#endif
