// HangHang All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"

#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "Widgets/Options/DataObjects/UListDataObject_KeyBinding.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "LocalizationKeys.h"

#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputAction.h"
#include "FrontendDebugHelper.h"

// ============================================================================
// Factory helpers
// ============================================================================

UListDataObject_Collection* UOptionsDataRegistry::CreateCollection(const FName& Id, const FText& DisplayName) const
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}

void UOptionsDataRegistry::InitBindingMap()
{
	using FSettings = UFrontendGameUserSettings;

	// ---- Gameplay ----
	BindingMap.Add("Difficulty", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return FSettings::Get()->GetDifficulty(); },
		[](const FString& V) { FSettings::Get()->SetDifficulty(V); }));
	BindingMap.Add("ShowHealthBar", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetShowHealthBar()); },
		[](const FString& V) { FSettings::Get()->SetShowHealthBar(V.ToBool()); }));
	BindingMap.Add("ShowDamageNumbers", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetShowDamageNumbers()); },
		[](const FString& V) { FSettings::Get()->SetShowDamageNumbers(V.ToBool()); }));
	BindingMap.Add("HighlightInteraction", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetHighlightInteraction()); },
		[](const FString& V) { FSettings::Get()->SetHighlightInteraction(V.ToBool()); }));
	BindingMap.Add("ShowSubtitles", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetShowSubtitles()); },
		[](const FString& V) { FSettings::Get()->SetShowSubtitles(V.ToBool()); }));
	BindingMap.Add("AutoSave", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetAutoSave()); },
		[](const FString& V) { FSettings::Get()->SetAutoSave(V.ToBool()); }));
	BindingMap.Add("CurrentCulture", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return FSettings::Get()->GetCurrentCulture(); },
		[](const FString& V) { FSettings::Get()->SetCurrentCulture(V); }));

	// ---- Audio ----
	BindingMap.Add("OverallVolume", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetOverallVolume()); },
		[](const FString& V) { FSettings::Get()->SetOverallVolume(FCString::Atof(*V)); }));
	BindingMap.Add("MusicVolume", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetMusicVolume()); },
		[](const FString& V) { FSettings::Get()->SetMusicVolume(FCString::Atof(*V)); }));
	BindingMap.Add("SFXVolume", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetSFXVolume()); },
		[](const FString& V) { FSettings::Get()->SetSFXVolume(FCString::Atof(*V)); }));
	BindingMap.Add("VoiceVolume", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetVoiceVolume()); },
		[](const FString& V) { FSettings::Get()->SetVoiceVolume(FCString::Atof(*V)); }));
	BindingMap.Add("AmbientVolume", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetAmbientVolume()); },
		[](const FString& V) { FSettings::Get()->SetAmbientVolume(FCString::Atof(*V)); }));
	BindingMap.Add("UIVolume", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetUIVolume()); },
		[](const FString& V) { FSettings::Get()->SetUIVolume(FCString::Atof(*V)); }));
	BindingMap.Add("AllowBackgroundSound", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetAllowBackgroundSound()); },
		[](const FString& V) { FSettings::Get()->SetAllowBackgroundSound(V.ToBool()); }));
	BindingMap.Add("SubtitlesEnabled", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetSubtitlesEnabled()); },
		[](const FString& V) { FSettings::Get()->SetSubtitlesEnabled(V.ToBool()); }));
	BindingMap.Add("HRTFEnabled", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetHRTFEnabled()); },
		[](const FString& V) { FSettings::Get()->SetHRTFEnabled(V.ToBool()); }));
	BindingMap.Add("MonoOutput", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetMonoOutput()); },
		[](const FString& V) { FSettings::Get()->SetMonoOutput(V.ToBool()); }));
	BindingMap.Add("DynamicRangeCompression", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetDynamicRangeCompression()); },
		[](const FString& V) { FSettings::Get()->SetDynamicRangeCompression(V.ToBool()); }));
	BindingMap.Add("DialogueBoost", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetDialogueBoost()); },
		[](const FString& V) { FSettings::Get()->SetDialogueBoost(V.ToBool()); }));

	// ---- Video ----
	BindingMap.Add("FullscreenMode", MakeShared<FOptionsDataInteractionHelper>(
		[]() { const UEnum* E = StaticEnum<EWindowMode::Type>(); return E->GetNameStringByValue(static_cast<int64>(FSettings::Get()->GetFullscreenMode())); },
		[](const FString& V) { const UEnum* E = StaticEnum<EWindowMode::Type>(); const int64 Val = E->GetValueByNameString(V); if (Val != INDEX_NONE) FSettings::Get()->SetFullscreenMode(static_cast<EWindowMode::Type>(Val)); }));
	BindingMap.Add("Resolution", MakeShared<FOptionsDataInteractionHelper>(
		[]() { const FIntPoint R = FSettings::Get()->GetScreenResolution(); return FString::Printf(TEXT("(X=%d,Y=%d)"), R.X, R.Y); },
		[](const FString& V) { FIntPoint R(0, 0); sscanf_s(TCHAR_TO_ANSI(*V), "(X=%d,Y=%d)", &R.X, &R.Y); if (R.X > 0 && R.Y > 0) FSettings::Get()->SetScreenResolution(R); }));
	BindingMap.Add("DisplayGamma", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetCurrentDisplayGamma()); },
		[](const FString& V) { FSettings::Get()->SetCurrentDisplayGamma(FCString::Atof(*V)); }));
	BindingMap.Add("OverallScalabilityLevel", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetOverallScalabilityLevel()); },
		[](const FString& V) { FSettings::Get()->SetOverallScalabilityLevel(FCString::Atoi(*V)); }));
	BindingMap.Add("ResolutionScaleNormalized", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetResolutionScaleNormalized()); },
		[](const FString& V) { FSettings::Get()->SetResolutionScaleNormalized(FCString::Atof(*V)); }));
	BindingMap.Add("GlobalIlluminationQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetGlobalIlluminationQuality()); },
		[](const FString& V) { FSettings::Get()->SetGlobalIlluminationQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("ShadowQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetShadowQuality()); },
		[](const FString& V) { FSettings::Get()->SetShadowQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("AntiAliasingQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetAntiAliasingQuality()); },
		[](const FString& V) { FSettings::Get()->SetAntiAliasingQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("ViewDistanceQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetViewDistanceQuality()); },
		[](const FString& V) { FSettings::Get()->SetViewDistanceQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("TextureQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetTextureQuality()); },
		[](const FString& V) { FSettings::Get()->SetTextureQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("VisualEffectQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetVisualEffectQuality()); },
		[](const FString& V) { FSettings::Get()->SetVisualEffectQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("ReflectionQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetReflectionQuality()); },
		[](const FString& V) { FSettings::Get()->SetReflectionQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("PostProcessingQuality", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetPostProcessingQuality()); },
		[](const FString& V) { FSettings::Get()->SetPostProcessingQuality(FCString::Atoi(*V)); }));
	BindingMap.Add("VSync", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->IsVSyncEnabled()); },
		[](const FString& V) { FSettings::Get()->SetVSyncEnabled(V.ToBool()); }));
	BindingMap.Add("FrameRateLimit", MakeShared<FOptionsDataInteractionHelper>(
		[]() { return LexToString(FSettings::Get()->GetFrameRateLimit()); },
		[](const FString& V) { FSettings::Get()->SetFrameRateLimit(FCString::Atof(*V)); }));
}

TSharedPtr<FOptionsDataInteractionHelper> UOptionsDataRegistry::ResolveBinding(const FName& BindingKey) const
{
	if (const TSharedPtr<FOptionsDataInteractionHelper>* Found = BindingMap.Find(BindingKey))
	{
		return *Found;
	}
	return nullptr;
}

// ============================================================================
// Public interface
// ============================================================================

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	InitBindingMap();

	if (!ensureMsgf(OptionsConfig, TEXT("OptionsDataRegistry requires an OptionsConfig DataAsset.")))
	{
		return;
	}

	InitFromDataAsset(InOwningLocalPlayer);
}

TArray<UListDataObject_Base*> UOptionsDataRegistry::GetListSourceItemBySelectedTabId(const FName& InSelectedTabId) const
{
	UListDataObject_Collection* const* FoundCollectionTabPointer = RegisteredOptionsCollectionTabs.FindByPredicate(
	[InSelectedTabId](const UListDataObject_Collection* AvailableCollectionTab)->bool
	{
		return AvailableCollectionTab->GetDataId() == InSelectedTabId;
	}

	);

	if (!ensureMsgf(FoundCollectionTabPointer, TEXT("No valid found under the ID %s"), *InSelectedTabId.ToString())) { return {}; }

	const UListDataObject_Collection* FoundCollectionTab = *FoundCollectionTabPointer;

	TArray<UListDataObject_Base*> AllChildListItems;

	for (UListDataObject_Base* ChildListData : FoundCollectionTab->GetAllChildListData())
	{
		if (!ChildListData) continue;

		AllChildListItems.Add(ChildListData);

		if (ChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(ChildListData,AllChildListItems);
		}
	}

	return AllChildListItems;
}

void UOptionsDataRegistry::FindChildListDataRecursively(UListDataObject_Base* InParentData,
                                                        TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	if (!InParentData || !InParentData->HasAnyChildListData()) return;

	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		if (!SubChildListData) continue;

		OutFoundChildListData.Add(SubChildListData);

		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData,OutFoundChildListData);
		}
	}
}

// ============================================================================
// Control Tab (Key Bindings — dynamic, no factory extraction)
// ============================================================================

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* Tab = CreateCollection("Tab-Control", TXT_OPT_TAB_CONTROL());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = InOwningLocalPlayer
		? InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>() : nullptr;
	UEnhancedInputUserSettings* UserSettings = Subsystem ? Subsystem->GetUserSettings() : nullptr;
	UEnhancedPlayerMappableKeyProfile* ActiveProfile = UserSettings ? UserSettings->GetActiveKeyProfile() : nullptr;

	RegisteredOptionsCollectionTabs.Add(Tab);
	if (!ActiveProfile) { return; }

	const TMap<FName, FKeyMappingRow>& MappingRows = ActiveProfile->GetPlayerMappingRows();

	// Group mappings by UInputAction
	TMap<const UObject*, TArray<const FPlayerKeyMapping*>> GroupedByAction;

	for (const auto& RowPair : MappingRows)
	{
		for (const FPlayerKeyMapping& PlayerMapping : RowPair.Value.Mappings)
		{
			const UObject* Action = PlayerMapping.GetAssociatedInputAction();
			if (!Action) continue;

			GroupedByAction.FindOrAdd(Action).Add(&PlayerMapping);
		}
	}

	// Track created categories to avoid duplicates
	TMap<FString, UListDataObject_Collection*> CategoryMap;

	for (const auto& Pair : GroupedByAction)
	{
		const TArray<const FPlayerKeyMapping*>& MappingsForAction = Pair.Value;
		if (MappingsForAction.IsEmpty()) continue;

		const FName ActionMappingName = MappingsForAction[0]->GetMappingName();
		const FText ActionDisplayName = MappingsForAction[0]->GetDisplayName();
		const FText CategoryText = MappingsForAction[0]->GetDisplayCategory();

		const FString CategoryKey = CategoryText.IsEmpty() ? TEXT("General") : CategoryText.ToString();

		UListDataObject_Collection*& TargetCategory = CategoryMap.FindOrAdd(CategoryKey);
		if (!TargetCategory)
		{
			TargetCategory = CreateCollection(*CategoryKey, CategoryText.IsEmpty() ? TXT_VAL_GENERAL() : CategoryText);
			Tab->AddChildListData(TargetCategory);
		}

		UUListDataObject_KeyBinding* KeyBindingEntry = NewObject<UUListDataObject_KeyBinding>();
		KeyBindingEntry->SetOwningLocalPlayer(InOwningLocalPlayer);
		KeyBindingEntry->SetDataId(ActionMappingName);
		KeyBindingEntry->SetDataDisplayName(ActionDisplayName);

		if (const UInputAction* InputAction = Cast<UInputAction>(MappingsForAction[0]->GetAssociatedInputAction()))
		{
			KeyBindingEntry->SetDescriptionRichText(InputAction->ActionDescription);
		}

		for (const FPlayerKeyMapping* PlayerMapping : MappingsForAction)
		{
			FKeyBindingSlotDescriptor Slot;
			Slot.PlayerMappableName = PlayerMapping->GetMappingName();
			Slot.MappableSlot = static_cast<uint8>(PlayerMapping->GetSlot());

			const FText MappingDisplayName = PlayerMapping->GetDisplayName();
			if (!MappingDisplayName.IsEmpty() && MappingDisplayName.ToString() != ActionDisplayName.ToString())
			{
				Slot.SlotDisplayName = MappingDisplayName;
			}
			else
			{
				const bool bIsGamepad = PlayerMapping->GetPrimaryDeviceType() == EHardwareDevicePrimaryType::Gamepad;
				Slot.SlotDisplayName = bIsGamepad ? TXT_VAL_GAMEPAD() : TXT_VAL_KEYBOARD();
			}

			const bool bIsGamepadKey = PlayerMapping->GetCurrentKey().IsGamepadKey();
			Slot.bAllowKeyboardAndMouse = !bIsGamepadKey;
			Slot.bAllowGamepad = bIsGamepadKey;

			KeyBindingEntry->AddKeyBindingSlot(Slot);
		}

		TargetCategory->AddChildListData(KeyBindingEntry);
	}
}

// ============================================================================
// Data-driven init — uses UMenuConfigData DataAsset
// ============================================================================

/**
 * Build the options hierarchy from UMenuConfigData DataAsset.
 *
 * Walks Tabs → Categories → Entries, creating UListDataObject_Base instances
 * via CreateEntryFromConfig and wiring up EditConditions for cross-entry dependencies
 * (e.g. WindowMode affects Resolution, VSync requires Fullscreen, OverallQuality
 * bidirectionally links with quality presets).
 */
void UOptionsDataRegistry::InitFromDataAsset(ULocalPlayer* InOwningLocalPlayer)
{
	FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
	PackagedBuildOnlyCondition.SetEditConditionFunction(
		[]()->bool { return !(GIsEditor || GIsPlayInEditorWorld); });
	PackagedBuildOnlyCondition.SetDisableRichReason(TXT_OPT_WARN_PACKAGED_ONLY().ToString());

	for (UMenuTabConfig* TabData : OptionsConfig->Tabs)
	{
		UListDataObject_Collection* Tab = CreateCollection(TabData->TabId, TabData->TabName);
		TMap<FName, UListDataObject_Base*> EntryLookup;

		for (UMenuCategoryConfig* CatData : TabData->Categories)
		{
			UListDataObject_Collection* Cat = CreateCollection(CatData->CategoryId, CatData->CategoryName);

			for (UMenuEntryConfig* EntryData : CatData->Entries)
			{
				UListDataObject_Base* Entry = CreateEntryFromConfig(EntryData);
				if (!Entry) continue;

				Cat->AddChildListData(Entry);
				EntryLookup.Add(EntryData->DataId, Entry);

				// Wire BindingKey → getter/setter for Options entries
				if (!EntryData->BindingKey.IsNone())
				{
					if (auto Helper = ResolveBinding(EntryData->BindingKey))
					{
						if (UListDataObject_Value* ValueEntry = Cast<UListDataObject_Value>(Entry))
						{
							ValueEntry->SetDataDynamicGetter(Helper);
							ValueEntry->SetDataDynamicSetter(Helper);
						}
					}
				}
			}

			Tab->AddChildListData(Cat);
		}

		ApplyTabEditConditions(Tab, TabData, PackagedBuildOnlyCondition, EntryLookup);

		RegisteredOptionsCollectionTabs.Add(Tab);
	}

	InitControlCollectionTab(InOwningLocalPlayer);
}

void UOptionsDataRegistry::ApplyTabEditConditions(
	UListDataObject_Collection* Tab,
	UMenuTabConfig* TabData,
	const FOptionsDataEditConditionDescriptor& PackagedBuildOnlyCondition,
	const TMap<FName, UListDataObject_Base*>& EntryLookup)
{
	// ---- PackagedBuildOnly via DataAsset flag ----
	for (const UMenuCategoryConfig* CatData : TabData->Categories)
	{
		for (const UMenuEntryConfig* EntryData : CatData->Entries)
		{
			if (!EntryData->bPackagedBuildOnly) continue;
			UListDataObject_Base* const* EntryPtr = EntryLookup.Find(EntryData->DataId);
			if (EntryPtr && *EntryPtr)
			{
				(*EntryPtr)->AddEditConditionDescriptor(PackagedBuildOnlyCondition);
			}
		}
	}

	// ---- EditDependencyIds wiring ----
	for (const UMenuCategoryConfig* CatData : TabData->Categories)
	{
		for (const UMenuEntryConfig* EntryData : CatData->Entries)
		{
			if (EntryData->EditDependencyIds.IsEmpty()) continue;
			UListDataObject_Base* const* EntryPtr = EntryLookup.Find(EntryData->DataId);
			if (!EntryPtr || !*EntryPtr) continue;

			for (const FName& DepId : EntryData->EditDependencyIds)
			{
				UListDataObject_Base* const* DepPtr = EntryLookup.Find(DepId);
				if (DepPtr && *DepPtr)
				{
					(*EntryPtr)->AddEditDependencyData(*DepPtr);
				}
			}
		}
	}

	// ---- WindowMode-dependent EditConditions (hardcoded — enum-specific lambdas) ----
	if (TabData->TabId != "Tab-Video") return;

	UListDataObject_Base* const* WindowModePtr = EntryLookup.Find("WindowMode");
	UListDataObject_Base* const* ResolutionPtr = EntryLookup.Find("Resolution");
	UListDataObject_Base* const* VSyncPtr = EntryLookup.Find("Entry-VSync");

	UListDataObject_StringEnum* CreatedWindowMode = WindowModePtr ? Cast<UListDataObject_StringEnum>(*WindowModePtr) : nullptr;

	if (ResolutionPtr && *ResolutionPtr)
	{
		UListDataObject_StringResolution* Res = Cast<UListDataObject_StringResolution>(*ResolutionPtr);
		if (Res && CreatedWindowMode)
		{
			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunction(
				[CreatedWindowMode]()->bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() != EWindowMode::WindowedFullscreen;
				});
			WindowModeEditCondition.SetDisableRichReason(TXT_OPT_WARN_BORDERLESS_DISABLED().ToString());
			WindowModeEditCondition.SetDisabledForcedStringValue(Res->GetMaxAllowedResolution());
			Res->AddEditConditionDescriptor(WindowModeEditCondition);
			Res->AddEditDependencyData(CreatedWindowMode);
		}
	}

	if (VSyncPtr && *VSyncPtr && CreatedWindowMode)
	{
		FOptionsDataEditConditionDescriptor FullscreenOnlyCondition;
		FullscreenOnlyCondition.SetEditConditionFunction(
			[CreatedWindowMode]()->bool
			{
				return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
			});
		FullscreenOnlyCondition.SetDisableRichReason(TXT_OPT_WARN_FULLSCREEN_ONLY().ToString());
		FullscreenOnlyCondition.SetDisabledForcedStringValue("false");
		(*VSyncPtr)->AddEditConditionDescriptor(FullscreenOnlyCondition);
	}

	// ---- OverallQuality bidirectional: hub depends on every sub-entry that references it ----
	UListDataObject_Base* const* OverallQualityPtr = EntryLookup.Find("OverallQuality");
	if (OverallQualityPtr && *OverallQualityPtr)
	{
		for (const UMenuCategoryConfig* CatData : TabData->Categories)
		{
			for (const UMenuEntryConfig* EntryData : CatData->Entries)
			{
				if (!EntryData->EditDependencyIds.Contains("OverallQuality")) continue;
				UListDataObject_Base* const* SubEntryPtr = EntryLookup.Find(EntryData->DataId);
				if (SubEntryPtr && *SubEntryPtr)
				{
					(*OverallQualityPtr)->AddEditDependencyData(*SubEntryPtr);
				}
			}
		}
	}
}
