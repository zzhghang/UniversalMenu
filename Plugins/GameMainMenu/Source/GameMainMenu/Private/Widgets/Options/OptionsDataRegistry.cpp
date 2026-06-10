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

#define MAKE_OPTIONS_DATA_CONTROL(SetterOrGetterFuncName) \
MakeShared<FOptionsDataInteractionHelper>(GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings,SetterOrGetterFuncName))

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

void UOptionsDataRegistry::InitGameplayCollectionTab()
{
	UListDataObject_Collection* GameplayCollectionTab = NewObject<UListDataObject_Collection>();

	GameplayCollectionTab->SetDataId(FName("Tab-Gameplay"));
	GameplayCollectionTab->SetDataDisplayName(FText::FromString("Gameplay"));

	// HUD Category
	{
		UListDataObject_Collection* HUDCategory = NewObject<UListDataObject_Collection>();
		HUDCategory->SetDataId(FName("Category-HUD"));
		HUDCategory->SetDataDisplayName(FText::FromString("HUD"));

		GameplayCollectionTab->AddChildListData(HUDCategory);

		//血条
		{
			UListDataObject_StringBool* HealthBarEntry = NewObject<UListDataObject_StringBool>();
			HealthBarEntry->SetDataId(FName("Entry-HealthBar"));
			HealthBarEntry->SetDataDisplayName(FText::FromString("Health Bar"));
			HealthBarEntry->SetDescriptionRichText(FText::FromString("Toggle the display of enemy health bars."));

			HealthBarEntry->SetTrueAsDefaultValue();
			HealthBarEntry->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShowHealthBar));
			HealthBarEntry->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShowHealthBar));

			HealthBarEntry->SetShouldApplyChangeImmediately(true);

			HUDCategory->AddChildListData(HealthBarEntry);
		}
		//伤害数字
		{
			UListDataObject_StringBool* DamageNumbersEntry = NewObject<UListDataObject_StringBool>();
			DamageNumbersEntry->SetDataId(FName("Entry-DamageNumbers"));
			DamageNumbersEntry->SetDataDisplayName(FText::FromString("Damage Numbers"));
			DamageNumbersEntry->SetDescriptionRichText(FText::FromString("Toggle the display of floating damage numbers."));

			DamageNumbersEntry->SetTrueAsDefaultValue();
			DamageNumbersEntry->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShowDamageNumbers));
			DamageNumbersEntry->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShowDamageNumbers));

			DamageNumbersEntry->SetShouldApplyChangeImmediately(true);

			HUDCategory->AddChildListData(DamageNumbersEntry);
		}
		//高亮互动
		{
			UListDataObject_StringBool* HighlightInteractionEntry = NewObject<UListDataObject_StringBool>();
			HighlightInteractionEntry->SetDataId(FName("Entry-HighlightInteraction"));
			HighlightInteractionEntry->SetDataDisplayName(FText::FromString("Highlight Interaction"));
			HighlightInteractionEntry->SetDescriptionRichText(FText::FromString("Toggle the highlight outline on interactive objects."));

			HighlightInteractionEntry->SetTrueAsDefaultValue();
			HighlightInteractionEntry->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetHighlightInteraction));
			HighlightInteractionEntry->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetHighlightInteraction));

			HighlightInteractionEntry->SetShouldApplyChangeImmediately(true);

			HUDCategory->AddChildListData(HighlightInteractionEntry);
		}
		//字幕显示
		{
			UListDataObject_StringBool* SubtitlesEntry = NewObject<UListDataObject_StringBool>();
			SubtitlesEntry->SetDataId(FName("Entry-Subtitles"));
			SubtitlesEntry->SetDataDisplayName(FText::FromString("Subtitles"));
			SubtitlesEntry->SetDescriptionRichText(FText::FromString("Toggle subtitle display during dialogues and cutscenes."));

			SubtitlesEntry->SetTrueAsDefaultValue();
			SubtitlesEntry->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShowSubtitles));
			SubtitlesEntry->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShowSubtitles));

			SubtitlesEntry->SetShouldApplyChangeImmediately(true);

			HUDCategory->AddChildListData(SubtitlesEntry);
		}
	}

	// Game Category
	{
		UListDataObject_Collection* GameCategory = NewObject<UListDataObject_Collection>();
		GameCategory->SetDataId(FName("Category-Game"));
		GameCategory->SetDataDisplayName(FText::FromString("Game"));

		GameplayCollectionTab->AddChildListData(GameCategory);

		//难度
		{
			UListDataObject_String* GameDifficulty = NewObject<UListDataObject_String>();
			GameDifficulty->SetDataId(FName("Entry-Difficulty"));
			//设置条目名
			GameDifficulty->SetDataDisplayName(FText::FromString("Difficulty"));
			//设置条目描述
			GameDifficulty->SetDescriptionRichText(FText::FromString("Set the game difficulty level."));
			//添加条目选项
			GameDifficulty->AddDynamicString(TEXT("Easy"),FText::FromString("Easy"));
			GameDifficulty->AddDynamicString(TEXT("Normal"),FText::FromString("Normal"));
			GameDifficulty->AddDynamicString(TEXT("Hard"),FText::FromString("Hard"));
			GameDifficulty->AddDynamicString(TEXT("Nightmare"),FText::FromString("Nightmare"));
			//设置条目默认值
			GameDifficulty->SetDefaultValueFromString(TEXT("Normal"));

			GameDifficulty->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetDifficulty));
			GameDifficulty->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetDifficulty));

			GameDifficulty->SetShouldApplyChangeImmediately(true);

			GameCategory->AddChildListData(GameDifficulty);
		}
		//自动存档
		{
			UListDataObject_StringBool* AutoSaveEntry = NewObject<UListDataObject_StringBool>();
			AutoSaveEntry->SetDataId(FName("Entry-AutoSave"));
			AutoSaveEntry->SetDataDisplayName(FText::FromString("Auto Save"));
			AutoSaveEntry->SetDescriptionRichText(FText::FromString("Toggle automatic game progress saving."));

			AutoSaveEntry->SetTrueAsDefaultValue();
			AutoSaveEntry->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAutoSave));
			AutoSaveEntry->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAutoSave));

			AutoSaveEntry->SetShouldApplyChangeImmediately(true);

			GameCategory->AddChildListData(AutoSaveEntry);
		}
		//语言
		{
			UListDataObject_StringCulture* Language = NewObject<UListDataObject_StringCulture>();
			Language->SetDataId(FName("Entry-Language"));
			Language->SetDataDisplayName(FText::FromString("Language"));
			Language->SetDescriptionRichText(FText::FromString("Select the display language. Changes take effect immediately."));
			Language->SetDefaultValueFromString(TEXT("en"));
			Language->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentCulture));
			Language->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentCulture));
			Language->SetShouldApplyChangeImmediately(true);
			GameCategory->AddChildListData(Language);
		}
	}

	RegisteredOptionsCollectionTabs.Add(GameplayCollectionTab);
}

