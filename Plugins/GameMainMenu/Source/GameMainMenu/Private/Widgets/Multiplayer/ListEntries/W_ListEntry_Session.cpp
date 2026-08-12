// HangHang All Rights Reserved

#include "Widgets/Multiplayer/ListEntries/W_ListEntry_Session.h"

#include "CommonTextBlock.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Session.h"

void UW_ListEntry_Session::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
	Super::OnOwningListDataObjectSet(InOwningListDataObject);

	UListDataObject_Session* SessionData = CastChecked<UListDataObject_Session>(InOwningListDataObject);
	const FSessionListItem& Data = SessionData->GetSessionData();

	Tb_ServerName->SetText(Data.ServerName);
	Tb_MapName->SetText(Data.MapName);
	Tb_Players->SetText(FText::Format(
		FText::FromString(TEXT("{0}/{1}")),
		FText::AsNumber(Data.CurrentPlayers),
		FText::AsNumber(Data.MaxPlayers)));
	Tb_Private->SetText(Data.bIsPrivate
		? FText::FromString(TEXT("Yes"))
		: FText::FromString(TEXT("No")));
	Tb_Ping->SetText(FText::Format(
		FText::FromString(TEXT("{0}ms")),
		FText::AsNumber(Data.Ping)));
}
