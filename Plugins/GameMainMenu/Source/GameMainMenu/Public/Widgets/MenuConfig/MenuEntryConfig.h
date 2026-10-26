// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CommonNumericTextBlock.h"
#include "GameplayTagContainer.h"
#include "Widgets/MenuConfig/MenuConfigTypes.h"
#include "Widgets/MenuConfig/MenuEntryValueConfig.h"
#include "Widgets/MenuConfig/MenuFormTypes.h"
#include "MenuEntryConfig.generated.h"

// ============================================================================
// UMenuEntryConfig — unified entry DataAsset replacing UOptionEntryData + UMultiplayerEntryConfig
// ============================================================================

UCLASS(BlueprintType, meta = (PrioritizeCategories = "Entry Value Binding Conditions Display"))
class GAMEMAINMENU_API UMenuEntryConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Select a type and edit its parameters inline. Existing assets are upgraded on load. */
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, NoClear, Category = "Value")
	TObjectPtr<UMenuEntryValueConfig> ValueConfig;

	UFUNCTION(BlueprintPure, Category = "Menu Config")
	EMenuEntryValueType GetEffectiveValueType() const;

	/** Compatibility for old assets and C++ callers which still populate the original fields. */
	UMenuEntryValueConfig* CreateLegacyValueConfig(UObject* Outer) const;
	void UpgradeLegacyValueConfig();
	void SynchronizeLegacyFields();

	virtual void PostLoad() override;
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditUndo() override;
#endif

	// ---- Identity and description ----
	/** Entry identifier. Must be unique within its tab; dependencies reference this ID. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entry", meta = (DisplayPriority = "0"))
	FName DataId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entry", meta = (DisplayPriority = "1"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Entry", meta = (DisplayPriority = "2", MultiLine = "true"))
	FText Description;

	/** Legacy mirror for existing Blueprint readers. New code should use GetEffectiveValueType(). */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	EMenuEntryValueType ValueType = EMenuEntryValueType::Bool;

	// ---- Settings binding ----
	/** Options settings getter/setter key. Leave empty for unbound UI data, such as multiplayer form entries. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Binding")
	FName BindingKey;

	/** Options: apply this bound setting on user edits and resets; otherwise defer until Back. Both modes save on Back. Multiplayer form values do not apply game settings. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Binding")
	bool bApplyImmediately = false;

	// ---- Availability and dependency refresh ----
	/** Options only: disables editing in the editor and PIE, with a packaged-build-only reason. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions", meta = (DisplayPriority = "0"))
	bool bPackagedBuildOnly = false;

	/** IDs in the same tab to observe for changes. Multiplayer also requires all referenced Bool entries to be true; Options uses separate edit conditions. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions", meta = (DisplayPriority = "1"))
	TArray<FName> EditDependencyIds;

	/** Multiplayer: when populated, replaces the legacy Bool dependency gate. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|Multiplayer")
	TArray<FMenuEntryCondition> EditConditions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions|Multiplayer")
	EMenuConditionMatch ConditionMatch = EMenuConditionMatch::All;

	/** Multiplayer form persistence. Options continues to use its settings binding. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
	EMenuPersistence Persistence = EMenuPersistence::Persistent;

	/** Empty uses DataId, preserving existing saves and Blueprint lookups. Must be unique across the form. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Persistence")
	FName SaveKey;

	// Serialized compatibility fields. Kept under their original names for existing assets and
	// Blueprint getter nodes, but hidden from Details. ValueConfig is authoritative once present.
	// Do not mark these DeprecatedProperty: old asset values must still deserialize.
	// ---- Bool fields ----
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	bool bDefaultBool = false;

	// ---- Scalar fields ----
	/** Default value in the output range, stored as a numeric string. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	FString DefaultValue;

	/** Minimum value displayed by the slider. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	float ScalarMin = 0.f;

	/** Maximum value displayed by the slider. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	float ScalarMax = 1.f;

	/** Minimum underlying value sent to the settings binding. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	float ScalarOutMin = 0.f;

	/** Maximum underlying value sent to the settings binding. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	float ScalarOutMax = 2.f;

	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	float StepSize = 0.01f;

	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	ECommonNumericType NumericType = ECommonNumericType::Percentage;

	// ---- String fields ----
	/** Empty means no explicit default. FullscreenMode uses enum names; Culture uses a culture code; Resolution uses a resolution string. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	FString DefaultStringValue;

	/** FullscreenMode keys are enum names; an empty map uses the built-in window modes. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	TMap<FString, FText> AvailableOptions;

	// ---- Integer fields ----
	/** Always used as the default, including 0. FullscreenMode: 0 = fullscreen, 1 = borderless, 2 = windowed. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	int32 DefaultIntValue = 0;

	/** Configured options and localized labels. For FullscreenMode, an empty map uses the built-in window modes. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	TMap<int32, FText> AvailableIntegerOptions;

	// ---- Editable fields ----
	/** Maximum input length. Zero means no explicit limit. */
	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	int32 MaxCharCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	bool bIsPassword = false;

	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	FText HintText;

	UPROPERTY(BlueprintReadOnly, Category = "Legacy")
	EEditableCharFilter CharFilter = EEditableCharFilter::All;

	/** Rich-text reason for disabled editing. Also used by Multiplayer's Bool dependency condition. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions", meta = (DisplayPriority = "2", MultiLine = "true"))
	FText DisableRichReason;

	// ---- Additional display metadata ----
	/** Optional image tag resolved through OptionsScreenSoftImageMap and shown in the option detail panel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Display", meta = (Categories = "Frontend.Image"))
	FGameplayTag DescriptionImageTag;

	/** Additional rich text displayed in the option detail panel. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, AdvancedDisplay, Category = "Display", meta = (MultiLine = "true"))
	FText DynamicDetails;
};
