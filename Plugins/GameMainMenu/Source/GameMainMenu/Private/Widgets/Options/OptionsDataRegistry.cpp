// HangHang All Rights Reserved


#include "Widgets/Options/OptionsDataRegistry.h"

#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringCulture.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "Widgets/Options/DataObjects/UListDataObject_KeyBinding.h"
#include "Widgets/Options/DataObjects/OptionsConfigData.h"
#include "LocalizationKeys.h"

#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "InputAction.h"
#include "FrontendDebugHelper.h"

// ============================================================================
// Factory helpers
// ============================================================================

#define CREATE_HELPER(SetterOrGetterFuncName) \
	MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, SetterOrGetterFuncName))

UListDataObject_Collection* UOptionsDataRegistry::CreateCollection(const FName& Id, const FText& DisplayName) const
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}

UListDataObject_StringBool* UOptionsDataRegistry::CreateBoolEntry(
	const FName& Id, const FText& DisplayName, const FText& Description,
	const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
	const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
	bool bDefaultValue) const
{
	UListDataObject_StringBool* Entry = NewObject<UListDataObject_StringBool>();
	Entry->SetDataId(Id);
	Entry->SetDataDisplayName(DisplayName);
	Entry->SetDescriptionRichText(Description);
	Entry->SetDataDynamicGetter(Getter);
	Entry->SetDataDynamicSetter(Setter);
	Entry->SetShouldApplyChangeImmediately(true);
	bDefaultValue ? Entry->SetTrueAsDefaultValue() : Entry->SetFalseAsDefaultValue();
	return Entry;
}

UListDataObject_Scalar* UOptionsDataRegistry::CreateScalarEntry(
	const FName& Id, const FText& DisplayName, const FText& Description,
	const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
	const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
	const TRange<float>& DisplayRange, const TRange<float>& OutRange,
	float StepSize, float DefaultValue,
	ECommonNumericType NumericType) const
{
	UListDataObject_Scalar* Entry = NewObject<UListDataObject_Scalar>();
	Entry->SetDataId(Id);
	Entry->SetDataDisplayName(DisplayName);
	Entry->SetDescriptionRichText(Description);
	Entry->SetDisplayValueRange(DisplayRange);
	Entry->SetOutValueRange(OutRange);
	Entry->SetSliderStepSize(StepSize);
	Entry->SetDefaultValueFromString(LexToString(DefaultValue));
	Entry->SetCommonNumericType(NumericType);
	Entry->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
	Entry->SetDataDynamicGetter(Getter);
	Entry->SetDataDynamicSetter(Setter);
	Entry->SetShouldApplyChangeImmediately(true);
	return Entry;
}

UListDataObject_String* UOptionsDataRegistry::CreateStringEntry(
	const FName& Id, const FText& DisplayName, const FText& Description,
	const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
	const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
	const TArray<TPair<FString, FText>>& Options,
	const FString& DefaultValue) const
{
	UListDataObject_String* Entry = NewObject<UListDataObject_String>();
	Entry->SetDataId(Id);
	Entry->SetDataDisplayName(DisplayName);
	Entry->SetDescriptionRichText(Description);
	for (const auto& Pair : Options)
	{
		Entry->AddDynamicString(Pair.Key, Pair.Value);
	}
	Entry->SetDefaultValueFromString(DefaultValue);
	Entry->SetDataDynamicGetter(Getter);
	Entry->SetDataDynamicSetter(Setter);
	Entry->SetShouldApplyChangeImmediately(true);
	return Entry;
}

UListDataObject_StringInteger* UOptionsDataRegistry::CreateQualityPresetEntry(
	const FName& Id, const FText& DisplayName, const FText& Description,
	const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
	const TSharedPtr<FOptionsDataInteractionHelper>& Setter) const
{
	UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
	Entry->SetDataId(Id);
	Entry->SetDataDisplayName(DisplayName);
	Entry->SetDescriptionRichText(Description);
	Entry->AddIntegerOption(0, TXT_VAL_QUALITY_LOW());
	Entry->AddIntegerOption(1, TXT_VAL_QUALITY_MEDIUM());
	Entry->AddIntegerOption(2, TXT_VAL_QUALITY_HIGH());
	Entry->AddIntegerOption(3, TXT_VAL_QUALITY_EPIC());
	Entry->AddIntegerOption(4, TXT_VAL_QUALITY_CINEMATIC());
	Entry->SetDataDynamicGetter(Getter);
	Entry->SetDataDynamicSetter(Setter);
	Entry->SetShouldApplyChangeImmediately(true);
	return Entry;
}

