// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Widgets/MenuConfig/MenuCategoryConfig.h"
#include "MenuTabConfig.generated.h"

// ============================================================================
// UMenuTabConfig — unified tab DataAsset replacing UOptionTabData + UMultiplayerTabConfig
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMenuTabConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TabName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UMenuCategoryConfig*> Categories;
};
