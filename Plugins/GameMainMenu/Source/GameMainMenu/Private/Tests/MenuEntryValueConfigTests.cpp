// HangHang All Rights Reserved

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/LinkerInstancingContext.h"
#include "UObject/Linker.h"
#include "UObject/UnrealType.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMenuEntryLegacyUpgradeTest,
	"GameMainMenu.MenuConfig.LegacyValueUpgrade",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMenuEntryLegacyUpgradeTest::RunTest(const FString& Parameters)
{
	struct FCase { EMenuEntryValueType Type; TArray<FName> Fields; };
	const TArray<FCase> Cases = {
		{ EMenuEntryValueType::Bool, { TEXT("bDefaultBool") } },
		{ EMenuEntryValueType::Scalar, { TEXT("DefaultValue"), TEXT("ScalarMin"), TEXT("ScalarMax"), TEXT("ScalarOutMin"), TEXT("ScalarOutMax"), TEXT("StepSize"), TEXT("NumericType") } },
		{ EMenuEntryValueType::String, { TEXT("DefaultStringValue"), TEXT("AvailableOptions") } },
		{ EMenuEntryValueType::Integer, { TEXT("DefaultIntValue"), TEXT("AvailableIntegerOptions") } },
		{ EMenuEntryValueType::Culture, { TEXT("DefaultStringValue") } },
		{ EMenuEntryValueType::Resolution, { TEXT("DefaultStringValue") } },
		{ EMenuEntryValueType::Editable, { TEXT("HintText"), TEXT("MaxCharCount"), TEXT("bIsPassword"), TEXT("CharFilter") } }
	};
	for (const FCase& Case : Cases)
	{
		auto* Legacy = NewObject<UMenuEntryConfig>();
		Legacy->ValueType = Case.Type;
		Legacy->bDefaultBool = true;
		Legacy->DefaultValue = TEXT("2.2");
		Legacy->ScalarMin = -1.f;
		Legacy->ScalarMax = 100.f;
		Legacy->ScalarOutMin = 1.7f;
		Legacy->ScalarOutMax = 2.7f;
		Legacy->StepSize = 0.25f;
		Legacy->NumericType = ECommonNumericType::Number;
		Legacy->DefaultStringValue = TEXT("CustomValue");
		Legacy->AvailableOptions.Add(TEXT("CustomValue"), FText::FromString(TEXT("Localized choice")));
		Legacy->DefaultIntValue = 3;
		Legacy->AvailableIntegerOptions.Add(3, FText::FromString(TEXT("High")));
		Legacy->HintText = FText::FromString(TEXT("Enter password"));
		Legacy->MaxCharCount = 6;
		Legacy->bIsPassword = true;
		Legacy->CharFilter = EEditableCharFilter::NumbersOnly;

		Legacy->UpgradeLegacyValueConfig();
		if (!TestNotNull(TEXT("Every legacy type receives an inline config"), Legacy->ValueConfig.Get())) return false;
		TestEqual(TEXT("Type is preserved"), Legacy->GetEffectiveValueType(), Case.Type);
		TestTrue(TEXT("Migrated config belongs to its entry"), Legacy->ValueConfig->GetOuter() == Legacy);
		auto* RoundTrip = NewObject<UMenuEntryConfig>();
		Legacy->ValueConfig->ExportLegacy(*RoundTrip);
		for (FName Field : Case.Fields)
		{
			const FProperty* Property = FindFProperty<FProperty>(UMenuEntryConfig::StaticClass(), Field);
			if (!TestNotNull(Field.ToString(), Property)) return false;
			TestTrue(Field.ToString() + TEXT(" survives migration"), Property->Identical_InContainer(Legacy, RoundTrip));
			TestTrue(Field.ToString() + TEXT(" retains Blueprint getter access"), Property->HasAnyPropertyFlags(CPF_BlueprintVisible));
			TestFalse(Field.ToString() + TEXT(" is no longer a duplicate editor field"), Property->HasAnyPropertyFlags(CPF_Edit));
		}
		UMenuEntryValueConfig* Original = Legacy->ValueConfig;
		Legacy->UpgradeLegacyValueConfig();
		TestTrue(TEXT("Repeated upgrade preserves the configured object"), Legacy->ValueConfig == Original);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMenuEntryInlineFactoryTest,
	"GameMainMenu.MenuConfig.InlineFactoryAndDuplication",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMenuEntryInlineFactoryTest::RunTest(const FString& Parameters)
{
	auto* Config = NewObject<UMenuEntryConfig>();
	Config->DataId = TEXT("Gamma");
	Config->DisplayName = FText::FromString(TEXT("Gamma"));
	Config->Description = FText::FromString(TEXT("Display adjustment"));
	Config->DynamicDetails = FText::FromString(TEXT("Additional details"));
	auto* Scalar = NewObject<UMenuScalarValueConfig>(Config);
	Config->ValueConfig = Scalar;
	Scalar->DefaultValue = TEXT("2.2");
	Scalar->ScalarMin = 0.f;
	Scalar->ScalarMax = 100.f;
	Scalar->ScalarOutMin = 1.7f;
	Scalar->ScalarOutMax = 2.7f;
	Scalar->StepSize = 0.5f;
	Scalar->NumericType = ECommonNumericType::Number;
	// Legacy ValueType still says Bool: the inline object must be authoritative.
	auto* Slider = Cast<UListDataObject_Scalar>(CreateEntryFromConfig(Config));
	if (!TestNotNull(TEXT("Inline type selects the runtime class"), Slider)) return false;
	TestEqual(TEXT("Effective type does not depend on legacy fields"), Config->GetEffectiveValueType(), EMenuEntryValueType::Scalar);
	TestEqual(TEXT("Display range maximum"), Slider->GetDisplayValueRange().GetUpperBoundValue(), 100.f);
	TestEqual(TEXT("Output range minimum"), Slider->GetOutValueRange().GetLowerBoundValue(), 1.7f);
	TestEqual(TEXT("Output range maximum"), Slider->GetOutValueRange().GetUpperBoundValue(), 2.7f);
	TestEqual(TEXT("Step size"), Slider->GetSliderStepSize(), 0.5f);
	TestEqual(TEXT("Numeric display type"), Slider->GetCommonNumericType(), ECommonNumericType::Number);
	FString ResetValue;
	Slider->SetBinding(MakeShared<FOptionsDataInteractionHelper>(
		[]() { return FString(TEXT("2.5")); }, [&ResetValue](const FString& Value) { ResetValue = Value; }));
	TestTrue(TEXT("Inline default can be restored"), static_cast<UListDataObject_Base*>(Slider)->TryResetBackToDefaultValue());
	TestEqual(TEXT("Reset writes the configured output value"), ResetValue, FString(TEXT("2.2")));
	TestEqual(TEXT("Common identity"), Slider->GetDataId(), Config->DataId);
	TestEqual(TEXT("Common description"), Slider->GetDescriptionRichText().ToString(), Config->Description.ToString());
	TestEqual(TEXT("Common details"), Slider->GetDynamicDetailsRichText().ToString(), Config->DynamicDetails.ToString());

	auto* Copy = DuplicateObject<UMenuEntryConfig>(Config, GetTransientPackage());
	auto* CopiedScalar = Cast<UMenuScalarValueConfig>(Copy->ValueConfig);
	if (!TestNotNull(TEXT("Duplicated entry owns a typed config"), CopiedScalar)) return false;
	TestTrue(TEXT("Config is deep-copied"), CopiedScalar != Scalar && CopiedScalar->GetOuter() == Copy);
	CopiedScalar->DefaultValue = TEXT("2.5");
	TestEqual(TEXT("Editing the duplicate preserves the source"), Scalar->DefaultValue, FString(TEXT("2.2")));

	auto* Editable = NewObject<UMenuEditableValueConfig>(Config);
	Config->ValueConfig = Editable;
	Editable->MaxCharCount = 6;
	Editable->bIsPassword = true;
	Editable->CharFilter = EEditableCharFilter::NumbersOnly;
	Editable->HintText = FText::FromString(TEXT("Password"));
	auto* Password = Cast<UListDataObject_Editable>(CreateEntryFromConfig(Config));
	if (!TestNotNull(TEXT("Changing inline type creates a text input"), Password)) return false;
	FText Reason;
	TestTrue(TEXT("Password accepts configured digits"), Password->ValidateText(FText::FromString(TEXT("123456")), Reason));
	TestFalse(TEXT("Password rejects excessive length"), Password->ValidateText(FText::FromString(TEXT("1234567")), Reason));
	TestFalse(TEXT("Password rejects letters"), Password->ValidateText(FText::FromString(TEXT("abc")), Reason));
	TestTrue(TEXT("Password remains masked"), Password->GetbIsPassword());
	TestEqual(TEXT("Input hint"), Password->GetHintText().ToString(), FString(TEXT("Password")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMenuEntryExistingAssetTest,
	"GameMainMenu.MenuConfig.ExistingAssetsUpgradeOnLoad",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMenuEntryExistingAssetTest::RunTest(const FString& Parameters)
{
	const TCHAR* Paths[] = {
		TEXT("/GameMainMenu/BP/DataAsset/OptionScreen/Tabs/Category/Video/AdvancedGraphics/Entry-FrameRateLimit.Entry-FrameRateLimit"),
		TEXT("/GameMainMenu/BP/DataAsset/MultiplayerScreen/Tabs/Category/CreateGame/General/DA_Entry_Password.DA_Entry_Password")
	};
	for (const TCHAR* Path : Paths)
	{
		auto* Asset = LoadObject<UMenuEntryConfig>(nullptr, Path);
		if (!TestNotNull(TEXT("Existing entry asset loads"), Asset)) return false;
		if (!TestNotNull(TEXT("PostLoad creates an inline config"), Asset->ValueConfig.Get())) return false;
		TestTrue(TEXT("Loaded config belongs to the entry asset"), Asset->ValueConfig->GetOuter() == Asset);
		UListDataObject_Base* Entry = CreateEntryFromConfig(Asset);
		if (!TestNotNull(TEXT("Migrated asset creates its runtime entry"), Entry)) return false;
		if (Asset->BindingKey == TEXT("FrameRateLimit"))
		{
			auto* Rate = Cast<UListDataObject_String>(Entry);
			if (!TestNotNull(TEXT("Frame rate remains a choice entry"), Rate)) return false;
			int32 SetterCalls = 0;
			Rate->SetBinding(MakeShared<FOptionsDataInteractionHelper>(
				[]() { return LexToString(0.f); }, [&SetterCalls](const FString&) { ++SetterCalls; }));
			Rate->InitializeDataObject();
			TestTrue(TEXT("Migrated frame rate label matches a configured option"), Rate->GetOptionsTextArray().ContainsByPredicate(
				[Rate](const FText& Label) { return Label.EqualTo(Rate->GetCurrentTextValue()); }));
			TestEqual(TEXT("Loading and displaying does not change settings"), SetterCalls, 0);
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMenuEntryInlineSaveTest,
	"GameMainMenu.MenuConfig.InlineSaveAndReload",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMenuEntryInlineSaveTest::RunTest(const FString& Parameters)
{
	const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Automation/MenuEntryConfig/"));
	IFileManager::Get().MakeDirectory(*Directory, true);
	const FString Root = TEXT("/MenuEntryConfigTests/");
	FPackageName::RegisterMountPoint(Root, Directory);
	const FString Name = TEXT("Entry_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	const FString PackageName = Root + Name;
	const FString Filename = Directory / (Name + TEXT(".uasset"));
	UPackage* Package = nullptr;
	UPackage* ReloadedPackage = nullptr;
	ON_SCOPE_EXIT
	{
		if (ReloadedPackage) ResetLoaders(ReloadedPackage);
		if (Package) ResetLoaders(Package);
		FPackageName::UnRegisterMountPoint(Root, Directory);
		IFileManager::Get().Delete(*Filename);
	};
	Package = CreatePackage(*PackageName);
	auto* Asset = NewObject<UMenuEntryConfig>(Package, TEXT("Entry"), RF_Public | RF_Standalone);
	auto* Value = NewObject<UMenuStringValueConfig>(Asset, NAME_None, RF_Transactional);
	Asset->ValueConfig = Value;
	Asset->BindingKey = TEXT("FrameRateLimit");
	Value->DefaultValue = TEXT("60.0");
	Value->AvailableOptions.Add(TEXT("0"), FText::FromString(TEXT("Unlimited")));
	Value->AvailableOptions.Add(TEXT("60"), FText::FromString(TEXT("60 FPS")));
	FSavePackageArgs Args;
	Args.TopLevelFlags = RF_Public | RF_Standalone;
	// Procedural fixture save: it has no bulk data and must not queue editor validation
	// for the temporary mount point after the fixture has been removed.
	Args.SaveFlags = SAVE_BulkDataByReference;
	if (!TestTrue(TEXT("Entry with inline config saves"), UPackage::SavePackage(Package, Asset, *Filename, Args))) return false;
	TestEqual(TEXT("Saving synchronizes existing Blueprint type readers"), Asset->ValueType, EMenuEntryValueType::String);
	TestEqual(TEXT("Saving synchronizes existing Blueprint default readers"), Asset->DefaultStringValue, Value->DefaultValue);

	const FString ReloadName = PackageName + TEXT("_Reloaded");
	FLinkerInstancingContext Context;
	Context.AddPackageMapping(FName(*PackageName), FName(*ReloadName));
	ReloadedPackage = LoadPackage(CreatePackage(*ReloadName), *PackageName, LOAD_None, nullptr, &Context);
	if (!TestNotNull(TEXT("Saved package reloads independently"), ReloadedPackage)) return false;
	auto* Reloaded = FindObject<UMenuEntryConfig>(ReloadedPackage, TEXT("Entry"));
	if (!TestNotNull(TEXT("Entry is present after reload"), Reloaded)) return false;
	TestTrue(TEXT("Reload creates a different asset instance"), Reloaded != Asset);
	auto* ReloadedValue = Cast<UMenuStringValueConfig>(Reloaded->ValueConfig);
	if (!TestNotNull(TEXT("Inline type survives disk serialization"), ReloadedValue)) return false;
	TestTrue(TEXT("Reloaded config is owned by the reloaded asset"), ReloadedValue->GetOuter() == Reloaded);
	TestEqual(TEXT("Inline default survives disk serialization"), ReloadedValue->DefaultValue, FString(TEXT("60.0")));
	TestEqual(TEXT("Options survive disk serialization"), ReloadedValue->AvailableOptions.Num(), 2);
	auto* Entry = CastChecked<UListDataObject_String>(CreateEntryFromConfig(Reloaded));
	Entry->InitializeDataObject();
	TestEqual(TEXT("Reloaded config retains numeric frame rate matching"), Entry->GetCurrentTextValue().ToString(), FString(TEXT("60 FPS")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
