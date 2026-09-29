// HangHang All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"

#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/OptionsKeyBindingProfile.h"
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

	BindingMap.Reset();
	SettingCommitters.Reset();
	const TWeakObjectPtr<UFrontendGameUserSettings> Draft = PendingSettings;
	const auto Bind = [this, Draft](FName Key, auto Getter, auto Setter)
	{
		BindingMap.Add(Key, FOptionsDataInteractionHelper::Create([Draft]() { return Draft.Get(); }, Getter, Setter));
		SettingCommitters.Add([Draft, Getter, Setter](FSettings* Target)
		{
			if (Draft.IsValid()) (Target->*Setter)((Draft.Get()->*Getter)());
		});
	};

	// ---- Gameplay ----
	Bind(TEXT("Difficulty"), &FSettings::GetDifficulty, &FSettings::SetDifficulty);
	Bind(TEXT("ShowHealthBar"), &FSettings::GetShowHealthBar, &FSettings::SetShowHealthBar);
	Bind(TEXT("ShowDamageNumbers"), &FSettings::GetShowDamageNumbers, &FSettings::SetShowDamageNumbers);
	Bind(TEXT("HighlightInteraction"), &FSettings::GetHighlightInteraction, &FSettings::SetHighlightInteraction);
	Bind(TEXT("ShowSubtitles"), &FSettings::GetShowSubtitles, &FSettings::SetShowSubtitles);
	Bind(TEXT("AutoSave"), &FSettings::GetAutoSave, &FSettings::SetAutoSave);
	Bind(TEXT("CurrentCulture"), &FSettings::GetCurrentCulture, &FSettings::SetCurrentCulture);

	// ---- Audio ----
	Bind(TEXT("OverallVolume"), &FSettings::GetOverallVolume, &FSettings::SetOverallVolume);
	Bind(TEXT("MusicVolume"), &FSettings::GetMusicVolume, &FSettings::SetMusicVolume);
	Bind(TEXT("SFXVolume"), &FSettings::GetSFXVolume, &FSettings::SetSFXVolume);
	Bind(TEXT("VoiceVolume"), &FSettings::GetVoiceVolume, &FSettings::SetVoiceVolume);
	Bind(TEXT("AmbientVolume"), &FSettings::GetAmbientVolume, &FSettings::SetAmbientVolume);
	Bind(TEXT("UIVolume"), &FSettings::GetUIVolume, &FSettings::SetUIVolume);
	Bind(TEXT("AllowBackgroundSound"), &FSettings::GetAllowBackgroundSound, &FSettings::SetAllowBackgroundSound);
	Bind(TEXT("SubtitlesEnabled"), &FSettings::GetSubtitlesEnabled, &FSettings::SetSubtitlesEnabled);
	Bind(TEXT("HRTFEnabled"), &FSettings::GetHRTFEnabled, &FSettings::SetHRTFEnabled);
	Bind(TEXT("MonoOutput"), &FSettings::GetMonoOutput, &FSettings::SetMonoOutput);
	Bind(TEXT("DynamicRangeCompression"), &FSettings::GetDynamicRangeCompression, &FSettings::SetDynamicRangeCompression);
	Bind(TEXT("DialogueBoost"), &FSettings::GetDialogueBoost, &FSettings::SetDialogueBoost);

	// ---- Video ----
	Bind(TEXT("FullscreenMode"), &FSettings::GetFullscreenMode, &FSettings::SetFullscreenMode);
	Bind(TEXT("Resolution"), &FSettings::GetScreenResolution, &FSettings::SetScreenResolution);
	Bind(TEXT("DisplayGamma"), &FSettings::GetCurrentDisplayGamma, &FSettings::SetCurrentDisplayGamma);
	Bind(TEXT("OverallScalabilityLevel"), &FSettings::GetOverallScalabilityLevel, &FSettings::SetOverallScalabilityLevel);
	Bind(TEXT("ResolutionScaleNormalized"), &FSettings::GetResolutionScaleNormalized, &FSettings::SetResolutionScaleNormalized);
	Bind(TEXT("GlobalIlluminationQuality"), &FSettings::GetGlobalIlluminationQuality, &FSettings::SetGlobalIlluminationQuality);
	Bind(TEXT("ShadowQuality"), &FSettings::GetShadowQuality, &FSettings::SetShadowQuality);
	Bind(TEXT("AntiAliasingQuality"), &FSettings::GetAntiAliasingQuality, &FSettings::SetAntiAliasingQuality);
	Bind(TEXT("ViewDistanceQuality"), &FSettings::GetViewDistanceQuality, &FSettings::SetViewDistanceQuality);
	Bind(TEXT("TextureQuality"), &FSettings::GetTextureQuality, &FSettings::SetTextureQuality);
	Bind(TEXT("VisualEffectQuality"), &FSettings::GetVisualEffectQuality, &FSettings::SetVisualEffectQuality);
	Bind(TEXT("ReflectionQuality"), &FSettings::GetReflectionQuality, &FSettings::SetReflectionQuality);
	Bind(TEXT("PostProcessingQuality"), &FSettings::GetPostProcessingQuality, &FSettings::SetPostProcessingQuality);
	Bind(TEXT("VSync"), &FSettings::IsVSyncEnabled, &FSettings::SetVSyncEnabled);
	Bind(TEXT("FrameRateLimit"), &FSettings::GetFrameRateLimit, &FSettings::SetFrameRateLimit);
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
	UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();
	if (!ensure(Settings)) return;
	PendingSettings = DuplicateObject<UFrontendGameUserSettings>(Settings, this);
	// ScalabilityQuality is not a UPROPERTY, so UObject duplication does not copy it.
	PendingSettings->ScalabilityQuality = Settings->ScalabilityQuality;
	InitBindingMap();

	if (!ensureMsgf(OptionsConfig, TEXT("OptionsDataRegistry requires an OptionsConfig DataAsset.")))
	{
		return;
	}

	InitFromDataAsset(InOwningLocalPlayer);
}

