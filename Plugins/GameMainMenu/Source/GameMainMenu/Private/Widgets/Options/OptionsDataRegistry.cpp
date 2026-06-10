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

#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
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
	Entry->AddIntegerOption(0, FText::FromString("Low"));
	Entry->AddIntegerOption(1, FText::FromString("Medium"));
	Entry->AddIntegerOption(2, FText::FromString("High"));
	Entry->AddIntegerOption(3, FText::FromString("Epic"));
	Entry->AddIntegerOption(4, FText::FromString("Cinematic"));
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

	checkf(FoundCollectionTabPointer,TEXT("No valid found under the ID %s"),*InSelectedTabId.ToString());

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
	UListDataObject_Collection* Tab = CreateCollection("Tab-Gameplay", FText::FromString("Gameplay"));

	// ---- HUD ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-HUD", FText::FromString("HUD"));
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateBoolEntry("Entry-HealthBar",
			FText::FromString("Health Bar"),
			FText::FromString("Toggle the display of enemy health bars."),
			CREATE_HELPER(GetShowHealthBar), CREATE_HELPER(SetShowHealthBar), true));

		Cat->AddChildListData(CreateBoolEntry("Entry-DamageNumbers",
			FText::FromString("Damage Numbers"),
			FText::FromString("Toggle the display of floating damage numbers."),
			CREATE_HELPER(GetShowDamageNumbers), CREATE_HELPER(SetShowDamageNumbers), true));

		Cat->AddChildListData(CreateBoolEntry("Entry-HighlightInteraction",
			FText::FromString("Highlight Interaction"),
			FText::FromString("Toggle the highlight outline on interactive objects."),
			CREATE_HELPER(GetHighlightInteraction), CREATE_HELPER(SetHighlightInteraction), true));

		Cat->AddChildListData(CreateBoolEntry("Entry-Subtitles",
			FText::FromString("Subtitles"),
			FText::FromString("Toggle subtitle display during dialogues and cutscenes."),
			CREATE_HELPER(GetShowSubtitles), CREATE_HELPER(SetShowSubtitles), true));
	}

	// ---- Game ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-Game", FText::FromString("Game"));
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateStringEntry("Entry-Difficulty",
			FText::FromString("Difficulty"),
			FText::FromString("Set the game difficulty level."),
			CREATE_HELPER(GetDifficulty), CREATE_HELPER(SetDifficulty),
			{{TEXT("Easy"), FText::FromString("Easy")},
			 {TEXT("Normal"), FText::FromString("Normal")},
			 {TEXT("Hard"), FText::FromString("Hard")},
			 {TEXT("Nightmare"), FText::FromString("Nightmare")}},
			TEXT("Normal")));

		Cat->AddChildListData(CreateBoolEntry("Entry-AutoSave",
			FText::FromString("Auto Save"),
			FText::FromString("Toggle automatic game progress saving."),
			CREATE_HELPER(GetAutoSave), CREATE_HELPER(SetAutoSave), true));

		// Language — special type
		{
			UListDataObject_StringCulture* Language = NewObject<UListDataObject_StringCulture>();
			Language->SetDataId("Entry-Language");
			Language->SetDataDisplayName(FText::FromString("Language"));
			Language->SetDescriptionRichText(FText::FromString("Select the display language. Changes take effect immediately."));
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

	UListDataObject_Collection* Tab = CreateCollection("Tab-Audio", FText::FromString("Audio"));

	// ---- Volume ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-Volume", FText::FromString("Volume"));
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateScalarEntry("OverallVolume",
			FText::FromString("Overall Volume"),
			FText::FromString("Adjust the overall volume level of the game."),
			CREATE_HELPER(GetOverallVolume), CREATE_HELPER(SetOverallVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("MusicVolume",
			FText::FromString("Music Volume"),
			FText::FromString("Adjust the music volume level."),
			CREATE_HELPER(GetMusicVolume), CREATE_HELPER(SetMusicVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("SFXVolume",
			FText::FromString("SFX Volume"),
			FText::FromString("Adjust the sound effects volume level."),
			CREATE_HELPER(GetSFXVolume), CREATE_HELPER(SetSFXVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("VoiceVolume",
			FText::FromString("Voice Volume"),
			FText::FromString("Adjust the voice/dialogue volume level."),
			CREATE_HELPER(GetVoiceVolume), CREATE_HELPER(SetVoiceVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("AmbientVolume",
			FText::FromString("Ambient Volume"),
			FText::FromString("Adjust the ambient sounds volume level."),
			CREATE_HELPER(GetAmbientVolume), CREATE_HELPER(SetAmbientVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));

		Cat->AddChildListData(CreateScalarEntry("UIVolume",
			FText::FromString("UI Volume"),
			FText::FromString("Adjust the UI sounds volume level."),
			CREATE_HELPER(GetUIVolume), CREATE_HELPER(SetUIVolume),
			VolDisplayRange, VolOutRange, 0.01f, 1.f, ECommonNumericType::Percentage));
	}

	// ---- Sound ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Category-Sound", FText::FromString("Sound"));
		Tab->AddChildListData(Cat);

		Cat->AddChildListData(CreateBoolEntry("Background-Sound",
			FText::FromString("Background Sound"),
			FText::FromString("Allow the game to continue playing sounds when it's running in the background."),
			CREATE_HELPER(GetAllowBackgroundSound), CREATE_HELPER(SetAllowBackgroundSound), false));

		Cat->AddChildListData(CreateBoolEntry("Sound-Subtitles",
			FText::FromString("Subtitles"),
			FText::FromString("Toggle subtitle display during dialogues and cutscenes."),
			CREATE_HELPER(GetSubtitlesEnabled), CREATE_HELPER(SetSubtitlesEnabled), true));

		Cat->AddChildListData(CreateBoolEntry("Sound-HRTF",
			FText::FromString("3D Spatial Audio"),
			FText::FromString("Enable HRTF-based 3D spatial audio for headphones. Provides immersive directional sound."),
			CREATE_HELPER(GetHRTFEnabled), CREATE_HELPER(SetHRTFEnabled), true));

		Cat->AddChildListData(CreateBoolEntry("Sound-Mono",
			FText::FromString("Mono Output"),
			FText::FromString("Downmix all audio to mono. Useful for single-speaker setups or accessibility needs."),
			CREATE_HELPER(GetMonoOutput), CREATE_HELPER(SetMonoOutput), false));

		Cat->AddChildListData(CreateBoolEntry("Sound-DRC",
			FText::FromString("Dynamic Range Compression"),
			FText::FromString("Reduce the volume difference between loud and quiet sounds. Ideal for late-night gaming."),
			CREATE_HELPER(GetDynamicRangeCompression), CREATE_HELPER(SetDynamicRangeCompression), false));

		Cat->AddChildListData(CreateBoolEntry("Sound-DialogueBoost",
			FText::FromString("Dialogue Boost"),
			FText::FromString("Enhance voice and dialogue clarity by boosting mid-range frequencies."),
			CREATE_HELPER(GetDialogueBoost), CREATE_HELPER(SetDialogueBoost), true));
	}

	RegisteredOptionsCollectionTabs.Add(Tab);
}

// ============================================================================
// Video Tab
// ============================================================================

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* Tab = CreateCollection("Tab-Video", FText::FromString("Video"));

	UListDataObject_StringEnum* CreatedWindowMode = nullptr;

	FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
	PackagedBuildOnlyCondition.SetEditConditionFunction(
		[]()->bool
		{
			return !(GIsEditor || GIsPlayInEditorWorld);
		});
	PackagedBuildOnlyCondition.SetDisableRichReason(TEXT("\n\n<Warning>This setting can only adjusted in a packaged build</>"));

	// ---- Display ----
	{
		UListDataObject_Collection* Cat = CreateCollection("Tab-Display", FText::FromString("Display"));
		Tab->AddChildListData(Cat);

		// Window Mode — special Enum type with EditCondition
		{
			UListDataObject_StringEnum* WindowMode = NewObject<UListDataObject_StringEnum>();
			WindowMode->SetDataId("WindowMode");
			WindowMode->SetDataDisplayName(FText::FromString("Window Mode"));
			WindowMode->SetDescriptionRichText(FText::FromString("Select the window display mode"));
			WindowMode->AddEnumOption(EWindowMode::Fullscreen, FText::FromString("Fullscreen Mode"));
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString("Borderless Window"));
			WindowMode->AddEnumOption(EWindowMode::Windowed, FText::FromString("Windowed"));
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
			Resolution->SetDataDisplayName(FText::FromString("Resolution"));
			Resolution->SetDescriptionRichText(FText::FromString("Select the screen resolution for the game. Higher resolutions provide better visual quality but may impact performance."));
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
			WindowModeEditCondition.SetDisableRichReason(TEXT("\n\n<Warning>This setting is disabled in Borderless Window mode</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(Resolution->GetMaxAllowedResolution());
			Resolution->AddEditConditionDescriptor(WindowModeEditCondition);
			Resolution->AddEditDependencyData(CreatedWindowMode);

			Cat->AddChildListData(Resolution);
		}
	}

	// ---- Graphics ----
	UListDataObject_StringInteger* CreatedOverallQuality = nullptr;
	{
		UListDataObject_Collection* Cat = CreateCollection("GraphicsCategoryCollection", FText::FromString("Graphics"));
		Tab->AddChildListData(Cat);

		// Display Gamma — scalar with non-standard ranges
		Cat->AddChildListData(CreateScalarEntry("DisplayGamma",
			FText::FromString("Display Gamma"),
			FText::FromString("Adjust the display gamma correction value."),
			CREATE_HELPER(GetCurrentDisplayGamma), CREATE_HELPER(SetCurrentDisplayGamma),
			TRange<float>(0.f, 1.f), TRange<float>(1.7f, 2.7f), 0.01f, 2.2f,
			ECommonNumericType::Percentage));

		// Overall Quality
		{
			CreatedOverallQuality = CreateQualityPresetEntry("OverallQuality",
				FText::FromString("Overall Quality"),
				FText::FromString("Sets the overall graphics quality preset."),
				CREATE_HELPER(GetOverallScalabilityLevel), CREATE_HELPER(SetOverallScalabilityLevel));
			Cat->AddChildListData(CreatedOverallQuality);
		}

		// Resolution Scale — scalar with non-standard out-range
		{
			UListDataObject_Scalar* ResolutionScale = CreateScalarEntry("ResolutionScale",
				FText::FromString("Resolution Scale"),
				FText::FromString("Adjust the rendering resolution as a percentage of the native resolution. Lower values improve performance."),
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
			FText::FromString("Global Illumination Quality"),
			FText::FromString("Adjust the quality of global illumination lighting effects."),
			CREATE_HELPER(GetGlobalIlluminationQuality), CREATE_HELPER(SetGlobalIlluminationQuality));

		AddQualitySubEntry("ShadowQuality",
			FText::FromString("Shadow Quality"),
			FText::FromString("Adjust the quality and resolution of shadow rendering."),
			CREATE_HELPER(GetShadowQuality), CREATE_HELPER(SetShadowQuality));

		AddQualitySubEntry("AntiAliasingQuality",
			FText::FromString("AntiAliasing Quality"),
			FText::FromString("Adjust the quality of anti-aliasing for smoother edges."),
			CREATE_HELPER(GetAntiAliasingQuality), CREATE_HELPER(SetAntiAliasingQuality));

		AddQualitySubEntry("ViewDistanceQuality",
			FText::FromString("View Distance Quality"),
			FText::FromString("Adjust the view distance for rendering distant objects."),
			CREATE_HELPER(GetViewDistanceQuality), CREATE_HELPER(SetViewDistanceQuality));

		AddQualitySubEntry("TextureQuality",
			FText::FromString("Texture Quality"),
			FText::FromString("Adjust the texture resolution and streaming quality."),
			CREATE_HELPER(GetTextureQuality), CREATE_HELPER(SetTextureQuality));

		AddQualitySubEntry("VisualEffectsQuality",
			FText::FromString("Visual Effects Quality"),
			FText::FromString("Adjust the quality of visual effects such as particles and explosions."),
			CREATE_HELPER(GetVisualEffectQuality), CREATE_HELPER(SetVisualEffectQuality));

		AddQualitySubEntry("ReflectionQuality",
			FText::FromString("Reflection Quality"),
			FText::FromString("Adjust the quality of screen-space and planar reflections."),
			CREATE_HELPER(GetReflectionQuality), CREATE_HELPER(SetReflectionQuality));

		AddQualitySubEntry("PostProcessingQuality",
			FText::FromString("Post Processing Quality"),
			FText::FromString("Adjust the quality of post-processing effects such as bloom and depth of field."),
			CREATE_HELPER(GetPostProcessingQuality), CREATE_HELPER(SetPostProcessingQuality));
	}

	// ---- Advanced Graphics ----
	{
		UListDataObject_Collection* Cat = CreateCollection("AdvancedGraphicsCategoryCollection", FText::FromString("Advanced Graphics"));
		Tab->AddChildListData(Cat);

		// VSync — bool with special EditCondition
		{
			UListDataObject_StringBool* VSync = CreateBoolEntry("Entry-VSync",
				FText::FromString("Vertical Sync"),
				FText::FromString("Synchronize the frame rate with the display refresh rate to prevent screen tearing."),
				CREATE_HELPER(IsVSyncEnabled), CREATE_HELPER(SetVSyncEnabled), true);

			FOptionsDataEditConditionDescriptor FullscreenOnlyCondition;
			FullscreenOnlyCondition.SetEditConditionFunction(
				[CreatedWindowMode]()->bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
				});
			FullscreenOnlyCondition.SetDisableRichReason("\n\n<Warning>This setting is only available in Fullscreen mode</>");
			FullscreenOnlyCondition.SetDisabledForcedStringValue("false");
			VSync->AddEditConditionDescriptor(FullscreenOnlyCondition);

			Cat->AddChildListData(VSync);
		}

		// Frame Rate Limit
		Cat->AddChildListData(CreateStringEntry("Entry-FrameRateLimit",
			FText::FromString("Frame Rate Limit"),
			FText::FromString("Limit the maximum frames per second. Lower values reduce system load but may feel less responsive."),
			CREATE_HELPER(GetFrameRateLimit), CREATE_HELPER(SetFrameRateLimit),
			{{LexToString(0.0f), FText::FromString("Unlimited")},
			 {LexToString(30.0f), FText::FromString("30 FPS")},
			 {LexToString(60.0f), FText::FromString("60 FPS")},
			 {LexToString(120.0f), FText::FromString("120 FPS")},
			 {LexToString(144.0f), FText::FromString("144 FPS")},
			 {LexToString(240.0f), FText::FromString("240 FPS")}},
			LexToString(0.0f)));
	}

	RegisteredOptionsCollectionTabs.Add(Tab);
}

// ============================================================================
// Control Tab (Key Bindings — dynamic, no factory extraction)
// ============================================================================

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* Tab = CreateCollection("Tab-Control", FText::FromString("Control"));

	if (!InOwningLocalPlayer)
	{
		RegisteredOptionsCollectionTabs.Add(Tab);
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		RegisteredOptionsCollectionTabs.Add(Tab);
		return;
	}

	UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
	if (!UserSettings)
	{
		RegisteredOptionsCollectionTabs.Add(Tab);
		return;
	}

	UEnhancedPlayerMappableKeyProfile* ActiveProfile = UserSettings->GetActiveKeyProfile();
	if (!ActiveProfile)
	{
		RegisteredOptionsCollectionTabs.Add(Tab);
		return;
	}

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
			TargetCategory = CreateCollection(*CategoryKey, CategoryText.IsEmpty() ? FText::FromString("General") : CategoryText);
			Tab->AddChildListData(TargetCategory);
		}

		UUListDataObject_KeyBinding* KeyBindingEntry = NewObject<UUListDataObject_KeyBinding>();
		KeyBindingEntry->SetOwningLocalPlayer(InOwningLocalPlayer);
		KeyBindingEntry->SetDataId(ActionMappingName);
		KeyBindingEntry->SetDataDisplayName(ActionDisplayName);

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
				Slot.SlotDisplayName = bIsGamepad ? FText::FromString(TEXT("Gamepad")) : FText::FromString(TEXT("Keyboard"));
			}

			const bool bIsGamepadKey = PlayerMapping->GetCurrentKey().IsGamepadKey();
			Slot.bAllowKeyboardAndMouse = !bIsGamepadKey;
			Slot.bAllowGamepad = bIsGamepadKey;

			KeyBindingEntry->AddKeyBindingSlot(Slot);
		}

		TargetCategory->AddChildListData(KeyBindingEntry);
	}
	RegisteredOptionsCollectionTabs.Add(Tab);
}