// ============================================================================
// Public interface
// ============================================================================

void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
	if (OptionsConfig)
	{
		InitFromDataAsset(InOwningLocalPlayer);
		return;
	}

	// Fallback: hardcoded init (legacy path without DataAsset)
	InitGameplayCollectionTab();
	InitAudioCollectionTab();
	InitVideoCollectionTab();
	InitControlCollectionTab(InOwningLocalPlayer);
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
// Gameplay Tab
// ============================================================================

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* Tab = CreateCollection("Tab-Gameplay", TXT_OPT_TAB_GAMEPLAY());

	// ---- HUD ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-HUD", TXT_OPT_CAT_HUD());
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateBoolEntry("Entry-HealthBar",
			TXT_OPT_HEALTH_BAR(),
			TXT_OPT_HEALTH_BAR_DESC(),
			CREATE_HELPER(GetShowHealthBar), CREATE_HELPER(SetShowHealthBar), true));

		Cat->AddChildListData(CreateBoolEntry("Entry-DamageNumbers",
			TXT_OPT_DAMAGE_NUMBERS(),
			TXT_OPT_DAMAGE_NUMBERS_DESC(),
			CREATE_HELPER(GetShowDamageNumbers), CREATE_HELPER(SetShowDamageNumbers), true));

		Cat->AddChildListData(CreateBoolEntry("Entry-HighlightInteraction",
			TXT_OPT_HIGHLIGHT_INTERACTION(),
			TXT_OPT_HIGHLIGHT_INTERACTION_DESC(),
			CREATE_HELPER(GetHighlightInteraction), CREATE_HELPER(SetHighlightInteraction), true));

		Cat->AddChildListData(CreateBoolEntry("Entry-Subtitles",
			TXT_OPT_SUBTITLES(),
			TXT_OPT_SUBTITLES_DESC(),
			CREATE_HELPER(GetShowSubtitles), CREATE_HELPER(SetShowSubtitles), true));
	}

	// ---- Game ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-Game", TXT_OPT_CAT_GAME());
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateStringEntry("Entry-Difficulty",
			TXT_OPT_DIFFICULTY(),
			TXT_OPT_DIFFICULTY_DESC(),
			CREATE_HELPER(GetDifficulty), CREATE_HELPER(SetDifficulty),
			{{TEXT("Easy"), TXT_VAL_DIFFICULTY_EASY()},
			 {TEXT("Normal"), TXT_VAL_DIFFICULTY_NORMAL()},
			 {TEXT("Hard"), TXT_VAL_DIFFICULTY_HARD()},
			 {TEXT("Nightmare"), TXT_VAL_DIFFICULTY_NIGHTMARE()}},
			TEXT("Normal")));

		Cat->AddChildListData(CreateBoolEntry("Entry-AutoSave",
			TXT_OPT_AUTO_SAVE(),
			TXT_OPT_AUTO_SAVE_DESC(),
			CREATE_HELPER(GetAutoSave), CREATE_HELPER(SetAutoSave), true));

		// Language — special type
		{
			UListDataObject_StringCulture* Language = NewObject<UListDataObject_StringCulture>();
			Language->SetDataId("Entry-Language");
			Language->SetDataDisplayName(TXT_OPT_LANGUAGE());
			Language->SetDescriptionRichText(TXT_OPT_LANGUAGE_DESC());
			Language->SetDefaultValueFromString(TEXT("en"));
			Language->SetDataDynamicGetter(CREATE_HELPER(GetCurrentCulture));
			Language->SetDataDynamicSetter(CREATE_HELPER(SetCurrentCulture));
			Language->SetShouldApplyChangeImmediately(true);
			Cat->AddChildListData(Language);
		}
	}

	RegisteredOptionsCollectionTabs.Add(Tab);
}

