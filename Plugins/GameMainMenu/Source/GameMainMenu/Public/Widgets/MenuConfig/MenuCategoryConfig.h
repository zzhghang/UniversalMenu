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

	/** Multiplayer: receive dynamic search results here. Legacy ServerList entries still work. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Multiplayer")
	bool bSessionList = false;

	/** Multi-column header labels. When non-empty, a header row is prepended to this category in the list. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> HeaderColumns;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeaderColumnGap = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeaderEdgePadding = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HeaderVerticalPadding = 10.f;
};
