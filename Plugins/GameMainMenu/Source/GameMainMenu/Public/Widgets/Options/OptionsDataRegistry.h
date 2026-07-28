// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Math/Range.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

struct FOptionsDataEditConditionDescriptor;

class UListDataObject_Base;
class UListDataObject_Collection;
class UListDataObject_StringBool;
class UListDataObject_Scalar;
class UListDataObject_String;
class UListDataObject_StringInteger;
class FOptionsDataInteractionHelper;
class UMenuConfigData;
class UMenuTabConfig;
class UMenuEntryConfig;
enum class ECommonNumericType : uint8;

/**
 * Builds and owns the options data hierarchy (tabs -> categories -> entries).
 *
 * Supports two initialization paths:
 * - DataAsset-driven via UMenuConfigData (InitFromDataAsset)
 * - Hardcoded fallback when no DataAsset is assigned
 */
UCLASS()
class GAMEMAINMENU_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:

	/** Initialize all option tabs, either from DataAsset or hardcoded fallback. */
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	/** Assign a DataAsset config. Must be called before InitOptionsDataRegistry. */
	void SetOptionsConfig(UMenuConfigData* InConfig) { OptionsConfig = InConfig; }

	/** Returns the top-level collection tabs after initialization. */
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsCollectionTabs() { return RegisteredOptionsCollectionTabs;}

	/** Returns all flattened list items for a given tab, including nested category children. */
	TArray<UListDataObject_Base*> GetListSourceItemBySelectedTabId(const FName& InSelectedTabId) const;

private:

	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitBindingMap();

	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

	// ---- Data-driven init ----
	void InitFromDataAsset(ULocalPlayer* InOwningLocalPlayer);
	void ApplyTabEditConditions(
		UListDataObject_Collection* Tab,
		UMenuTabConfig* TabData,
		const FOptionsDataEditConditionDescriptor& PackagedBuildOnlyCondition,
		const TMap<FName, UListDataObject_Base*>& EntryLookup);

	// ---- Helpers: look up BindingKey in BindingMap, return helper if found ----
	TSharedPtr<FOptionsDataInteractionHelper> ResolveBinding(const FName& BindingKey) const;

	UListDataObject_Collection* CreateCollection(const FName& Id, const FText& DisplayName) const;

	UListDataObject_StringBool* CreateBoolEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const FName& BindingKey, bool bDefaultValue) const;

	UListDataObject_Scalar* CreateScalarEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const FName& BindingKey,
		const TRange<float>& DisplayRange, const TRange<float>& OutRange,
		float StepSize, float DefaultValue,
		ECommonNumericType NumericType) const;

	UListDataObject_String* CreateStringEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const FName& BindingKey,
		const TArray<TPair<FString, FText>>& Options,
		const FString& DefaultValue) const;

	UListDataObject_StringInteger* CreateQualityPresetEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const FName& BindingKey, int32 DefaultValue = 3) const;

	UPROPERTY(EditAnywhere, Category = "Options")
	UMenuConfigData* OptionsConfig;

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsCollectionTabs;

	TMap<FName, TSharedPtr<FOptionsDataInteractionHelper>> BindingMap;
};
