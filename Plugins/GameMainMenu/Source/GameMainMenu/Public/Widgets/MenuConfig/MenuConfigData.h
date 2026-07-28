// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Widgets/MenuConfig/MenuTabConfig.h"
#include "MenuConfigData.generated.h"

class UListDataObject_Base;
class FOptionsDataInteractionHelper;

// ============================================================================
// UMenuConfigData — unified master DataAsset replacing UOptionsConfigData + UMultiplayerConfigData
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMenuConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UMenuTabConfig*> Tabs;
};

// ============================================================================
// Free helpers
// ============================================================================

/** Factory: creates a UListDataObject_Base subclass from a UMenuEntryConfig DataAsset. */
GAMEMAINMENU_API UListDataObject_Base* CreateEntryFromConfig(const UMenuEntryConfig* Data);

/** Convenience: creates a collection DataObject. */
GAMEMAINMENU_API class UListDataObject_Collection* CreateCollectionFromConfig(const FName& Id, const FText& DisplayName);
