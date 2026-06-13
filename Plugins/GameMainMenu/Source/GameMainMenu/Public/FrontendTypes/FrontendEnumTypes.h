#pragma once

/** Type of confirm dialog: single OK button, Yes/No, or OK/Cancel. */
UENUM(BlueprintType)
enum class EConfirmScreenType : uint8
{
	Ok,
	YesNo,
	OkCancel,
	UnKnown UMETA(Hidden)
};


/** Result of a confirm dialog button click. */
UENUM(BlueprintType)
enum class EConfirmScreenButtonType : uint8
{
	Confirmed,
	Cancelled,
	Closed,
	UnKnown UMETA(Hidden)

};


/** Reason an options list data change was triggered. */
UENUM(BlueprintType)
enum class EOptionsListDataModifyReason : uint8
{
	DirectlyModified,    /**< User directly changed the value. */
	DependencyModified,  /**< A dependency value changed, triggering this update. */
	ResetToDefault,      /**< User reset this entry to default. */
	UnKnown UMETA(Hidden)

};
