// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MultiplayerDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;
class UMultiplayerConfigData;

/**
 * Builds and owns the multiplayer data hierarchy (tabs -> categories -> entries).
 *
 * Supports two initialization paths:
 * - DataAsset-driven via UMultiplayerConfigData
 * - Hardcoded fallback when no DataAsset is assigned (mirrors UOptionsDataRegistry)
 */
UCLASS()
class GAMEMAINMENU_API UMultiplayerDataRegistry : public UObject
{
	GENERATED_BODY()

public:

	/** Initialize all tabs, either from DataAsset or hardcoded fallback. */
	void InitRegistry();

	/** Assign a DataAsset config. Must be called before InitRegistry. */
	void SetConfig(UMultiplayerConfigData* InConfig) { Config = InConfig; }

	/** Returns the top-level tab collections after initialization. */
	const TArray<UListDataObject_Collection*>& GetRegisteredTabs() const { return RegisteredTabs; }

	/** Returns all flattened list items for a given tab. */
	TArray<UListDataObject_Base*> GetListItemsForTab(const FName& TabId) const;

private:

	void InitFromConfig();

	/** Fallback used when no config DataAsset is assigned. */
	void InitHardcodedFallback();

	void FindChildListDataRecursively(const UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	UListDataObject_Collection* CreateCollection(const FName& Id, const FText& DisplayName) const;

	UPROPERTY()
	UMultiplayerConfigData* Config;

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredTabs;
};
