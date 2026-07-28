// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "MenuConfigTypes.generated.h"

// ============================================================================
// EEditableCharFilter — moved from MultiplayerConfigData.h
// ============================================================================

UENUM(BlueprintType)
enum class EEditableCharFilter : uint8
{
	All,
	LettersOnly,
	NumbersOnly,
	Alphanumeric
};

// ============================================================================
// EMenuEntryValueType — unified value type replacing EOptionValueType + EMultiplayerEntryType
// ============================================================================

UENUM(BlueprintType)
enum class EMenuEntryValueType : uint8
{
	/** Boolean toggle. */
	Bool,
	/** Numeric slider. */
	Scalar,
	/** Dropdown / enum selection (string values). */
	String,
	/** Free-form text input (Multiplayer editable). */
	Editable,
	/** Integer dropdown (quality presets, player count). */
	Integer,
	/** Culture/locale selection (auto-populated). */
	Culture,
	/** Screen resolution selection (auto-populated). */
	Resolution
};
