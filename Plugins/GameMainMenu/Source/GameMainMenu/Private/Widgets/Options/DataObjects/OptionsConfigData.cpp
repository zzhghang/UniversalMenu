// HangHang All Rights Reserved


#include "Widgets/Options/DataObjects/OptionsConfigData.h"
#include "Widgets/Options/OptionsDataInteractionHelper.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_Scalar.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringCulture.h"
#include "Widgets/Options/DataObjects/ListDataObject_StringResolution.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "LocalizationKeys.h"

namespace
{
	FORCEINLINE UFrontendGameUserSettings* GetGS()
	{
		return UFrontendGameUserSettings::Get();
	}
}

// ============================================================================
// Static dispatch: EGameUserSettingFunction → getter/setter lambdas
// ============================================================================

static FOptionsDataInteractionHelper::FGetter MakeGetter(EGameUserSettingFunction Func)
{
	switch (Func)
	{
	// ---- Gameplay ----
	case EGameUserSettingFunction::GetDifficulty:
		return []{ auto* S = GetGS(); return S ? S->GetDifficulty() : FString(); };
	case EGameUserSettingFunction::GetShowHealthBar:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetShowHealthBar()) : FString(); };
	case EGameUserSettingFunction::GetShowDamageNumbers:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetShowDamageNumbers()) : FString(); };
	case EGameUserSettingFunction::GetHighlightInteraction:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetHighlightInteraction()) : FString(); };
	case EGameUserSettingFunction::GetShowSubtitles:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetShowSubtitles()) : FString(); };
	case EGameUserSettingFunction::GetAutoSave:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetAutoSave()) : FString(); };
	case EGameUserSettingFunction::GetCurrentCulture:
		return []{ auto* S = GetGS(); return S ? S->GetCurrentCulture() : FString(); };

	// ---- Audio ----
	case EGameUserSettingFunction::GetOverallVolume:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetOverallVolume()) : FString(); };
	case EGameUserSettingFunction::GetMusicVolume:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetMusicVolume()) : FString(); };
	case EGameUserSettingFunction::GetSFXVolume:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetSFXVolume()) : FString(); };
	case EGameUserSettingFunction::GetVoiceVolume:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetVoiceVolume()) : FString(); };
	case EGameUserSettingFunction::GetAmbientVolume:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetAmbientVolume()) : FString(); };
	case EGameUserSettingFunction::GetUIVolume:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetUIVolume()) : FString(); };
	case EGameUserSettingFunction::GetAllowBackgroundSound:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetAllowBackgroundSound()) : FString(); };
	case EGameUserSettingFunction::GetSubtitlesEnabled:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetSubtitlesEnabled()) : FString(); };
	case EGameUserSettingFunction::GetHRTFEnabled:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetHRTFEnabled()) : FString(); };
	case EGameUserSettingFunction::GetMonoOutput:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetMonoOutput()) : FString(); };
	case EGameUserSettingFunction::GetDynamicRangeCompression:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetDynamicRangeCompression()) : FString(); };
	case EGameUserSettingFunction::GetDialogueBoost:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetDialogueBoost()) : FString(); };

	// ---- Video ----
	case EGameUserSettingFunction::GetFullscreenMode:
		return []{
			auto* S = GetGS();
			if (!S) return FString();
			return StaticEnum<EWindowMode::Type>()->GetNameStringByValue(static_cast<int64>(S->GetFullscreenMode()));
		};
	case EGameUserSettingFunction::GetScreenResolution:
		return []{
			auto* S = GetGS();
			if (!S) return FString();
			const FIntPoint Res = S->GetScreenResolution();
			return FString::Printf(TEXT("(X=%d,Y=%d)"), Res.X, Res.Y);
		};
	case EGameUserSettingFunction::GetCurrentDisplayGamma:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetCurrentDisplayGamma()) : FString(); };
	case EGameUserSettingFunction::GetOverallScalabilityLevel:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetOverallScalabilityLevel()) : FString(); };
	case EGameUserSettingFunction::GetResolutionScaleNormalized:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetResolutionScaleNormalized()) : FString(); };
	case EGameUserSettingFunction::GetGlobalIlluminationQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetGlobalIlluminationQuality()) : FString(); };
	case EGameUserSettingFunction::GetShadowQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetShadowQuality()) : FString(); };
	case EGameUserSettingFunction::GetAntiAliasingQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetAntiAliasingQuality()) : FString(); };
	case EGameUserSettingFunction::GetViewDistanceQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetViewDistanceQuality()) : FString(); };
	case EGameUserSettingFunction::GetTextureQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetTextureQuality()) : FString(); };
	case EGameUserSettingFunction::GetVisualEffectQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetVisualEffectQuality()) : FString(); };
	case EGameUserSettingFunction::GetReflectionQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetReflectionQuality()) : FString(); };
	case EGameUserSettingFunction::GetPostProcessingQuality:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetPostProcessingQuality()) : FString(); };
	case EGameUserSettingFunction::IsVSyncEnabled:
		return []{ auto* S = GetGS(); return S ? LexToString(S->IsVSyncEnabled()) : FString(); };
	case EGameUserSettingFunction::GetFrameRateLimit:
		return []{ auto* S = GetGS(); return S ? LexToString(S->GetFrameRateLimit()) : FString(); };

	default:
		return nullptr;
	}
}

