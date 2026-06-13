// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonNumericTextBlock.h"
#include "Widgets/Options/DataObjects/OptionsDataTypes.h"
#include "OptionsConfigData.generated.h"

// ============================================================================
// UOptionEntryData — single settings option
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UOptionEntryData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DataId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EOptionValueType ValueType = EOptionValueType::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGameUserSettingFunction GetterFunc = EGameUserSettingFunction::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EGameUserSettingFunction SetterFunc = EGameUserSettingFunction::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString DefaultValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bApplyImmediately = true;

	// ---- Scalar fields ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
	float ScalarMin = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
	float ScalarMax = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
	float ScalarOutMin = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
	float ScalarOutMax = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
	float StepSize = 0.01f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
	ECommonNumericType NumericType = ECommonNumericType::Percentage;

	// ---- String / Enum fields ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EOptionValueType::String || ValueType == EOptionValueType::Enum", EditConditionHides))
	TMap<FString, FText> AvailableOptions;

	// ---- Dependencies (referenced Entry DataIds) ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> EditDependencyIds;
};

// ============================================================================
// UOptionCategoryData — group of entries under a tab
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UOptionCategoryData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CategoryId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UOptionEntryData*> Entries;
};

// ============================================================================
// UOptionTabData — top-level settings tab
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UOptionTabData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TabName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UOptionCategoryData*> Categories;
};

// ============================================================================
// UOptionsConfigData — master config holding all tabs
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UOptionsConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UOptionTabData*> Tabs;
};

// ============================================================================
// Free helpers — usable from Registry
// ============================================================================

class FOptionsDataInteractionHelper;

GAMEMAINMENU_API TSharedPtr<FOptionsDataInteractionHelper> CreateHelperFromEnum(EGameUserSettingFunction Func);
GAMEMAINMENU_API class UListDataObject_Base* CreateEntryFromData(class UOptionEntryData* Data);
GAMEMAINMENU_API class UListDataObject_Collection* CreateCollectionFromData(const FName& Id, const FText& DisplayName);
