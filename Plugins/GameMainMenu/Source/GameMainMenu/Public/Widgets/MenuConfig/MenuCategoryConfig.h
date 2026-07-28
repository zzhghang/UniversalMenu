// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Widgets/MenuConfig/MenuEntryConfig.h"
#include "MenuCategoryConfig.generated.h"

// ============================================================================
// UMenuCategoryConfig — unified category DataAsset replacing UOptionCategoryData + UMultiplayerCategoryConfig
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMenuCategoryConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CategoryId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UMenuEntryConfig*> Entries;
};