// ============================================================================
// Audio Tab
// ============================================================================

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	static const TRange<float> VolDisplayRange(0.f, 1.f);
	static const TRange<float> VolOutRange(0.f, 2.f);

	UListDataObject_Collection* Tab = CreateCollection("Tab-Audio", TXT_OPT_TAB_AUDIO());

	// ---- Volume ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-Volume", TXT_OPT_CAT_VOLUME());
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateScalarEntry("OverallVolume",
			TXT_OPT_OVERALL_VOLUME(),
			TXT_OPT_OVERALL_VOLUME_DESC(),
			CREATE_HELPER(GetOverallVolume), CREATE_HELPER(SetOverallVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("MusicVolume",
			TXT_OPT_MUSIC_VOLUME(),
			TXT_OPT_MUSIC_VOLUME_DESC(),
			CREATE_HELPER(GetMusicVolume), CREATE_HELPER(SetMusicVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("SFXVolume",
			TXT_OPT_SFX_VOLUME(),
			TXT_OPT_SFX_VOLUME_DESC(),
			CREATE_HELPER(GetSFXVolume), CREATE_HELPER(SetSFXVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("VoiceVolume",
			TXT_OPT_VOICE_VOLUME(),
			TXT_OPT_VOICE_VOLUME_DESC(),
			CREATE_HELPER(GetVoiceVolume), CREATE_HELPER(SetVoiceVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("AmbientVolume",
			TXT_OPT_AMBIENT_VOLUME(),
			TXT_OPT_AMBIENT_VOLUME_DESC(),
			CREATE_HELPER(GetAmbientVolume), CREATE_HELPER(SetAmbientVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("UIVolume",
			TXT_OPT_UI_VOLUME(),
			TXT_OPT_UI_VOLUME_DESC(),
			CREATE_HELPER(GetUIVolume), CREATE_HELPER(SetUIVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));
	}

	// ---- Sound ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-Sound", TXT_OPT_CAT_SOUND());
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateBoolEntry("Background-Sound",
			TXT_OPT_BACKGROUND_SOUND(),
			TXT_OPT_BACKGROUND_SOUND_DESC(),
			CREATE_HELPER(GetAllowBackgroundSound), CREATE_HELPER(SetAllowBackgroundSound), false));

		Cat->AddChildListData(CreateBoolEntry("Sound-Subtitles",
			TXT_OPT_SUBTITLES_SOUND(),
			TXT_OPT_SUBTITLES_SOUND_DESC(),
			CREATE_HELPER(GetSubtitlesEnabled), CREATE_HELPER(SetSubtitlesEnabled), true));

		Cat->AddChildListData(CreateBoolEntry("Sound-HRTF",
			TXT_OPT_3D_SPATIAL_AUDIO(),
			TXT_OPT_3D_SPATIAL_AUDIO_DESC(),
			CREATE_HELPER(GetHRTFEnabled), CREATE_HELPER(SetHRTFEnabled), true));

		Cat->AddChildListData(CreateBoolEntry("Sound-Mono",
			TXT_OPT_MONO_OUTPUT(),
			TXT_OPT_MONO_OUTPUT_DESC(),
			CREATE_HELPER(GetMonoOutput), CREATE_HELPER(SetMonoOutput), false));

		Cat->AddChildListData(CreateBoolEntry("Sound-DRC",
			TXT_OPT_DYNAMIC_RANGE(),
			TXT_OPT_DYNAMIC_RANGE_DESC(),
			CREATE_HELPER(GetDynamicRangeCompression), CREATE_HELPER(SetDynamicRangeCompression), false));

		Cat->AddChildListData(CreateBoolEntry("Sound-DialogueBoost",
			TXT_OPT_DIALOGUE_BOOST(),
			TXT_OPT_DIALOGUE_BOOST_DESC(),
			CREATE_HELPER(GetDialogueBoost), CREATE_HELPER(SetDialogueBoost), true));
	}

	RegisteredOptionsCollectionTabs.Add(Tab);
}

// ============================================================================
// Video Tab
// ============================================================================

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* Tab = CreateCollection("Tab-Video", TXT_OPT_TAB_VIDEO());

	UListDataObject_StringEnum* CreatedWindowMode = nullptr;

	FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
	PackagedBuildOnlyCondition.SetEditConditionFunction(
		[]()->bool
		{
			return !(GIsEditor || GIsPlayInEditorWorld);
		});
	PackagedBuildOnlyCondition.SetDisableRichReason(TXT_OPT_WARN_PACKAGED_ONLY().ToString());

	// ---- Display ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Tab-Display", TXT_OPT_CAT_DISPLAY());
		Tab->AddChildListData(Cat);

		// Window Mode — special Enum type with EditCondition
		{
			UListDataObject_StringEnum* WindowMode = NewObject<UListDataObject_StringEnum>();
			WindowMode->SetDataId("WindowMode");
			WindowMode->SetDataDisplayName(TXT_OPT_WINDOW_MODE());
			WindowMode->SetDescriptionRichText(TXT_OPT_WINDOW_MODE_DESC());
			WindowMode->AddEnumOption(EWindowMode::Fullscreen, TXT_VAL_WINDOW_FULLSCREEN());
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, TXT_VAL_WINDOW_BORDERLESS());
			WindowMode->AddEnumOption(EWindowMode::Windowed, TXT_VAL_WINDOW_WINDOWED());
			WindowMode->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);
			WindowMode->SetDataDynamicGetter(CREATE_HELPER(GetFullscreenMode));
			WindowMode->SetDataDynamicSetter(CREATE_HELPER(SetFullscreenMode));
			WindowMode->SetShouldApplyChangeImmediately(true);
			WindowMode->AddEditConditionDescriptor(PackagedBuildOnlyCondition);
			CreatedWindowMode = WindowMode;
			Cat->AddChildListData(WindowMode);
		}

		// Resolution — special Resolution type
		{
			UListDataObject_StringResolution* Resolution = NewObject<UListDataObject_StringResolution>();
			Resolution->SetDataId("Resolution");
			Resolution->SetDataDisplayName(TXT_OPT_RESOLUTION());
			Resolution->SetDescriptionRichText(TXT_OPT_RESOLUTION_DESC());
			Resolution->InitResolutionValues();
			Resolution->SetDataDynamicGetter(CREATE_HELPER(GetScreenResolution));
			Resolution->SetDataDynamicSetter(CREATE_HELPER(SetScreenResolution));
			Resolution->SetShouldApplyChangeImmediately(true);
			Resolution->AddEditConditionDescriptor(PackagedBuildOnlyCondition);

			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunction(
				[CreatedWindowMode]()->bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() != EWindowMode::WindowedFullscreen;
				});
			WindowModeEditCondition.SetDisableRichReason(TXT_OPT_WARN_BORDERLESS_DISABLED().ToString());
			WindowModeEditCondition.SetDisabledForcedStringValue(Resolution->GetMaxAllowedResolution());
			Resolution->AddEditConditionDescriptor(WindowModeEditCondition);
			Resolution->AddEditDependencyData(CreatedWindowMode);

			Cat->AddChildListData(Resolution);
		}
	}

	// ---- Graphics ----
	UListDataObject_StringInteger* CreatedOverallQuality = nullptr;
	{
		UListDataObject_Collection* Cat = CreateCollection("GraphicsCategoryCollection", TXT_OPT_CAT_GRAPHICS());
		Tab->AddChildListData(Cat);

		// Display Gamma — scalar with non-standard ranges
		Cat->AddChildListData(CreateScalarEntry("DisplayGamma",
			TXT_OPT_DISPLAY_GAMMA(),
			TXT_OPT_DISPLAY_GAMMA_DESC(),
			CREATE_HELPER(GetCurrentDisplayGamma), CREATE_HELPER(SetCurrentDisplayGamma),
			TRange<float>(0.f, 1.f), TRange<float>(1.7f, 2.7f), 0.01f, 2.2f,
			ECommonNumericType::Percentage));

		// Overall Quality
		{
			CreatedOverallQuality = CreateQualityPresetEntry("OverallQuality",
				TXT_OPT_OVERALL_QUALITY(),
				TXT_OPT_OVERALL_QUALITY_DESC(),
				CREATE_HELPER(GetOverallScalabilityLevel), CREATE_HELPER(SetOverallScalabilityLevel));
			Cat->AddChildListData(CreatedOverallQuality);
		}

		// Resolution Scale — scalar with non-standard out-range
		{
			UListDataObject_Scalar* ResolutionScale = CreateScalarEntry("ResolutionScale",
				TXT_OPT_RESOLUTION_SCALE(),
				TXT_OPT_RESOLUTION_SCALE_DESC(),
				CREATE_HELPER(GetResolutionScaleNormalized), CREATE_HELPER(SetResolutionScaleNormalized),
				TRange<float>(0.f, 1.f), TRange<float>(0.5f, 1.f), 0.01f, 1.f,
				ECommonNumericType::Percentage);
			ResolutionScale->AddEditDependencyData(CreatedOverallQuality);
			Cat->AddChildListData(ResolutionScale);
		}

		// Quality sub-entries — each links bidirectionally with OverallQuality
		auto AddQualitySubEntry = [&](const FName& Id, const FText& Name, const FText& Desc,
			const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
			const TSharedPtr<FOptionsDataInteractionHelper>& Setter)
		{
			UListDataObject_StringInteger* Entry = CreateQualityPresetEntry(Id, Name, Desc, Getter, Setter);
			Entry->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(Entry);
			Cat->AddChildListData(Entry);
		};

		AddQualitySubEntry("GlobalIlluminationQuality",
			TXT_OPT_GI_QUALITY(),
			TXT_OPT_GI_QUALITY_DESC(),
			CREATE_HELPER(GetGlobalIlluminationQuality), CREATE_HELPER(SetGlobalIlluminationQuality));

		AddQualitySubEntry("ShadowQuality",
			TXT_OPT_SHADOW_QUALITY(),
			TXT_OPT_SHADOW_QUALITY_DESC(),
			CREATE_HELPER(GetShadowQuality), CREATE_HELPER(SetShadowQuality));

		AddQualitySubEntry("AntiAliasingQuality",
			TXT_OPT_AA_QUALITY(),
			TXT_OPT_AA_QUALITY_DESC(),
			CREATE_HELPER(GetAntiAliasingQuality), CREATE_HELPER(SetAntiAliasingQuality));

		AddQualitySubEntry("ViewDistanceQuality",
			TXT_OPT_VIEW_DISTANCE_QUALITY(),
			TXT_OPT_VIEW_DISTANCE_QUALITY_DESC(),
			CREATE_HELPER(GetViewDistanceQuality), CREATE_HELPER(SetViewDistanceQuality));

		AddQualitySubEntry("TextureQuality",
			TXT_OPT_TEXTURE_QUALITY(),
			TXT_OPT_TEXTURE_QUALITY_DESC(),
			CREATE_HELPER(GetTextureQuality), CREATE_HELPER(SetTextureQuality));

		AddQualitySubEntry("VisualEffectsQuality",
			TXT_OPT_VFX_QUALITY(),
			TXT_OPT_VFX_QUALITY_DESC(),
			CREATE_HELPER(GetVisualEffectQuality), CREATE_HELPER(SetVisualEffectQuality));

		AddQualitySubEntry("ReflectionQuality",
			TXT_OPT_REFLECTION_QUALITY(),
			TXT_OPT_REFLECTION_QUALITY_DESC(),
			CREATE_HELPER(GetReflectionQuality), CREATE_HELPER(SetReflectionQuality));

		AddQualitySubEntry("PostProcessingQuality",
			TXT_OPT_POSTPROCESS_QUALITY(),
			TXT_OPT_POSTPROCESS_QUALITY_DESC(),
			CREATE_HELPER(GetPostProcessingQuality), CREATE_HELPER(SetPostProcessingQuality));
	}

	// ---- Advanced Graphics ----
	{
		UListDataObject_Collection* Cat = CreateCollection("AdvancedGraphicsCategoryCollection", TXT_OPT_CAT_ADVANCED_GRAPHICS());
		Tab->AddChildListData(Cat);

		// VSync — bool with special EditCondition
		{
			UListDataObject_StringBool* VSync = CreateBoolEntry("Entry-VSync",
				TXT_OPT_VSYNC(),
				TXT_OPT_VSYNC_DESC(),
				CREATE_HELPER(IsVSyncEnabled), CREATE_HELPER(SetVSyncEnabled), true);

			FOptionsDataEditConditionDescriptor FullscreenOnlyCondition;
			FullscreenOnlyCondition.SetEditConditionFunction(
				[CreatedWindowMode]()->bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
				});
			FullscreenOnlyCondition.SetDisableRichReason(TXT_OPT_WARN_FULLSCREEN_ONLY().ToString());
			FullscreenOnlyCondition.SetDisabledForcedStringValue("false");
			VSync->AddEditConditionDescriptor(FullscreenOnlyCondition);

			Cat->AddChildListData(VSync);
		}

		// Frame Rate Limit
		Cat->AddChildListData(CreateStringEntry("Entry-FrameRateLimit",
			TXT_OPT_FRAME_RATE_LIMIT(),
			TXT_OPT_FRAME_RATE_LIMIT_DESC(),
			CREATE_HELPER(GetFrameRateLimit), CREATE_HELPER(SetFrameRateLimit),
			{{LexToString(0.0f), TXT_VAL_FPS_UNLIMITED()},
			 {LexToString(30.0f), TXT_VAL_FPS_30()},
			 {LexToString(60.0f), TXT_VAL_FPS_60()},
			 {LexToString(120.0f), TXT_VAL_FPS_120()},
			 {LexToString(144.0f), TXT_VAL_FPS_144()},
			 {LexToString(240.0f), TXT_VAL_FPS_240()}},
			LexToString(0.0f)));
	}

	RegisteredOptionsCollectionTabs.Add(Tab);
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
// Data-driven init — uses UOptionsConfigData DataAsset
// ============================================================================

/**
 * Build the options hierarchy from UOptionsConfigData DataAsset.
 *
 * Walks Tabs → Categories → Entries, creating UListDataObject_Base instances
 * via CreateEntryFromData and wiring up EditConditions for cross-entry dependencies
 * (e.g. WindowMode affects Resolution, VSync requires Fullscreen, OverallQuality
 * bidirectionally links with quality presets).
 */
void UOptionsDataRegistry::InitFromDataAsset(ULocalPlayer* InOwningLocalPlayer)
{
	FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
	PackagedBuildOnlyCondition.SetEditConditionFunction(
		[]()->bool { return !(GIsEditor || GIsPlayInEditorWorld); });
	PackagedBuildOnlyCondition.SetDisableRichReason(TXT_OPT_WARN_PACKAGED_ONLY().ToString());

	for (UOptionTabData* TabData : OptionsConfig->Tabs)
	{
		UListDataObject_Collection* Tab = CreateCollection(TabData->TabId, TabData->TabName);
		TMap<FName, UListDataObject_Base*> EntryLookup;

		for (UOptionCategoryData* CatData : TabData->Categories)
		{
			UListDataObject_Collection* Cat = CreateCollection(CatData->CategoryId, CatData->CategoryName);

			for (UOptionEntryData* EntryData : CatData->Entries)
			{
				UListDataObject_Base* Entry = CreateEntryFromData(EntryData);
				if (!Entry) continue;

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
	UOptionTabData* TabData,
	const FOptionsDataEditConditionDescriptor& PackagedBuildOnlyCondition,
	const TMap<FName, UListDataObject_Base*>& EntryLookup)
{
	// ---- PackagedBuildOnly via DataAsset flag ----
	for (const UOptionCategoryData* CatData : TabData->Categories)
	{
		for (const UOptionEntryData* EntryData : CatData->Entries)
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
	for (const UOptionCategoryData* CatData : TabData->Categories)
	{
		for (const UOptionEntryData* EntryData : CatData->Entries)
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
		for (const UOptionCategoryData* CatData : TabData->Categories)
		{
			for (const UOptionEntryData* EntryData : CatData->Entries)
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
