// HangHang All Rights Reserved
#pragma once

#include "CoreMinimal.h"
#include "MenuFormTypes.generated.h"

UENUM(BlueprintType)
enum class EMenuPersistence : uint8
{
	Persistent,
	SessionOnly,
	None
};

UENUM(BlueprintType)
enum class EMenuConditionMatch : uint8 { All, Any };

UENUM(BlueprintType)
enum class EMenuComparison : uint8
{
	Equals, NotEquals, Greater, GreaterOrEqual, Less, LessOrEqual,
	IsTrue, IsFalse, IsEmpty, IsNotEmpty
};

USTRUCT(BlueprintType)
struct GAMEMAINMENU_API FMenuEntryCondition
{
	GENERATED_BODY()

	/** Empty means the same tab as the dependent entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SourceTabId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SourceDataId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EMenuComparison Comparison = EMenuComparison::Equals;

	/** Stored option key, or a number for numeric comparisons. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Comparison == EMenuComparison::Equals || Comparison == EMenuComparison::NotEquals || Comparison == EMenuComparison::Greater || Comparison == EMenuComparison::GreaterOrEqual || Comparison == EMenuComparison::Less || Comparison == EMenuComparison::LessOrEqual", EditConditionHides))
	FString Value;

	bool Matches(const FString& Actual) const;
};
