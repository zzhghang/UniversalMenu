// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

struct FOptionsDataEditConditionDescriptor;

class UListDataObject_Base;
class UListDataObject_Collection;
class FOptionsDataInteractionHelper;
class UMenuConfigData;
class UMenuTabConfig;
class UMenuEntryConfig;
class UFrontendGameUserSettings;
class UEnhancedInputUserSettings;
class UEnhancedPlayerMappableKeyProfile;

/**
 * Builds and owns the options data hierarchy (tabs -> categories -> entries).
 *
 * DataAsset-driven via UMenuConfigData (InitFromDataAsset). The Control tab
 * (key bindings) is built dynamically and appended after the DataAsset tabs.
 */
UCLASS()
class GAMEMAINMENU_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()

public:

	/** Initialize all option tabs from the assigned DataAsset. */
	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	/** Transfer the edited values to live settings when leaving the options screen. */
	void CommitPendingSettings(UFrontendGameUserSettings* TargetSettings);

	/** Assign a DataAsset config. Must be called before InitOptionsDataRegistry. */
	void SetOptionsConfig(UMenuConfigData* InConfig) { OptionsConfig = InConfig; }

	/** Returns the top-level collection tabs after initialization. */
	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsCollectionTabs() { return RegisteredOptionsCollectionTabs;}

	/** Returns all flattened list items for a given tab, including nested category children. */
	TArray<UListDataObject_Base*> GetListSourceItemBySelectedTabId(const FName& InSelectedTabId) const;

private:

	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitBindingMap();

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

	UPROPERTY(EditAnywhere, Category = "Options")
	UMenuConfigData* OptionsConfig;

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsCollectionTabs;

	TMap<FName, TSharedPtr<FOptionsDataInteractionHelper>> BindingMap;
	TArray<TFunction<void(UFrontendGameUserSettings*)>> SettingCommitters;

	UPROPERTY(Transient)
	TObjectPtr<UFrontendGameUserSettings> PendingSettings;

	UPROPERTY(Transient)
	TObjectPtr<UEnhancedPlayerMappableKeyProfile> PendingKeyProfile;

	TWeakObjectPtr<UEnhancedInputUserSettings> InputUserSettings;
};
