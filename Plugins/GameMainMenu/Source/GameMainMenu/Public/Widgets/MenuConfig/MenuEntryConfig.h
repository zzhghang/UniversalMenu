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

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMenuEntryConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	// ---- Common (all ValueTypes) ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DataId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMenuEntryValueType ValueType = EMenuEntryValueType::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName BindingKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bApplyImmediately = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bPackagedBuildOnly = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> EditDependencyIds;

	// ---- Bool fields ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Bool", EditConditionHides))
	bool bDefaultBool = false;

	// ---- Scalar fields ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	FString DefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarMin = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarMax = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarOutMin = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float ScalarOutMax = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	float StepSize = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))
	ECommonNumericType NumericType = ECommonNumericType::Percentage;

	// ---- String fields ----
	/** Empty means no explicit default. FullscreenMode uses Fullscreen, WindowedFullscreen or Windowed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::String", EditConditionHides))
	FString DefaultStringValue;

	/** FullscreenMode keys are enum names; an empty map uses the built-in window modes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::String", EditConditionHides))
	TMap<FString, FText> AvailableOptions;

	// ---- Integer fields ----
	/** Always used as the default, including 0. FullscreenMode: 0 = fullscreen, 1 = borderless, 2 = windowed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Integer", EditConditionHides))
	int32 DefaultIntValue = 0;

	/** Configured options and localized labels. For FullscreenMode, an empty map uses the built-in window modes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Integer", EditConditionHides))
	TMap<int32, FText> AvailableIntegerOptions;

	// ---- Editable fields ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	int32 MaxCharCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	bool bIsPassword = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	FText HintText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Editable", EditConditionHides))
	EEditableCharFilter CharFilter = EEditableCharFilter::All;

	// ---- Display metadata (all ValueTypes) ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisableRichReason;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DynamicDetails;
};
