// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonNumericTextBlock.h"
#include "Widgets/MenuConfig/MenuConfigTypes.h"
#include "MenuEntryConfig.generated.h"

// ============================================================================
// UMenuEntryConfig — unified entry DataAsset replacing UOptionEntryData + UMultiplayerEntryConfig
// ============================================================================

UCLASS(BlueprintType, meta = (PrioritizeCategories = "Entry Value Binding Conditions Display"))
class GAMEMAINMENU_API UMenuEntryConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// ---- Identity and description ----
	/** Entry identifier. Must be unique within its tab; dependencies reference this ID. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entry", meta = (DisplayPriority = "0"))
	FName DataId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entry", meta = (DisplayPriority = "1"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entry", meta = (DisplayPriority = "2", MultiLine = "true"))
	FText Description;

	/** Selects the entry control and the type-specific fields shown below. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "0"))
	EMenuEntryValueType ValueType = EMenuEntryValueType::Bool;

	// ---- Settings binding ----
	/** Options settings getter/setter key. Leave empty for unbound UI data, such as multiplayer form entries. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binding")
	FName BindingKey;

	/** Requests immediate settings application for supported value entries. Options overrides this to false and commits on Back. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Binding")
	bool bApplyImmediately = false;

	// ---- Availability and dependency refresh ----
	/** Options only: disables editing in the editor and PIE, with a packaged-build-only reason. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions", meta = (DisplayPriority = "0"))
	bool bPackagedBuildOnly = false;

	/** IDs in the same tab to observe for changes. Multiplayer also requires all referenced Bool entries to be true; Options uses separate edit conditions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions", meta = (DisplayPriority = "1"))
	TArray<FName> EditDependencyIds;

	// ---- Bool fields ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "1", EditCondition = "ValueType == EMenuEntryValueType::Bool", EditConditionHides))
	bool bDefaultBool = false;

	// ---- Scalar fields ----
	/** Default value in the output range, stored as a numeric string. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "1", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	FString DefaultValue;

	/** Minimum value displayed by the slider. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "2", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarMin = 0.f;

	/** Maximum value displayed by the slider. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "3", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarMax = 1.f;

	/** Minimum underlying value sent to the settings binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "4", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarOutMin = 0.f;

	/** Maximum underlying value sent to the settings binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "5", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarOutMax = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "6", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float StepSize = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "7", EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	ECommonNumericType NumericType = ECommonNumericType::Percentage;

	// ---- String fields ----
	/** Empty means no explicit default. FullscreenMode uses enum names; Culture uses a culture code; Resolution uses a resolution string. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "1", EditCondition = "ValueType == EMenuEntryValueType::String || ValueType == EMenuEntryValueType::Culture || ValueType == EMenuEntryValueType::Resolution", EditConditionHides))
	FString DefaultStringValue;

	/** FullscreenMode keys are enum names; an empty map uses the built-in window modes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "2", EditCondition = "ValueType == EMenuEntryValueType::String", EditConditionHides))
	TMap<FString, FText> AvailableOptions;

	// ---- Integer fields ----
	/** Always used as the default, including 0. FullscreenMode: 0 = fullscreen, 1 = borderless, 2 = windowed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "1", EditCondition = "ValueType == EMenuEntryValueType::Integer", EditConditionHides))
	int32 DefaultIntValue = 0;

	/** Configured options and localized labels. For FullscreenMode, an empty map uses the built-in window modes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "2", EditCondition = "ValueType == EMenuEntryValueType::Integer", EditConditionHides))
	TMap<int32, FText> AvailableIntegerOptions;

	// ---- Editable fields ----
	/** Maximum input length. Zero means no explicit limit. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "2", EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	int32 MaxCharCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "3", EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	bool bIsPassword = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "1", EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	FText HintText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value", meta = (DisplayPriority = "4", EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	EEditableCharFilter CharFilter = EEditableCharFilter::All;

	/** Rich-text reason for disabled editing. Also used by Multiplayer's Bool dependency condition. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions", meta = (DisplayPriority = "2", MultiLine = "true"))
	FText DisableRichReason;

	// ---- Additional display metadata ----
	/** Additional rich text displayed in the option detail panel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Display", meta = (MultiLine = "true"))
	FText DynamicDetails;
};