void UOptionsDataRegistry::InitAudioCollectionTab()
{
	UListDataObject_Collection* AudioCollectionTab = NewObject<UListDataObject_Collection>();

	AudioCollectionTab->SetDataId(FName("Tab-Audio"));
	AudioCollectionTab->SetDataDisplayName(FText::FromString("Audio"));

	// Volume Category
	{
		UListDataObject_Collection* VolumeCategory = NewObject<UListDataObject_Collection>();
		VolumeCategory->SetDataId(FName("Category-Volume"));
		VolumeCategory->SetDataDisplayName(FText::FromString("Volume"));

		AudioCollectionTab->AddChildListData(VolumeCategory);
		//Overall Volume
		{
			UListDataObject_Scalar* OverallVolume = NewObject<UListDataObject_Scalar>();
			OverallVolume->SetDataId(FName("OverallVolume"));
			OverallVolume->SetDataDisplayName(FText::FromString("Overall Volume"));
			OverallVolume->SetDescriptionRichText(FText::FromString("Adjust the overall volume level of the game."));
			OverallVolume->SetDisplayValueRange(TRange<float>(0.f,1.f));
			OverallVolume->SetOutValueRange(TRange<float>(0.f, 2.f));
			OverallVolume->SetSliderStepSize(0.01f);
			OverallVolume->SetDefaultValueFromString(LexToString(1.f));
			OverallVolume->SetCommonNumericType(ECommonNumericType::Percentage);
			OverallVolume->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal()); //No 50% OneDecimal 50.5%

			OverallVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallVolume));
			OverallVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallVolume));

			OverallVolume->SetShouldApplyChangeImmediately(true);

			VolumeCategory->AddChildListData(OverallVolume);
		}
		//Music Volume
		{
			UListDataObject_Scalar* MusicVolume = NewObject<UListDataObject_Scalar>();
			MusicVolume->SetDataId(FName("MusicVolume"));
			MusicVolume->SetDataDisplayName(FText::FromString("Music Volume"));
			MusicVolume->SetDescriptionRichText(FText::FromString("Adjust the music volume level."));
			MusicVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			MusicVolume->SetOutValueRange(TRange<float>(0.f, 2.f));
			MusicVolume->SetSliderStepSize(0.01f);
			MusicVolume->SetDefaultValueFromString(LexToString(1.f));
			MusicVolume->SetCommonNumericType(ECommonNumericType::Percentage);
			MusicVolume->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			MusicVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMusicVolume));
			MusicVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMusicVolume));

			MusicVolume->SetShouldApplyChangeImmediately(true);

			VolumeCategory->AddChildListData(MusicVolume);
		}
		//SFX Volume
		{
			UListDataObject_Scalar* SFXVolume = NewObject<UListDataObject_Scalar>();
			SFXVolume->SetDataId(FName("SFXVolume"));
			SFXVolume->SetDataDisplayName(FText::FromString("SFX Volume"));
			SFXVolume->SetDescriptionRichText(FText::FromString("Adjust the sound effects volume level."));
			SFXVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			SFXVolume->SetOutValueRange(TRange<float>(0.f, 2.f));
			SFXVolume->SetSliderStepSize(0.01f);
			SFXVolume->SetDefaultValueFromString(LexToString(1.f));
			SFXVolume->SetCommonNumericType(ECommonNumericType::Percentage);
			SFXVolume->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			SFXVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSFXVolume));
			SFXVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSFXVolume));

			SFXVolume->SetShouldApplyChangeImmediately(true);

			VolumeCategory->AddChildListData(SFXVolume);
		}
		//Voice Volume
		{
			UListDataObject_Scalar* VoiceVolume = NewObject<UListDataObject_Scalar>();
			VoiceVolume->SetDataId(FName("VoiceVolume"));
			VoiceVolume->SetDataDisplayName(FText::FromString("Voice Volume"));
			VoiceVolume->SetDescriptionRichText(FText::FromString("Adjust the voice/dialogue volume level."));
			VoiceVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			VoiceVolume->SetOutValueRange(TRange<float>(0.f, 2.f));
			VoiceVolume->SetSliderStepSize(0.01f);
			VoiceVolume->SetDefaultValueFromString(LexToString(1.f));
			VoiceVolume->SetCommonNumericType(ECommonNumericType::Percentage);
			VoiceVolume->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			VoiceVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVoiceVolume));
			VoiceVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVoiceVolume));

			VoiceVolume->SetShouldApplyChangeImmediately(true);

			VolumeCategory->AddChildListData(VoiceVolume);
		}
		//Ambient Volume
		{
			UListDataObject_Scalar* AmbientVolume = NewObject<UListDataObject_Scalar>();
			AmbientVolume->SetDataId(FName("AmbientVolume"));
			AmbientVolume->SetDataDisplayName(FText::FromString("Ambient Volume"));
			AmbientVolume->SetDescriptionRichText(FText::FromString("Adjust the ambient sounds volume level."));
			AmbientVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			AmbientVolume->SetOutValueRange(TRange<float>(0.f, 2.f));
			AmbientVolume->SetSliderStepSize(0.01f);
			AmbientVolume->SetDefaultValueFromString(LexToString(1.f));
			AmbientVolume->SetCommonNumericType(ECommonNumericType::Percentage);
			AmbientVolume->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			AmbientVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAmbientVolume));
			AmbientVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAmbientVolume));

			AmbientVolume->SetShouldApplyChangeImmediately(true);

			VolumeCategory->AddChildListData(AmbientVolume);
		}
		//UI Volume
		{
			UListDataObject_Scalar* UIVolume = NewObject<UListDataObject_Scalar>();
			UIVolume->SetDataId(FName("UIVolume"));
			UIVolume->SetDataDisplayName(FText::FromString("UI Volume"));
			UIVolume->SetDescriptionRichText(FText::FromString("Adjust the UI sounds volume level."));
			UIVolume->SetDisplayValueRange(TRange<float>(0.f, 1.f));
			UIVolume->SetOutValueRange(TRange<float>(0.f, 2.f));
			UIVolume->SetSliderStepSize(0.01f);
			UIVolume->SetDefaultValueFromString(LexToString(1.f));
			UIVolume->SetCommonNumericType(ECommonNumericType::Percentage);
			UIVolume->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			UIVolume->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetUIVolume));
			UIVolume->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetUIVolume));

			UIVolume->SetShouldApplyChangeImmediately(true);

			VolumeCategory->AddChildListData(UIVolume);
		}
	}
	// Sound Category
	{
		UListDataObject_Collection* SoundCategory = NewObject<UListDataObject_Collection>();
		SoundCategory->SetDataId(FName("Category-Sound"));
		SoundCategory->SetDataDisplayName(FText::FromString("Sound"));

		AudioCollectionTab->AddChildListData(SoundCategory);
		//Allow Background Sound
		{
			UListDataObject_StringBool* BackgroundSound = NewObject<UListDataObject_StringBool>();
			BackgroundSound->SetDataId(FName("Background-Sound"));
			BackgroundSound->SetDataDisplayName(FText::FromString("Background Sound"));
			BackgroundSound->SetDescriptionRichText(FText::FromString("Allow the game to continue playing sounds when it's running in the background."));
			
			BackgroundSound->SetFalseAsDefaultValue();
			BackgroundSound->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAllowBackgroundSound));
			BackgroundSound->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAllowBackgroundSound));
			
			BackgroundSound->SetShouldApplyChangeImmediately(true);
			
			SoundCategory->AddChildListData(BackgroundSound);
		}
		//Subtitles
		{
			UListDataObject_StringBool* Subtitles = NewObject<UListDataObject_StringBool>();
			Subtitles->SetDataId(FName("Sound-Subtitles"));
			Subtitles->SetDataDisplayName(FText::FromString("Subtitles"));
			Subtitles->SetDescriptionRichText(FText::FromString("Toggle subtitle display during dialogues and cutscenes."));
			Subtitles->SetTrueAsDefaultValue();
			Subtitles->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetSubtitlesEnabled));
			Subtitles->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetSubtitlesEnabled));
			Subtitles->SetShouldApplyChangeImmediately(true);
			SoundCategory->AddChildListData(Subtitles);
		}
		//3D Spatial Audio (HRTF)
		{
			UListDataObject_StringBool* HRTFAudio = NewObject<UListDataObject_StringBool>();
			HRTFAudio->SetDataId(FName("Sound-HRTF"));
			HRTFAudio->SetDataDisplayName(FText::FromString("3D Spatial Audio"));
			HRTFAudio->SetDescriptionRichText(FText::FromString("Enable HRTF-based 3D spatial audio for headphones. Provides immersive directional sound."));
			HRTFAudio->SetTrueAsDefaultValue();
			HRTFAudio->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetHRTFEnabled));
			HRTFAudio->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetHRTFEnabled));
			HRTFAudio->SetShouldApplyChangeImmediately(true);
			SoundCategory->AddChildListData(HRTFAudio);
		}
		//Mono Output
		{
			UListDataObject_StringBool* MonoOutput = NewObject<UListDataObject_StringBool>();
			MonoOutput->SetDataId(FName("Sound-Mono"));
			MonoOutput->SetDataDisplayName(FText::FromString("Mono Output"));
			MonoOutput->SetDescriptionRichText(FText::FromString("Downmix all audio to mono. Useful for single-speaker setups or accessibility needs."));
			MonoOutput->SetFalseAsDefaultValue();
			MonoOutput->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetMonoOutput));
			MonoOutput->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetMonoOutput));
			MonoOutput->SetShouldApplyChangeImmediately(true);
			SoundCategory->AddChildListData(MonoOutput);
		}
		//Dynamic Range Compression
		{
			UListDataObject_StringBool* DynamicRange = NewObject<UListDataObject_StringBool>();
			DynamicRange->SetDataId(FName("Sound-DRC"));
			DynamicRange->SetDataDisplayName(FText::FromString("Dynamic Range Compression"));
			DynamicRange->SetDescriptionRichText(FText::FromString("Reduce the volume difference between loud and quiet sounds. Ideal for late-night gaming."));
			DynamicRange->SetFalseAsDefaultValue();
			DynamicRange->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetDynamicRangeCompression));
			DynamicRange->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetDynamicRangeCompression));
			DynamicRange->SetShouldApplyChangeImmediately(true);
			SoundCategory->AddChildListData(DynamicRange);
		}
		//Dialogue Boost
		{
			UListDataObject_StringBool* DialogueBoost = NewObject<UListDataObject_StringBool>();
			DialogueBoost->SetDataId(FName("Sound-DialogueBoost"));
			DialogueBoost->SetDataDisplayName(FText::FromString("Dialogue Boost"));
			DialogueBoost->SetDescriptionRichText(FText::FromString("Enhance voice and dialogue clarity by boosting mid-range frequencies."));
			DialogueBoost->SetTrueAsDefaultValue();
			DialogueBoost->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetDialogueBoost));
			DialogueBoost->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetDialogueBoost));
			DialogueBoost->SetShouldApplyChangeImmediately(true);
			SoundCategory->AddChildListData(DialogueBoost);
		}
	}
	RegisteredOptionsCollectionTabs.Add(AudioCollectionTab);
}

