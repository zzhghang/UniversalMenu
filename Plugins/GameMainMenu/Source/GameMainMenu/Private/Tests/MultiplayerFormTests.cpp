// HangHang All Rights Reserved
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Multiplayer/MultiplayerDataRegistry.h"
#include "Widgets/Multiplayer/MenuSessionData.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Session.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

namespace
{
	struct FFormFixture
	{
		UMenuConfigData* Config = NewObject<UMenuConfigData>();
		UMultiplayerDataRegistry* Registry = NewObject<UMultiplayerDataRegistry>();
		UMenuCategoryConfig* Category;
		FFormFixture()
		{
			Config->bPreviewSessionsInEditor = false;
			Category = AddTab(TEXT("CreateGame"));
		}
		UMenuCategoryConfig* AddTab(FName Id)
		{
			auto* Tab = NewObject<UMenuTabConfig>(Config);
			Tab->TabId = Id;
			Config->Tabs.Add(Tab);
			auto* Cat = NewObject<UMenuCategoryConfig>(Tab);
			Cat->CategoryId = TEXT("General");
			Tab->Categories.Add(Cat);
			return Cat;
		}
		template<class T> UMenuEntryConfig* Add(FName Id, UMenuCategoryConfig* Cat = nullptr)
		{
			if (!Cat) Cat = Category;
			auto* Entry = NewObject<UMenuEntryConfig>(Cat);
			Entry->DataId = Id;
			Entry->ValueConfig = NewObject<T>(Entry);
			Cat->Entries.Add(Entry);
			return Entry;
		}
		void Init() { Registry->SetConfig(Config); Registry->InitRegistry(); }
		UListDataObject_Base* Item(FName Id, FName Tab = TEXT("CreateGame")) { return Registry->FindEntry(Tab, Id); }
		FString Value(FName Id, FName Tab = TEXT("CreateGame"))
		{
			FString Result;
			if (auto* Found = Item(Id, Tab)) Found->TryGetValueAsString(Result);
			return Result;
		}
	};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiplayerPersistenceTest, "GameMainMenu.Multiplayer.Persistence", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMultiplayerPersistenceTest::RunTest(const FString& Parameters)
{
	FFormFixture F;
	F.Add<UMenuEditableValueConfig>(TEXT("GameName"));
	auto* Password = F.Add<UMenuEditableValueConfig>(TEXT("Password"));
	Password->Persistence = EMenuPersistence::SessionOnly;
	auto* Unstored = F.Add<UMenuEditableValueConfig>(TEXT("Unstored"));
	Unstored->Persistence = EMenuPersistence::None;
	auto* Map = F.Add<UMenuStringValueConfig>(TEXT("Map"));
	auto* MapValue = CastChecked<UMenuStringValueConfig>(Map->ValueConfig);
	MapValue->DefaultValue = TEXT("Map_01");
	MapValue->AvailableOptions.Add(TEXT("Map_01"), FText::FromString(TEXT("Map One")));
	MapValue->AvailableOptions.Add(TEXT("Map_02"), FText::FromString(TEXT("Map Two")));
	auto* Slider = F.Add<UMenuScalarValueConfig>(TEXT("Scale"));
	auto* Scalar = CastChecked<UMenuScalarValueConfig>(Slider->ValueConfig);
	Scalar->ScalarMin = 0; Scalar->ScalarMax = 100;
	Scalar->ScalarOutMin = 0.5f; Scalar->ScalarOutMax = 2;
	Scalar->DefaultValue = TEXT("1.0");
	F.Init();
	TestFalse(TEXT("Valid form"), F.Registry->HasConfigurationErrors());
	auto* Save = NewObject<UMenuSessionData>();
	Save->SetSavedValue(TEXT("GameName"), TEXT("Old room"));
	Save->SetSavedValue(TEXT("Password"), TEXT("Old password"));
	F.Registry->LoadFormValues(Save);
	TestEqual(TEXT("Loads legacy field"), F.Value(TEXT("GameName")), FString(TEXT("Old room")));
	TestEqual(TEXT("Session-only ignores disk values"), F.Value(TEXT("Password")), FString());
	F.Item(TEXT("GameName"))->TrySetValueFromString(TEXT(""));
	F.Item(TEXT("Password"))->TrySetValueFromString(TEXT("123456"));
	F.Item(TEXT("Unstored"))->TrySetValueFromString(TEXT("temporary"));
	CastChecked<UListDataObject_Scalar>(F.Item(TEXT("Scale")))->SetCurrentValue(50);
	F.Item(TEXT("Map"))->TrySetValueFromString(TEXT("Map_02"));
	TestTrue(TEXT("Saves all supported values"), F.Registry->SaveFormValues(Save));
	FString Empty;
	TestTrue(TEXT("Empty value is present"), Save->TryGetSavedValue(TEXT("GameName"), Empty));
	TestTrue(TEXT("Clearing input removes old text"), Empty.IsEmpty());
	TestFalse(TEXT("Password removed from persistent map"), Save->SavedValues.Contains(TEXT("Password")));
	TestEqual(TEXT("Session value remains available to Blueprint getters"), Save->GetSavedValue(TEXT("Password")), FString(TEXT("123456")));
	TestFalse(TEXT("Unstored field omitted"), Save->TryGetSavedValue(TEXT("Unstored"), Empty));
	TestEqual(TEXT("Scalar stores output value, not display percentage"), FCString::Atof(*Save->GetSavedValue(TEXT("Scale"))), 1.25f);
	TArray<uint8> Bytes;
	TestTrue(TEXT("Serializes save in memory"), UGameplayStatics::SaveGameToMemory(Save, Bytes));
	auto* Reloaded = Cast<UMenuSessionData>(UGameplayStatics::LoadGameFromMemory(Bytes));
	if (!TestNotNull(TEXT("Deserializes save"), Reloaded)) return false;
	TestTrue(TEXT("Session value never reaches serialized save"), Reloaded->GetSavedValue(TEXT("Password")).IsEmpty());
	F.Init();
	F.Registry->LoadFormValues(Reloaded);
	TestEqual(TEXT("Empty text survives reload"), F.Value(TEXT("GameName")), FString());
	TestEqual(TEXT("Choice stores key"), F.Value(TEXT("Map")), FString(TEXT("Map_02")));
	TestEqual(TEXT("Slider restores mapped display"), CastChecked<UListDataObject_Scalar>(F.Item(TEXT("Scale")))->GetCurrentValue(), 50.f);
	Reloaded->SetSavedValue(TEXT("Map"), TEXT("DeletedMap"));
	Reloaded->SetSavedValue(TEXT("Scale"), TEXT("not-a-number"));
	F.Registry->LoadFormValues(Reloaded);
	TestEqual(TEXT("Deleted option falls back to configured default"), F.Value(TEXT("Map")), FString(TEXT("Map_01")));
	TestEqual(TEXT("Invalid number falls back to configured default"), FCString::Atof(*F.Value(TEXT("Scale"))), 1.f);
	TestTrue(TEXT("Scalar clamps out of range saves"), F.Item(TEXT("Scale"))->TrySetValueFromString(TEXT("999")));
	TestEqual(TEXT("Clamped scalar"), FCString::Atof(*F.Value(TEXT("Scale"))), 2.f);
	TestTrue(TEXT("Unbound scalar resets"), F.Item(TEXT("Scale"))->TryResetBackToDefaultValue());
	F.Item(TEXT("Map"))->TrySetValueFromString(TEXT("Map_02"));
	TestTrue(TEXT("Unbound choice resets"), F.Item(TEXT("Map"))->TryResetBackToDefaultValue());
	Save->SetSavedValue(TEXT("InvalidInt"), TEXT("garbage"));
	TestEqual(TEXT("Invalid int honors fallback"), Save->GetSavedValueAsInt(TEXT("InvalidInt"), 4), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiplayerConditionsTest, "GameMainMenu.Multiplayer.Conditions", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMultiplayerConditionsTest::RunTest(const FString& Parameters)
{
	FFormFixture F;
	F.Add<UMenuBoolValueConfig>(TEXT("HasPassword"));
	auto* Password = F.Add<UMenuEditableValueConfig>(TEXT("Password"));
	Password->EditDependencyIds.Add(TEXT("HasPassword"));
	auto* OtherTab = F.AddTab(TEXT("Advanced"));
	F.Add<UMenuScalarValueConfig>(TEXT("Players"), OtherTab);
	auto* Conditional = F.Add<UMenuEditableValueConfig>(TEXT("Conditional"));
	FMenuEntryCondition Rule;
	Rule.SourceDataId = TEXT("Players"); Rule.SourceTabId = TEXT("Advanced");
	Rule.Comparison = EMenuComparison::GreaterOrEqual; Rule.Value = TEXT("1.5");
	Conditional->EditConditions.Add(Rule);
	Rule.SourceTabId = NAME_None; Rule.SourceDataId = TEXT("HasPassword"); Rule.Comparison = EMenuComparison::IsTrue;
	Conditional->EditConditions.Add(Rule);
	F.Init();
	int32 Notifications = 0;
	F.Item(TEXT("Password"))->OnDependencyDataModified.AddLambda([&Notifications](UListDataObject_Base*, EOptionsListDataModifyReason) { ++Notifications; });
	TestFalse(TEXT("Legacy gate initially disabled"), F.Item(TEXT("Password"))->IsDataCurrentlyEditable());
	F.Item(TEXT("HasPassword"))->TrySetValueFromString(TEXT("true"));
	TestTrue(TEXT("Legacy gate enabled"), F.Item(TEXT("Password"))->IsDataCurrentlyEditable());
	TestTrue(TEXT("Programmatic restore refreshes dependent UI"), Notifications > 0);
	TestFalse(TEXT("All needs both rules"), F.Item(TEXT("Conditional"))->IsDataCurrentlyEditable());
	F.Item(TEXT("Players"), TEXT("Advanced"))->TrySetValueFromString(TEXT("1.75"));
	TestTrue(TEXT("Cross-tab numeric condition"), F.Item(TEXT("Conditional"))->IsDataCurrentlyEditable());
	Conditional->ConditionMatch = EMenuConditionMatch::Any;
	F.Init();
	F.Item(TEXT("HasPassword"))->TrySetValueFromString(TEXT("true"));
	TestTrue(TEXT("Any needs one rule"), F.Item(TEXT("Conditional"))->IsDataCurrentlyEditable());
	FMenuEntryCondition Compare;
	Compare.Comparison = EMenuComparison::NotEquals; Compare.Value = TEXT("Map_01");
	TestTrue(TEXT("Not equals"), Compare.Matches(TEXT("Map_02")));
	Compare.Comparison = EMenuComparison::Less; Compare.Value = TEXT("8");
	TestTrue(TEXT("Numeric less"), Compare.Matches(TEXT("4")));
	TestFalse(TEXT("Bad numeric source rejected"), Compare.Matches(TEXT("invalid")));
	Compare.Comparison = EMenuComparison::IsEmpty;
	TestTrue(TEXT("Empty check"), Compare.Matches(TEXT("")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiplayerKeysTest, "GameMainMenu.Multiplayer.SaveKeys", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMultiplayerKeysTest::RunTest(const FString& Parameters)
{
	FFormFixture F;
	auto* Name = F.Add<UMenuEditableValueConfig>(TEXT("GameName"));
	Name->SaveKey = TEXT("RoomName");
	F.Init();
	auto* Save = NewObject<UMenuSessionData>();
	Save->SetSavedValue(TEXT("GameName"), TEXT("Legacy room"));
	F.Registry->LoadFormValues(Save);
	TestEqual(TEXT("New key reads old DataId save"), F.Value(TEXT("GameName")), FString(TEXT("Legacy room")));
	F.Registry->SaveFormValues(Save);
	TestEqual(TEXT("Migrated value saved under new key"), Save->GetSavedValue(TEXT("RoomName")), FString(TEXT("Legacy room")));
	TestFalse(TEXT("Legacy key removed after migration"), Save->SavedValues.Contains(TEXT("GameName")));
	auto* Other = F.AddTab(TEXT("Other"));
	auto* Duplicate = F.Add<UMenuEditableValueConfig>(TEXT("DifferentId"), Other);
	Duplicate->SaveKey = TEXT("RoomName");
	AddExpectedError(TEXT("duplicate SaveKey"), EAutomationExpectedErrorFlags::Contains, 1);
	F.Init();
	TestTrue(TEXT("Duplicate key detected"), F.Registry->HasConfigurationErrors());
	TestFalse(TEXT("Invalid form cannot overwrite save"), F.Registry->SaveFormValues(Save));
	Duplicate->SaveKey = TEXT("OtherName");
	F.Init();
	TestFalse(TEXT("Independent keys resolve collision"), F.Registry->HasConfigurationErrors());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiplayerListTest, "GameMainMenu.Multiplayer.SessionList", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMultiplayerListTest::RunTest(const FString& Parameters)
{
	for (bool bHeader : {false, true})
	{
		FFormFixture F;
		auto* List = F.AddTab(TEXT("JoinGame"));
		if (bHeader) List->HeaderColumns.Add(FText::FromString(TEXT("Server")));
		F.Add<UMenuEditableValueConfig>(TEXT("ServerList"), List);
		F.Init();
		TArray<FSessionListItem> Sessions;
		Sessions.SetNum(2);
		Sessions[0].ServerName = FText::FromString(TEXT("One"));
		Sessions[1].ServerName = FText::FromString(TEXT("Two"));
		const auto Count = [&F](UClass* Class)
		{
			int32 Result = 0;
			for (auto* Item : F.Registry->GetListItemsForTab(TEXT("JoinGame"))) if (Item->IsA(Class)) ++Result;
			return Result;
		};
		TestEqual(TEXT("No fake servers when preview disabled"), Count(UListDataObject_Session::StaticClass()), 0);
		TestNull(TEXT("Legacy placeholder is not a form field"), F.Item(TEXT("ServerList"), TEXT("JoinGame")));
		F.Registry->RebuildSessionList(Sessions);
		TestEqual(TEXT("Results work with both header and collection layouts"), Count(UListDataObject_Session::StaticClass()), 2);
		Sessions.SetNum(1);
		F.Registry->RebuildSessionList(Sessions);
		TestEqual(TEXT("Refresh replaces old results"), Count(UListDataObject_Session::StaticClass()), 1);
		F.Registry->RebuildSessionList({});
		TestEqual(TEXT("Empty search clears stale results"), Count(UListDataObject_Session::StaticClass()), 0);
		TestEqual(TEXT("Refresh preserves header"), Count(UListDataObject_HeaderRow::StaticClass()), bHeader ? 1 : 0);
		List->bSessionList = true;
		List->Entries.Reset();
		F.Init();
		F.Registry->RebuildSessionList(Sessions);
		TestEqual(TEXT("Explicit container needs no placeholder entry"), Count(UListDataObject_Session::StaticClass()), 1);
	}
	return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiplayerAssetTest, "GameMainMenu.Multiplayer.ExistingAssets", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FMultiplayerAssetTest::RunTest(const FString& Parameters)
{
	auto* Asset = LoadObject<UMenuConfigData>(nullptr, TEXT("/GameMainMenu/BP/DataAsset/MultiplayerScreen/DA_MultiplayerConfig.DA_MultiplayerConfig"));
	if (!TestNotNull(TEXT("Existing multiplayer config loads"), Asset)) return false;
	auto* Config = DuplicateObject<UMenuConfigData>(Asset, GetTransientPackage());
	Config->bPreviewSessionsInEditor = false;
	auto* Registry = NewObject<UMultiplayerDataRegistry>();
	Registry->SetConfig(Config);
	Registry->InitRegistry();
	TestFalse(TEXT("Existing config remains valid without resaving assets"), Registry->HasConfigurationErrors());
	auto* Save = NewObject<UMenuSessionData>();
	TestTrue(TEXT("Existing form can save"), Registry->SaveFormValues(Save));
	Registry->LoadFormValues(Save);
	TestTrue(TEXT("Existing create tab has entries"), Registry->GetListItemsForTab(Config->CreateGameTabId).Num() > 0);
	TArray<FSessionListItem> Results;
	Results.SetNum(1);
	Registry->RebuildSessionList(Results);
	int32 Count = 0;
	for (auto* Item : Registry->GetListItemsForTab(Config->JoinGameTabId)) if (Item->IsA<UListDataObject_Session>()) ++Count;
	TestEqual(TEXT("Existing JoinGame config receives real results"), Count, 1);
	return true;
}
#endif