void UOptionsDataRegistry::CommitPendingSettings(UFrontendGameUserSettings* TargetSettings)
{
	if (!TargetSettings || !PendingSettings) return;
	for (const auto& Commit : SettingCommitters) Commit(TargetSettings);
	// Preserve the complete custom quality state, including foliage/shading and resolution scale.
	TargetSettings->ScalabilityQuality = PendingSettings->ScalabilityQuality;

	UEnhancedInputUserSettings* InputSettings = InputUserSettings.Get();
	if (InputSettings && PendingKeyProfile)
	{
		for (const auto& Row : PendingKeyProfile->GetPlayerMappingRows())
		{
			for (const FPlayerKeyMapping& Mapping : Row.Value.Mappings)
			{
				FMapPlayerKeyArgs Args;
				Args.ProfileIdString = PendingKeyProfile->GetProfileIdString();
				Args.MappingName = Mapping.GetMappingName();
				Args.Slot = Mapping.GetSlot();
				Args.HardwareDeviceId = Mapping.GetHardwareDeviceId().HardwareDeviceIdentifier;
				Args.NewKey = Mapping.GetCurrentKey();
				FGameplayTagContainer FailureReason;
				InputSettings->MapPlayerKey(Args, FailureReason);
			}
		}
		InputSettings->ApplySettings();
		InputSettings->SaveSettings();
	}
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
	InputUserSettings = UserSettings;
	UOptionsKeyBindingProfile* DraftProfile = NewObject<UOptionsKeyBindingProfile>(this);
	DraftProfile->InitializeFromProfile(*ActiveProfile);
	PendingKeyProfile = DraftProfile;

	const TMap<FName, FKeyMappingRow>& MappingRows = PendingKeyProfile->GetPlayerMappingRows();

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
		KeyBindingEntry->SetKeyProfile(PendingKeyProfile);
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
				// Options are committed on Back, even for legacy assets marked immediate.
				Entry->SetShouldApplyChangeImmediately(false);

				// Wire BindingKey → getter/setter for Options entries
				if (!EntryData->BindingKey.IsNone())
				{
					if (auto Helper = ResolveBinding(EntryData->BindingKey))
					{
						if (UListDataObject_Value* ValueEntry = Cast<UListDataObject_Value>(Entry))
						{
							ValueEntry->SetBinding(Helper);
						}
					}
				}

				// Adding a child initializes it, so bind before it reads the current settings.
				Cat->AddChildListData(Entry);
				EntryLookup.Add(EntryData->DataId, Entry);
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
