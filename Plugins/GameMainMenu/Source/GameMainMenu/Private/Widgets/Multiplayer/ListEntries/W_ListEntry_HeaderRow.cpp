// HangHang All Rights Reserved


#include "Widgets/Multiplayer/ListEntries/W_ListEntry_HeaderRow.h"

#include "CommonTextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.h"

void UW_ListEntry_HeaderRow::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	UListDataObject_HeaderRow* HeaderData = CastChecked<UListDataObject_HeaderRow>(InOwningListDataObject);
	if (!Hb_HeaderColumns) return;

	Hb_HeaderColumns->ClearChildren();

	const TArray<FText>& Columns = HeaderData->GetColumns();
	const float Gap = HeaderData->GetColumnGap();
	const float Edge = HeaderData->GetEdgePadding();
	const float VertPad = HeaderData->GetVerticalPadding();
	const int32 Count = Columns.Num();

	for (int32 i = 0; i < Count; ++i)
	{
		UCommonTextBlock* TextBlock = NewObject<UCommonTextBlock>(this);
		TextBlock->SetText(Columns[i]);

		if (HeaderTextStyle)
		{
			TextBlock->SetStyle(HeaderTextStyle);
		}

		UHorizontalBoxSlot* HSlot = Hb_HeaderColumns->AddChildToHorizontalBox(TextBlock);
		FMargin SlotPadding;
		SlotPadding.Top = VertPad;
		SlotPadding.Bottom = VertPad;
		if (i == 0) { SlotPadding.Left = Edge; }
		if (i < Count - 1) { SlotPadding.Right = Gap; }
		else { SlotPadding.Right = Edge; }
		HSlot->SetPadding(SlotPadding);
	}
}