static FOptionsDataInteractionHelper::FSetter MakeSetter(EGameUserSettingFunction Func)
{
	switch (Func)
	{
	// ---- Gameplay ----
	case EGameUserSettingFunction::SetDifficulty:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetDifficulty(V); };
	case EGameUserSettingFunction::SetShowHealthBar:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetShowHealthBar(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetShowDamageNumbers:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetShowDamageNumbers(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetHighlightInteraction:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetHighlightInteraction(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetShowSubtitles:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetShowSubtitles(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetAutoSave:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetAutoSave(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetCurrentCulture:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetCurrentCulture(V); };

	// ---- Audio ----
	case EGameUserSettingFunction::SetOverallVolume:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetOverallVolume(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetMusicVolume:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetMusicVolume(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetSFXVolume:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetSFXVolume(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetVoiceVolume:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetVoiceVolume(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetAmbientVolume:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetAmbientVolume(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetUIVolume:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetUIVolume(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetAllowBackgroundSound:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetAllowBackgroundSound(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetSubtitlesEnabled:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetSubtitlesEnabled(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetHRTFEnabled:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetHRTFEnabled(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetMonoOutput:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetMonoOutput(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetDynamicRangeCompression:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetDynamicRangeCompression(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetDialogueBoost:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetDialogueBoost(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };

	// ---- Video ----
	case EGameUserSettingFunction::SetFullscreenMode:
		return [](const FString& V){
			if (auto* S = GetGS())
			{
				const int64 ModeVal = StaticEnum<EWindowMode::Type>()->GetValueByNameString(V);
				if (ModeVal != INDEX_NONE) S->SetFullscreenMode(static_cast<EWindowMode::Type>(ModeVal));
			}
		};
	case EGameUserSettingFunction::SetScreenResolution:
		return [](const FString& V){
			if (auto* S = GetGS())
			{
				FIntPoint Res;
				FParse::Value(*V, TEXT("X="), Res.X);
				FParse::Value(*V, TEXT("Y="), Res.Y);
				if (Res.X > 0 && Res.Y > 0) S->SetScreenResolution(Res);
			}
		};
	case EGameUserSettingFunction::SetCurrentDisplayGamma:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetCurrentDisplayGamma(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetOverallScalabilityLevel:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetOverallScalabilityLevel(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetResolutionScaleNormalized:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetResolutionScaleNormalized(FCString::Atof(*V)); };
	case EGameUserSettingFunction::SetGlobalIlluminationQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetGlobalIlluminationQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetShadowQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetShadowQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetAntiAliasingQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetAntiAliasingQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetViewDistanceQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetViewDistanceQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetTextureQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetTextureQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetVisualEffectQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetVisualEffectQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetReflectionQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetReflectionQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetPostProcessingQuality:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetPostProcessingQuality(FCString::Atoi(*V)); };
	case EGameUserSettingFunction::SetVSyncEnabled:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetVSyncEnabled(V.Equals(TEXT("true"), ESearchCase::IgnoreCase)); };
	case EGameUserSettingFunction::SetFrameRateLimit:
		return [](const FString& V){ if (auto* S = GetGS()) S->SetFrameRateLimit(FCString::Atof(*V)); };

	default:
		return nullptr;
	}
}

// ============================================================================
// Public API
// ============================================================================

TSharedPtr<FOptionsDataInteractionHelper> CreateHelperFromEnum(EGameUserSettingFunction Func)
{
	if (Func == EGameUserSettingFunction::None) return nullptr;

	auto Getter = MakeGetter(Func);
	auto Setter = MakeSetter(Func);
	return MakeShared<FOptionsDataInteractionHelper>(MoveTemp(Getter), MoveTemp(Setter));
}

/**
 * Factory that creates a UListDataObject_Base from DataAsset configuration.
 *
 * Dispatches on EOptionValueType to instantiate the correct subclass
 * (StringBool, Scalar, String, Enum, QualityPreset, Culture, Resolution)
 * and wires up getter/setter helpers from EGameUserSettingFunction enums.
 */
UListDataObject_Base* CreateEntryFromData(UOptionEntryData* Data)
{
	if (!Data) return nullptr;

	const TSharedPtr<FOptionsDataInteractionHelper> Getter = CreateHelperFromEnum(Data->GetterFunc);
	const TSharedPtr<FOptionsDataInteractionHelper> Setter = CreateHelperFromEnum(Data->SetterFunc);

	switch (Data->ValueType)
	{
	case EOptionValueType::Bool:
		{
			UListDataObject_StringBool* Entry = NewObject<UListDataObject_StringBool>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			if (Data->DefaultValue.Equals(TEXT("true"), ESearchCase::IgnoreCase))
				Entry->SetTrueAsDefaultValue();
			else
				Entry->SetFalseAsDefaultValue();
			return Entry;
		}

	case EOptionValueType::Scalar:
		{
			UListDataObject_Scalar* Entry = NewObject<UListDataObject_Scalar>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->SetDisplayValueRange(TRange<float>(Data->ScalarMin, Data->ScalarMax));
			Entry->SetOutValueRange(TRange<float>(Data->ScalarOutMin, Data->ScalarOutMax));
			Entry->SetSliderStepSize(Data->StepSize);
			Entry->SetDefaultValueFromString(Data->DefaultValue);
			Entry->SetCommonNumericType(Data->NumericType);
			Entry->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::String:
	case EOptionValueType::Enum:
		{
			// Normalize numeric keys so "0.0" matches LexToString(0.0f) = "0.000000"
			auto IsNumericString = [](const FString& Str) -> bool
			{
				if (Str.IsEmpty()) return false;
				for (const TCHAR Ch : Str)
				{
					if (!FChar::IsDigit(Ch) && Ch != '.' && Ch != '-')
						return false;
				}
				return true;
			};
			auto NormalizeKey = [&](const FString& Key) -> FString
			{
				return IsNumericString(Key) ? LexToString(FCString::Atof(*Key)) : Key;
			};

			FString NormalizedDefault = NormalizeKey(Data->DefaultValue);

			UListDataObject_String* Entry = NewObject<UListDataObject_String>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			for (const auto& Pair : Data->AvailableOptions)
			{
				Entry->AddDynamicString(NormalizeKey(Pair.Key), Pair.Value);
			}
			Entry->SetDefaultValueFromString(NormalizedDefault);
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::QualityPreset:
		{
			UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->AddIntegerOption(0, TXT_VAL_QUALITY_LOW());
			Entry->AddIntegerOption(1, TXT_VAL_QUALITY_MEDIUM());
			Entry->AddIntegerOption(2, TXT_VAL_QUALITY_HIGH());
			Entry->AddIntegerOption(3, TXT_VAL_QUALITY_EPIC());
			Entry->AddIntegerOption(4, TXT_VAL_QUALITY_CINEMATIC());
			if (!Data->DefaultValue.IsEmpty()) { Entry->SetDefaultValueFromString(Data->DefaultValue); }
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::Culture:
		{
			UListDataObject_StringCulture* Entry = NewObject<UListDataObject_StringCulture>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->SetDefaultValueFromString(Data->DefaultValue);
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	case EOptionValueType::Resolution:
		{
			UListDataObject_StringResolution* Entry = NewObject<UListDataObject_StringResolution>();
			Entry->SetDataId(Data->DataId);
			Entry->SetDataDisplayName(Data->DisplayName);
			Entry->SetDescriptionRichText(Data->Description);
			Entry->InitResolutionValues();
			if (!Data->DefaultValue.IsEmpty()) { Entry->SetDefaultValueFromString(Data->DefaultValue); }
			if (Getter) Entry->SetDataDynamicGetter(Getter);
			if (Setter) Entry->SetDataDynamicSetter(Setter);
			Entry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);
			return Entry;
		}

	default:
		return nullptr;
	}
}

UListDataObject_Collection* CreateCollectionFromData(const FName& Id, const FText& DisplayName)
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}
