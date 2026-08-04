// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_HeaderRow.generated.h"

/**
 * Data object for a multi-column header row displayed in a list view.
 * Pure display — no interaction, no save/load, not selectable.
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_HeaderRow : public UListDataObject_Base
{
	GENERATED_BODY()

public:
	void SetHeaderConfig(const TArray<FText>& InColumns, float InColumnGap, float InEdgePadding, float InVerticalPadding);

	const TArray<FText>& GetColumns() const { return Columns; }
	float GetColumnGap() const { return ColumnGap; }
	float GetEdgePadding() const { return EdgePadding; }
	float GetVerticalPadding() const { return VerticalPadding; }

private:
	TArray<FText> Columns;
	float ColumnGap = 40.f;
	float EdgePadding = 20.f;
	float VerticalPadding = 10.f;
};
