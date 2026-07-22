// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MultiplayerConfigData.generated.h"

// ============================================================================
// UMultiplayerEntryConfig — single entry definition
// ============================================================================

UENUM(BlueprintType)
enum class EEditableCharFilter : uint8
{
	All,
	LettersOnly,
	NumbersOnly,
	Alphanumeric
};

UENUM(BlueprintType)
enum class EMultiplayerEntryType : uint8
{
	/** Display-only collection (used for categories, not interactable). */
	Collection,
	/** EditableText entry (room name, password). */
	Editable,
	/** Boolean toggle (has password, etc.). */
	Bool
};

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMultiplayerEntryConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EntryId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMultiplayerEntryType EntryType = EMultiplayerEntryType::Collection;

	/** Ids of other entries this one depends on for edit-state toggling. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> EditDependencyIds;

	/** Extra int param (e.g. max char count for editable). 0 = use default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EntryType==EMultiplayerEntryType::Editable"))
	int32 MaxCharCount = 0;

	/** Meaningful for Editable type: when true, uses password mask. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EntryType==EMultiplayerEntryType::Editable"))
	bool bIsPassword = false;

	/** Placeholder hint text for Editable type (e.g. "请输入房间名"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EntryType==EMultiplayerEntryType::Editable"))
	FText HintText;

	/** Character filter for Editable type. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EntryType==EMultiplayerEntryType::Editable"))
	EEditableCharFilter CharFilter = EEditableCharFilter::All;

	/** Meaningful for Bool type: default value. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "EntryType==EMultiplayerEntryType::Bool"))
	bool bDefaultBool = false;
};

// ============================================================================
// UMultiplayerCategoryConfig — category grouping entries within a tab
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMultiplayerCategoryConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CategoryId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText CategoryName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UMultiplayerEntryConfig*> Entries;
};

// ============================================================================
// UMultiplayerTabConfig — tab with its categories
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMultiplayerTabConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName TabId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText TabName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UMultiplayerCategoryConfig*> Categories;
};

// ============================================================================
// UMultiplayerConfigData — master config holding all tabs
// ============================================================================

UCLASS(BlueprintType)
class GAMEMAINMENU_API UMultiplayerConfigData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UMultiplayerTabConfig*> Tabs;
};
