// HangHang All Rights Reserved

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Culture.h"
#include "UObject/UnrealType.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/OptionsKeyBindingProfile.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/UListDataObject_KeyBinding.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"
#include "UserSettings/EnhancedInputUserSettings.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsDeferredSettingsTest,
	"GameMainMenu.Options.Deferred.EditsAndResetsStayInDraft",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsDeferredSettingsTest::RunTest(const FString& Parameters)
{
	const UFrontendGameUserSettings* Live = UFrontendGameUserSettings::Get();
	if (!TestNotNull(TEXT("Live settings"), Live)) return false;
	const float OriginalVolume = Live->GetOverallVolume();
	const int32 OriginalShadow = Live->GetShadowQuality();
	const FString OriginalDifficulty = Live->GetDifficulty();
	const FString OriginalCulture = FInternationalization::Get().GetCurrentCulture()->GetName();
	const float OriginalGamma = GEngine->GetDisplayGamma();

	UMenuConfigData* Config = NewObject<UMenuConfigData>();
	UMenuTabConfig* Tab = NewObject<UMenuTabConfig>(Config);
	Tab->TabId = TEXT("Tab-Video");
	Config->Tabs.Add(Tab);
	UMenuCategoryConfig* Category = NewObject<UMenuCategoryConfig>(Tab);
	Category->CategoryId = TEXT("DraftTest");
	Tab->Categories.Add(Category);
	const auto AddEntry = [Category](const TCHAR* Id, EMenuEntryValueType Type, const TCHAR* Key)
	{
		UMenuEntryConfig* Entry = NewObject<UMenuEntryConfig>(Category);
		Entry->DataId = Id;
		Entry->BindingKey = Key;
		Entry->ValueType = Type;
		Entry->bApplyImmediately = true; // Legacy assets must still defer all changes.
		Category->Entries.Add(Entry);
		return Entry;
	};
	UMenuEntryConfig* VolumeConfig = AddEntry(TEXT("Volume"), EMenuEntryValueType::Scalar, TEXT("OverallVolume"));
	VolumeConfig->ScalarMax = 2.f;
	VolumeConfig->ScalarOutMax = 2.f;
	VolumeConfig->DefaultValue = TEXT("1.0");
	UMenuEntryConfig* DifficultyConfig = AddEntry(TEXT("Difficulty"), EMenuEntryValueType::String, TEXT("Difficulty"));
	DifficultyConfig->DefaultStringValue = TEXT("Normal");
	UMenuEntryConfig* OverallConfig = AddEntry(TEXT("OverallQuality"), EMenuEntryValueType::Integer, TEXT("OverallScalabilityLevel"));
	UMenuEntryConfig* ShadowConfig = AddEntry(TEXT("Shadow"), EMenuEntryValueType::Integer, TEXT("ShadowQuality"));
	ShadowConfig->EditDependencyIds.Add(TEXT("OverallQuality"));
	AddEntry(TEXT("Culture"), EMenuEntryValueType::String, TEXT("CurrentCulture"));
	UMenuEntryConfig* GammaConfig = AddEntry(TEXT("Gamma"), EMenuEntryValueType::Scalar, TEXT("DisplayGamma"));
	GammaConfig->ScalarMax = GammaConfig->ScalarOutMax = 4.f;
	for (int32 Quality = 0; Quality <= 4; ++Quality)
	{
		OverallConfig->AvailableIntegerOptions.Add(Quality, FText::AsNumber(Quality));
		ShadowConfig->AvailableIntegerOptions.Add(Quality, FText::AsNumber(Quality));
	}

	UOptionsDataRegistry* Registry = NewObject<UOptionsDataRegistry>();
	Registry->SetOptionsConfig(Config);
	Registry->InitOptionsDataRegistry(nullptr);
	TMap<FName, UListDataObject_Base*> Items;
	for (UListDataObject_Base* Item : Registry->GetListSourceItemBySelectedTabId(Tab->TabId)) Items.Add(Item->GetDataId(), Item);
	auto* Volume = CastChecked<UListDataObject_Scalar>(Items.FindChecked(TEXT("Volume")));
	auto* Difficulty = CastChecked<UListDataObject_String>(Items.FindChecked(TEXT("Difficulty")));
	auto* Overall = CastChecked<UListDataObject_String>(Items.FindChecked(TEXT("OverallQuality")));
	auto* Shadow = CastChecked<UListDataObject_String>(Items.FindChecked(TEXT("Shadow")));
	auto* Culture = CastChecked<UListDataObject_String>(Items.FindChecked(TEXT("Culture")));
	auto* Gamma = CastChecked<UListDataObject_Scalar>(Items.FindChecked(TEXT("Gamma")));

	Volume->SetCurrentValue(1.75f);
	Difficulty->SetValueFromString(TEXT("Hard"));
	Culture->SetValueFromString(OriginalCulture == TEXT("en") ? TEXT("fr") : TEXT("en"));
	Gamma->SetCurrentValue(3.f);
	Overall->SetValueFromString(TEXT("2"));
	TestEqual(TEXT("Preset updates the draft shadow quality"), Shadow->GetCurrentStringValue(), FString(TEXT("2")));
	Shadow->SetValueFromString(TEXT("1"));
	TestEqual(TEXT("Individual edit makes the draft preset custom"), Overall->GetCurrentStringValue(), FString(TEXT("-1")));
	TestEqual(TEXT("Live volume is unchanged"), Live->GetOverallVolume(), OriginalVolume);
	TestEqual(TEXT("Live gameplay setting is unchanged"), Live->GetDifficulty(), OriginalDifficulty);
	TestEqual(TEXT("Live quality is unchanged"), Live->GetShadowQuality(), OriginalShadow);
	TestEqual(TEXT("Runtime language is unchanged"), FInternationalization::Get().GetCurrentCulture()->GetName(), OriginalCulture);
	TestEqual(TEXT("Runtime gamma is unchanged"), GEngine->GetDisplayGamma(), OriginalGamma);

	// Commit to a disposable settings object, without applying or saving the user's settings.
	UFrontendGameUserSettings* Target = NewObject<UFrontendGameUserSettings>();
	Registry->CommitPendingSettings(Target);
	TestEqual(TEXT("Commit transfers pending volume"), Target->GetOverallVolume(), 1.75f);
	TestEqual(TEXT("Commit transfers pending difficulty"), Target->GetDifficulty(), FString(TEXT("Hard")));
	TestEqual(TEXT("Commit transfers pending gamma"), Target->GetCurrentDisplayGamma(), 3.f);
	TestEqual(TEXT("Commit preserves custom shadow quality"), Target->GetShadowQuality(), 1);
	TestEqual(TEXT("Commit preserves other preset qualities"), Target->GetTextureQuality(), 2);
	TestEqual(TEXT("Commit preserves unlisted foliage quality"), Target->GetFoliageQuality(), 2);
	TestEqual(TEXT("Commit preserves custom preset"), Target->GetOverallScalabilityLevel(), -1);
	TestEqual(TEXT("Commit alone does not switch language"), FInternationalization::Get().GetCurrentCulture()->GetName(), OriginalCulture);
	TestEqual(TEXT("Commit alone does not change runtime gamma"), GEngine->GetDisplayGamma(), OriginalGamma);

	TestTrue(TEXT("Reset volume updates the draft"), Items.FindChecked(TEXT("Volume"))->TryResetBackToDefaultValue());
	TestEqual(TEXT("Reset default appears in the UI"), Volume->GetCurrentValue(), 1.f);
	TestEqual(TEXT("Reset does not change live volume"), Live->GetOverallVolume(), OriginalVolume);
	Registry->CommitPendingSettings(Target);
	TestEqual(TEXT("Reset value is transferred on commit"), Target->GetOverallVolume(), 1.f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOptionsDeferredKeyBindingTest,
	"GameMainMenu.Options.Deferred.KeyBindingsStayInDraft",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOptionsDeferredKeyBindingTest::RunTest(const FString& Parameters)
{
	UEnhancedInputUserSettings* Settings = NewObject<UEnhancedInputUserSettings>();
	FPlayerMappableKeyProfileCreationArgs ProfileArgs;
	ProfileArgs.ProfileType = UEnhancedPlayerMappableKeyProfile::StaticClass();
	ProfileArgs.ProfileStringIdentifier = TEXT("OptionsDraftTest");
	ProfileArgs.bSetAsCurrentProfile = true;
	UEnhancedPlayerMappableKeyProfile* LiveProfile = Settings->CreateNewKeyProfile(ProfileArgs);
	UInputMappingContext* Context = NewObject<UInputMappingContext>();
	const auto AddMapping = [Context](const TCHAR* Name, FKey Key)
	{
		UInputAction* Action = NewObject<UInputAction>(Context);
		UPlayerMappableKeySettings* KeySettings = NewObject<UPlayerMappableKeySettings>(Action);
		KeySettings->Name = Name;
		// Populate the editor-exposed property as an input action asset would.
		FObjectProperty* Property = FindFProperty<FObjectProperty>(UInputAction::StaticClass(), TEXT("PlayerMappableKeySettings"));
		check(Property);
		Property->SetObjectPropertyValue_InContainer(Action, KeySettings);
		Context->MapKey(Action, Key);
	};
	AddMapping(TEXT("Jump"), EKeys::SpaceBar);
	AddMapping(TEXT("Interact"), EKeys::E);
	Settings->RegisterInputMappingContext(Context);
	UOptionsKeyBindingProfile* Draft = NewObject<UOptionsKeyBindingProfile>();
	Draft->InitializeFromProfile(*LiveProfile);
	TestEqual(TEXT("Draft duplicates all registered mapping rows"), Draft->GetPlayerMappingRows().Num(), 2);
	const auto CreateEntry = [Draft](const TCHAR* Name)
	{
		UUListDataObject_KeyBinding* Entry = NewObject<UUListDataObject_KeyBinding>();
		Entry->SetKeyProfile(Draft);
		FKeyBindingSlotDescriptor Slot;
		Slot.PlayerMappableName = Name;
		Slot.MappableSlot = static_cast<uint8>(EPlayerMappableKeySlot::First);
		Entry->AddKeyBindingSlot(Slot);
		return Entry;
	};
	UUListDataObject_KeyBinding* Jump = CreateEntry(TEXT("Jump"));
	UUListDataObject_KeyBinding* Interact = CreateEntry(TEXT("Interact"));
	Jump->SetKeyForSlot(0, EKeys::F);
	TestEqual(TEXT("New key appears in draft"), Jump->GetCurrentKeyForSlot(0), EKeys::F);
	const FPlayerKeyMapping* LiveMapping = Settings->FindCurrentMappingForSlot(TEXT("Jump"), EPlayerMappableKeySlot::First);
	if (!TestNotNull(TEXT("Live mapping exists"), LiveMapping)) return false;
	TestEqual(TEXT("Active mapping has not changed"), LiveMapping->GetCurrentKey(), EKeys::SpaceBar);
	FName ConflictName;
	FText ConflictText;
	TestTrue(TEXT("Conflict detection uses other pending changes"), Interact->FindConflictingAction(EKeys::F, ConflictName, ConflictText));
	TestEqual(TEXT("Conflict identifies pending jump mapping"), ConflictName, FName(TEXT("Jump")));
	Jump->ResetSlotToDefault(0);
	TestEqual(TEXT("Reset updates the draft"), Jump->GetCurrentKeyForSlot(0), EKeys::SpaceBar);
	TestFalse(TEXT("Reset clears the pending conflict"), Interact->FindConflictingAction(EKeys::F, ConflictName, ConflictText));
	TestEqual(TEXT("Reset leaves active mapping alone"), LiveMapping->GetCurrentKey(), EKeys::SpaceBar);
	return true;
}

#endif