void UOptionsDataRegistry::InitVideoCollectionTab()
{
	UListDataObject_Collection* VideoCollectionTab = NewObject<UListDataObject_Collection>();

	VideoCollectionTab->SetDataId(FName("Tab-Video"));
	VideoCollectionTab->SetDataDisplayName(FText::FromString("Video"));
	
	UListDataObject_StringEnum* CreatedWindowMode = nullptr;

	//Display Category
	{
		UListDataObject_Collection* DisplayCollectionTab = NewObject<UListDataObject_Collection>();
		DisplayCollectionTab->SetDataId(FName("Tab-Display"));
		DisplayCollectionTab->SetDataDisplayName(FText::FromString("Display"));
		
		VideoCollectionTab->AddChildListData(DisplayCollectionTab);
		
		FOptionsDataEditConditionDescriptor PackagedBuildOnlyCondition;
		PackagedBuildOnlyCondition.SetEditConditionFunction(
			[]()->bool
			{
				const bool bIsInEditor = GIsEditor || GIsPlayInEditorWorld;

				return !bIsInEditor;
			}
		);

		PackagedBuildOnlyCondition.SetDisableRichReason(TEXT("\n\n<Warning>This setting can only adjusted in a packaged build</>"));
		
		//Window Mode
		{
			UListDataObject_StringEnum* WindowMode = NewObject<UListDataObject_StringEnum>();
			WindowMode->SetDataId(FName("WindowMode"));
			WindowMode->SetDataDisplayName(FText::FromString("Window Mode"));
			WindowMode->SetDescriptionRichText(FText::FromString("Select the window display mode"));

			WindowMode->AddEnumOption(EWindowMode::Fullscreen,FText::FromString("Fullscreen Mode"));
			WindowMode->AddEnumOption(EWindowMode::WindowedFullscreen,FText::FromString("Borderless Window"));
			WindowMode->AddEnumOption(EWindowMode::Windowed,FText::FromString("Windowed"));

			WindowMode->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);

			WindowMode->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFullscreenMode));
			WindowMode->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFullscreenMode));

			WindowMode->SetShouldApplyChangeImmediately(true);
			
			WindowMode->AddEditConditionDescriptor(PackagedBuildOnlyCondition);
			
			CreatedWindowMode = WindowMode;

			DisplayCollectionTab->AddChildListData(WindowMode);
		}
		//Resolution
		{
			
			UListDataObject_StringResolution* Resolution = NewObject<UListDataObject_StringResolution>();
			Resolution->SetDataId(FName("Resolution"));
			Resolution->SetDataDisplayName(FText::FromString("Resolution"));
			Resolution->SetDescriptionRichText(FText::FromString("Select the screen resolution for the game. Higher resolutions provide better visual quality but may impact performance."));
			
			Resolution->InitResolutionValues();
			
			Resolution->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetScreenResolution));
			Resolution->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetScreenResolution));
			Resolution->SetShouldApplyChangeImmediately(true);
			
			Resolution->AddEditConditionDescriptor(PackagedBuildOnlyCondition);
			
			FOptionsDataEditConditionDescriptor WindowModeEditCondition;
			WindowModeEditCondition.SetEditConditionFunction(
				[CreatedWindowMode]()->bool
				{
					const bool bIsBorderlessWindow = CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::WindowedFullscreen;

					return !bIsBorderlessWindow;
				}
			);
			
			WindowModeEditCondition.SetDisableRichReason(TEXT("\n\n<Warning>This setting is disabled in Borderless Window mode</>"));
			WindowModeEditCondition.SetDisabledForcedStringValue(Resolution->GetMaxAllowedResolution());

			Resolution->AddEditConditionDescriptor(WindowModeEditCondition);
			Resolution->AddEditDependencyData(CreatedWindowMode);
			
			DisplayCollectionTab->AddChildListData(Resolution);
		}
	}
	//Graphics Category
	{
		UListDataObject_Collection* GraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
		GraphicsCategoryCollection->SetDataId(FName("GraphicsCategoryCollection"));
		GraphicsCategoryCollection->SetDataDisplayName(FText::FromString("Graphics"));

		VideoCollectionTab->AddChildListData(GraphicsCategoryCollection);
		//Display Gamma
		{
			UListDataObject_Scalar* DisplayGamma = NewObject<UListDataObject_Scalar>();
			DisplayGamma->SetDataId(FName("DisplayGamma"));
			DisplayGamma->SetDataDisplayName(FText::FromString("DisplayGamma"));
			DisplayGamma->SetDescriptionRichText(FText::FromString("DisplayGammaDescKey"));
			DisplayGamma->SetDisplayValueRange(TRange<float>(0.f,1.f));
			DisplayGamma->SetOutValueRange(TRange<float>(1.7f,2.7f));
			DisplayGamma->SetSliderStepSize(0.01f);
			DisplayGamma->SetCommonNumericType(ECommonNumericType::Percentage);
			DisplayGamma->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			DisplayGamma->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetCurrentDisplayGamma));
			DisplayGamma->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetCurrentDisplayGamma));

			DisplayGamma->SetDefaultValueFromString(LexToString(2.2f));

			GraphicsCategoryCollection->AddChildListData(DisplayGamma);
		}
		UListDataObject_StringInteger* CreatedOverallQuality = nullptr;
		//Overall Quality
		{
			UListDataObject_StringInteger* OverallQuality = NewObject<UListDataObject_StringInteger>();
			OverallQuality->SetDataId(FName("OverallQuality"));
			OverallQuality->SetDataDisplayName(FText::FromString("OverallQuality"));
			OverallQuality->SetDescriptionRichText(FText::FromString("OverallQualityDescKey"));
			OverallQuality->AddIntegerOption(0,FText::FromString("Low"));
			OverallQuality->AddIntegerOption(1,FText::FromString("Medium"));
			OverallQuality->AddIntegerOption(2,FText::FromString("High"));
			OverallQuality->AddIntegerOption(3,FText::FromString("Epic"));
			OverallQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			OverallQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetOverallScalabilityLevel));
			OverallQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetOverallScalabilityLevel));
			
			OverallQuality->SetShouldApplyChangeImmediately(true);
			
			GraphicsCategoryCollection->AddChildListData(OverallQuality);
			
			CreatedOverallQuality = OverallQuality;
		}
		//Resolution Scale
		{
			UListDataObject_Scalar* ResolutionScale = NewObject<UListDataObject_Scalar>();
			ResolutionScale->SetDataId(FName("ResolutionScale"));
			ResolutionScale->SetDataDisplayName(FText::FromString("Resolution Scale"));
			ResolutionScale->SetDescriptionRichText(FText::FromString("Adjust the rendering resolution as a percentage of the native resolution. Lower values improve performance."));
			ResolutionScale->SetDisplayValueRange(TRange<float>(0.f,1.f));
			ResolutionScale->SetOutValueRange(TRange<float>(0.5f,1.f));
			ResolutionScale->SetSliderStepSize(0.01f);
			ResolutionScale->SetDefaultValueFromString(LexToString(1.f));
			ResolutionScale->SetCommonNumericType(ECommonNumericType::Percentage);
			ResolutionScale->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());

			ResolutionScale->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetResolutionScaleNormalized));
			ResolutionScale->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetResolutionScaleNormalized));

			ResolutionScale->SetShouldApplyChangeImmediately(true);
			
			ResolutionScale->AddEditDependencyData(CreatedOverallQuality);

			GraphicsCategoryCollection->AddChildListData(ResolutionScale);
		}
		//Global Illumination Quality
		{
			UListDataObject_StringInteger* GlobalIlluminationQuality = NewObject<UListDataObject_StringInteger>();
			GlobalIlluminationQuality->SetDataId(FName("GlobalIlluminationQuality"));
			GlobalIlluminationQuality->SetDataDisplayName(FText::FromString("Global Illumination Quality"));
			GlobalIlluminationQuality->SetDescriptionRichText(FText::FromString("Adjust the quality of global illumination lighting effects."));
			GlobalIlluminationQuality->AddIntegerOption(0,FText::FromString("Low"));
			GlobalIlluminationQuality->AddIntegerOption(1,FText::FromString("Medium"));
			GlobalIlluminationQuality->AddIntegerOption(2,FText::FromString("High"));
			GlobalIlluminationQuality->AddIntegerOption(3,FText::FromString("Epic"));
			GlobalIlluminationQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			GlobalIlluminationQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetGlobalIlluminationQuality));
			GlobalIlluminationQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetGlobalIlluminationQuality));

			GlobalIlluminationQuality->SetShouldApplyChangeImmediately(true);
			
			GlobalIlluminationQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(GlobalIlluminationQuality);

			GraphicsCategoryCollection->AddChildListData(GlobalIlluminationQuality);
		}
		//Shadow Quality
		{
			UListDataObject_StringInteger* ShadowQuality = NewObject<UListDataObject_StringInteger>();
			ShadowQuality->SetDataId(FName("ShadowQuality"));
			ShadowQuality->SetDataDisplayName(FText::FromString("Shadow Quality"));
			ShadowQuality->SetDescriptionRichText(FText::FromString("Adjust the quality and resolution of shadow rendering."));
			ShadowQuality->AddIntegerOption(0,FText::FromString("Low"));
			ShadowQuality->AddIntegerOption(1,FText::FromString("Medium"));
			ShadowQuality->AddIntegerOption(2,FText::FromString("High"));
			ShadowQuality->AddIntegerOption(3,FText::FromString("Epic"));
			ShadowQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			ShadowQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetShadowQuality));
			ShadowQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetShadowQuality));

			ShadowQuality->SetShouldApplyChangeImmediately(true);
			
			ShadowQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ShadowQuality);

			GraphicsCategoryCollection->AddChildListData(ShadowQuality);
		}
		//AntiAliasing Quality
		{
			UListDataObject_StringInteger* AntiAliasingQuality = NewObject<UListDataObject_StringInteger>();
			AntiAliasingQuality->SetDataId(FName("AntiAliasingQuality"));
			AntiAliasingQuality->SetDataDisplayName(FText::FromString("AntiAliasing Quality"));
			AntiAliasingQuality->SetDescriptionRichText(FText::FromString("Adjust the quality of anti-aliasing for smoother edges."));
			AntiAliasingQuality->AddIntegerOption(0,FText::FromString("Low"));
			AntiAliasingQuality->AddIntegerOption(1,FText::FromString("Medium"));
			AntiAliasingQuality->AddIntegerOption(2,FText::FromString("High"));
			AntiAliasingQuality->AddIntegerOption(3,FText::FromString("Epic"));
			AntiAliasingQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			AntiAliasingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetAntiAliasingQuality));
			AntiAliasingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetAntiAliasingQuality));

			AntiAliasingQuality->SetShouldApplyChangeImmediately(true);
			
			AntiAliasingQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(AntiAliasingQuality);

			GraphicsCategoryCollection->AddChildListData(AntiAliasingQuality);
		}
		//View Distance Quality
		{
			UListDataObject_StringInteger* ViewDistanceQuality = NewObject<UListDataObject_StringInteger>();
			ViewDistanceQuality->SetDataId(FName("ViewDistanceQuality"));
			ViewDistanceQuality->SetDataDisplayName(FText::FromString("View Distance Quality"));
			ViewDistanceQuality->SetDescriptionRichText(FText::FromString("Adjust the view distance for rendering distant objects."));
			ViewDistanceQuality->AddIntegerOption(0,FText::FromString("Low"));
			ViewDistanceQuality->AddIntegerOption(1,FText::FromString("Medium"));
			ViewDistanceQuality->AddIntegerOption(2,FText::FromString("High"));
			ViewDistanceQuality->AddIntegerOption(3,FText::FromString("Epic"));
			ViewDistanceQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			ViewDistanceQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetViewDistanceQuality));
			ViewDistanceQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetViewDistanceQuality));

			ViewDistanceQuality->SetShouldApplyChangeImmediately(true);
			
			ViewDistanceQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ViewDistanceQuality);

			GraphicsCategoryCollection->AddChildListData(ViewDistanceQuality);
		}
		//Texture Quality
		{
			UListDataObject_StringInteger* TextureQuality = NewObject<UListDataObject_StringInteger>();
			TextureQuality->SetDataId(FName("TextureQuality"));
			TextureQuality->SetDataDisplayName(FText::FromString("Texture Quality"));
			TextureQuality->SetDescriptionRichText(FText::FromString("Adjust the texture resolution and streaming quality."));
			TextureQuality->AddIntegerOption(0,FText::FromString("Low"));
			TextureQuality->AddIntegerOption(1,FText::FromString("Medium"));
			TextureQuality->AddIntegerOption(2,FText::FromString("High"));
			TextureQuality->AddIntegerOption(3,FText::FromString("Epic"));
			TextureQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			TextureQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetTextureQuality));
			TextureQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetTextureQuality));

			TextureQuality->SetShouldApplyChangeImmediately(true);
			
			TextureQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(TextureQuality);

			GraphicsCategoryCollection->AddChildListData(TextureQuality);
		}
		//Visual Effects Quality
		{
			UListDataObject_StringInteger* VisualEffectsQuality = NewObject<UListDataObject_StringInteger>();
			VisualEffectsQuality->SetDataId(FName("VisualEffectsQuality"));
			VisualEffectsQuality->SetDataDisplayName(FText::FromString("Visual Effects Quality"));
			VisualEffectsQuality->SetDescriptionRichText(FText::FromString("Adjust the quality of visual effects such as particles and explosions."));
			VisualEffectsQuality->AddIntegerOption(0,FText::FromString("Low"));
			VisualEffectsQuality->AddIntegerOption(1,FText::FromString("Medium"));
			VisualEffectsQuality->AddIntegerOption(2,FText::FromString("High"));
			VisualEffectsQuality->AddIntegerOption(3,FText::FromString("Epic"));
			VisualEffectsQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			VisualEffectsQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetVisualEffectQuality));
			VisualEffectsQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVisualEffectQuality));

			VisualEffectsQuality->SetShouldApplyChangeImmediately(true);
			
			VisualEffectsQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(VisualEffectsQuality);

			GraphicsCategoryCollection->AddChildListData(VisualEffectsQuality);
		}
		//Reflection Quality
		{
			UListDataObject_StringInteger* ReflectionQuality = NewObject<UListDataObject_StringInteger>();
			ReflectionQuality->SetDataId(FName("ReflectionQuality"));
			ReflectionQuality->SetDataDisplayName(FText::FromString("Reflection Quality"));
			ReflectionQuality->SetDescriptionRichText(FText::FromString("Adjust the quality of screen-space and planar reflections."));
			ReflectionQuality->AddIntegerOption(0,FText::FromString("Low"));
			ReflectionQuality->AddIntegerOption(1,FText::FromString("Medium"));
			ReflectionQuality->AddIntegerOption(2,FText::FromString("High"));
			ReflectionQuality->AddIntegerOption(3,FText::FromString("Epic"));
			ReflectionQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			ReflectionQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetReflectionQuality));
			ReflectionQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetReflectionQuality));

			ReflectionQuality->SetShouldApplyChangeImmediately(true);
			
			ReflectionQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(ReflectionQuality);

			GraphicsCategoryCollection->AddChildListData(ReflectionQuality);
		}
		//Post Processing Quality
		{
			UListDataObject_StringInteger* PostProcessingQuality = NewObject<UListDataObject_StringInteger>();
			PostProcessingQuality->SetDataId(FName("PostProcessingQuality"));
			PostProcessingQuality->SetDataDisplayName(FText::FromString("Post Processing Quality"));
			PostProcessingQuality->SetDescriptionRichText(FText::FromString("Adjust the quality of post-processing effects such as bloom and depth of field."));
			PostProcessingQuality->AddIntegerOption(0,FText::FromString("Low"));
			PostProcessingQuality->AddIntegerOption(1,FText::FromString("Medium"));
			PostProcessingQuality->AddIntegerOption(2,FText::FromString("High"));
			PostProcessingQuality->AddIntegerOption(3,FText::FromString("Epic"));
			PostProcessingQuality->AddIntegerOption(4,FText::FromString("Cinematic"));

			PostProcessingQuality->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetPostProcessingQuality));
			PostProcessingQuality->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetPostProcessingQuality));

			PostProcessingQuality->SetShouldApplyChangeImmediately(true);
			
			PostProcessingQuality->AddEditDependencyData(CreatedOverallQuality);
			CreatedOverallQuality->AddEditDependencyData(PostProcessingQuality);

			GraphicsCategoryCollection->AddChildListData(PostProcessingQuality);
		}
	}
	//Advanced Graphics Category
	{
		UListDataObject_Collection* AdvancedGraphicsCategoryCollection = NewObject<UListDataObject_Collection>();
		AdvancedGraphicsCategoryCollection->SetDataId(FName("AdvancedGraphicsCategoryCollection"));
		AdvancedGraphicsCategoryCollection->SetDataDisplayName(FText::FromString("Advanced Graphics"));

		VideoCollectionTab->AddChildListData(AdvancedGraphicsCategoryCollection);
		//Vertical Sync
		{
			UListDataObject_StringBool* VerticalSync = NewObject<UListDataObject_StringBool>();
			VerticalSync->SetDataId(FName("Entry-VSync"));
			VerticalSync->SetDataDisplayName(FText::FromString("Vertical Sync"));
			VerticalSync->SetDescriptionRichText(FText::FromString("Synchronize the frame rate with the display refresh rate to prevent screen tearing."));

			VerticalSync->SetTrueAsDefaultValue();
			VerticalSync->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(IsVSyncEnabled));
			VerticalSync->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetVSyncEnabled));

			VerticalSync->SetShouldApplyChangeImmediately(true);
			
			FOptionsDataEditConditionDescriptor FullscreenOnlyCondition;
			FullscreenOnlyCondition.SetEditConditionFunction(
				[CreatedWindowMode]()->bool
				{
					return CreatedWindowMode->GetCurrentValueAsEnum<EWindowMode::Type>() == EWindowMode::Fullscreen;
				});
			
			FullscreenOnlyCondition.SetDisableRichReason("\n\n<Warning>This setting is only available in Fullscreen mode</>");
			FullscreenOnlyCondition.SetDisabledForcedStringValue("false");

			VerticalSync->AddEditConditionDescriptor(FullscreenOnlyCondition);

			AdvancedGraphicsCategoryCollection->AddChildListData(VerticalSync);
		}
		//Frame Rate Limit
		{
			UListDataObject_String* FrameRateLimit = NewObject<UListDataObject_String>();
			FrameRateLimit->SetDataId(FName("Entry-FrameRateLimit"));
			FrameRateLimit->SetDataDisplayName(FText::FromString("Frame Rate Limit"));
			FrameRateLimit->SetDescriptionRichText(FText::FromString("Limit the maximum frames per second. Lower values reduce system load but may feel less responsive."));

			FrameRateLimit->AddDynamicString(LexToString(0.0f), FText::FromString("Unlimited"));
			FrameRateLimit->AddDynamicString(LexToString(30.0f), FText::FromString("30 FPS"));
			FrameRateLimit->AddDynamicString(LexToString(60.0f), FText::FromString("60 FPS"));
			FrameRateLimit->AddDynamicString(LexToString(120.0f), FText::FromString("120 FPS"));
			FrameRateLimit->AddDynamicString(LexToString(144.0f), FText::FromString("144 FPS"));
			FrameRateLimit->AddDynamicString(LexToString(240.0f), FText::FromString("240 FPS"));

			FrameRateLimit->SetDefaultValueFromString(LexToString(0.0f));

			FrameRateLimit->SetDataDynamicGetter(MAKE_OPTIONS_DATA_CONTROL(GetFrameRateLimit));
			FrameRateLimit->SetDataDynamicSetter(MAKE_OPTIONS_DATA_CONTROL(SetFrameRateLimit));

			FrameRateLimit->SetShouldApplyChangeImmediately(true);

			AdvancedGraphicsCategoryCollection->AddChildListData(FrameRateLimit);
		}
	}
	
	RegisteredOptionsCollectionTabs.Add(VideoCollectionTab);
}

