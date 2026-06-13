// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Math/Range.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;
class UListDataObject_StringBool;
class UListDataObject_Scalar;
class UListDataObject_String;
class UListDataObject_StringInteger;
class FOptionsDataInteractionHelper;
class UOptionsConfigData;
class UOptionTabData;
class UOptionEntryData;
enum class ECommonNumericType : uint8;

/**
 * Builds and owns the options data hierarchy (tabs -> categories -> entries).
 *
 * Supports two initialization paths:
 * - DataAsset-driven via UOptionsConfigData (InitFromDataAsset)
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
	void SetOptionsConfig(UOptionsConfigData* InConfig) { OptionsConfig = InConfig; }

	/** Returns the top-level collection tabs after initialization. */
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsCollectionTabs() { return RegisteredOptionsCollectionTabs;}

	/** Returns all flattened list items for a given tab, including nested category children. */
	TArray<UListDataObject_Base*> GetListSourceItemBySelectedTabId(const FName& InSelectedTabId) const;

private:

	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitGameplayCollectionTab();
	void InitAudioCollectionTab();
	void InitVideoCollectionTab();
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

	// ---- Data-driven init ----
	void InitFromDataAsset(ULocalPlayer* InOwningLocalPlayer);
	UListDataObject_Base* CreateEntryFromDataAsset(UOptionEntryData* Data, UListDataObject_Collection* OwnerCategory) const;
	UListDataObject_Base* FindEntryInCategory(const FName& Id, UListDataObject_Collection* Cat) const;

	// ---- Factory helpers ----

	UListDataObject_Collection* CreateCollection(const FName& Id, const FText& DisplayName) const;

	UListDataObject_StringBool* CreateBoolEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
		bool bDefaultValue) const;

	UListDataObject_Scalar* CreateScalarEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
		const TRange<float>& DisplayRange, const TRange<float>& OutRange,
		float StepSize, float DefaultValue,
		ECommonNumericType NumericType) const;

	UListDataObject_String* CreateStringEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter,
		const TArray<TPair<FString, FText>>& Options,
		const FString& DefaultValue) const;

	UListDataObject_StringInteger* CreateQualityPresetEntry(
		const FName& Id, const FText& DisplayName, const FText& Description,
		const TSharedPtr<FOptionsDataInteractionHelper>& Getter,
		const TSharedPtr<FOptionsDataInteractionHelper>& Setter) const;

	UPROPERTY(EditAnywhere, Category = "Options")
	UOptionsConfigData* OptionsConfig;

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsCollectionTabs;

};
