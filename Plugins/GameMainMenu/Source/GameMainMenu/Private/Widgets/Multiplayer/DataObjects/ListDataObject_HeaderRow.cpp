// HangHang All Rights Reserved


#include "Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.h"

void UListDataObject_HeaderRow::SetHeaderConfig(const TArray<FText>& InColumns, float InColumnGap, float InEdgePadding, float InVerticalPadding)
{
	Columns = InColumns;
	ColumnGap = InColumnGap;
	EdgePadding = InEdgePadding;
	VerticalPadding = InVerticalPadding;
}