void UOptionsDataRegistry::InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer)
{
	UListDataObject_Collection* ControlCollectionTab = NewObject<UListDataObject_Collection>();

	ControlCollectionTab->SetDataId(FName("Tab-Control"));
	ControlCollectionTab->SetDataDisplayName(FText::FromString("Control"));

	if (!InOwningLocalPlayer)
	{
		RegisteredOptionsCollectionTabs.Add(ControlCollectionTab);
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = InOwningLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		RegisteredOptionsCollectionTabs.Add(ControlCollectionTab);
		return;
	}

	UEnhancedInputUserSettings* UserSettings = Subsystem->GetUserSettings();
	if (!UserSettings)
	{
		RegisteredOptionsCollectionTabs.Add(ControlCollectionTab);
		return;
	}

	UEnhancedPlayerMappableKeyProfile* ActiveProfile = UserSettings->GetActiveKeyProfile();
	if (!ActiveProfile)
	{
		RegisteredOptionsCollectionTabs.Add(ControlCollectionTab);
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
			if (!Action)
			{
				//Debug::Print(FString::Printf(TEXT("  SKIP: Mapping='%s' has no associated InputAction"),
					//*PlayerMapping.GetMappingName().ToString()));
				continue;
			}

			GroupedByAction.FindOrAdd(Action).Add(&PlayerMapping);

			//Debug::Print(FString::Printf(TEXT("  MappingName='%s' | Action='%s' | CurrentKey='%s' | DefaultKey='%s' | Category='%s' | Display='%s' | Slot=%d | Device=%d"),
				/**PlayerMapping.GetMappingName().ToString(),
				*Action->GetName(),
				*PlayerMapping.GetCurrentKey().ToString(),
				*PlayerMapping.GetDefaultKey().ToString(),
				*PlayerMapping.GetDisplayCategory().ToString(),
				*PlayerMapping.GetDisplayName().ToString(),
				static_cast<int32>(PlayerMapping.GetSlot()),
				static_cast<int32>(PlayerMapping.GetPrimaryDeviceType())));*/
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
			TargetCategory = NewObject<UListDataObject_Collection>();
			TargetCategory->SetDataId(FName(*CategoryKey));
			TargetCategory->SetDataDisplayName(CategoryText.IsEmpty() ? FText::FromString("General") : CategoryText);
			ControlCollectionTab->AddChildListData(TargetCategory);
		}

		UUListDataObject_KeyBinding* KeyBindingEntry = NewObject<UUListDataObject_KeyBinding>();
		KeyBindingEntry->SetOwningLocalPlayer(InOwningLocalPlayer);
		//KeyBindingEntry->SetShouldApplyChangeImmediately(true);
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
	RegisteredOptionsCollectionTabs.Add(ControlCollectionTab);
}
