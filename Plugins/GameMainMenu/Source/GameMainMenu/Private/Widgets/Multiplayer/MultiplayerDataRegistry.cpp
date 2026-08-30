// HangHang All Rights Reserved


#include "Widgets/Multiplayer/MultiplayerDataRegistry.h"

#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Session.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

static const FName TabId_JoinGame(TEXT("JoinGame"));

void UMultiplayerDataRegistry::InitRegistry()
{
	RegisteredTabs.Empty();

	if (!ensureMsgf(Config, TEXT("MultiplayerDataRegistry requires a Config DataAsset.")))
	{
		return;
	}

	InitFromConfig();

	// ponytail: test entries, replaced by real search results on refresh
	/*TArray<FSessionListItem> TestSessions;
	TestSessions.SetNum(5);
	for (int32 i = 0; i < TestSessions.Num(); ++i)
	{
		TestSessions[i].SessionIndex = i;
		TestSessions[i].ServerName = FText::FromString(FString::Printf(TEXT("Test Server %d"), i + 1));
		TestSessions[i].MapName = FText::FromString(FString::Printf(TEXT("Map_0%d"), i + 1));
	}
	RebuildSessionList(TestSessions);*/
}

void UMultiplayerDataRegistry::InitFromConfig()
{
	for (UMenuTabConfig* TabData : Config->Tabs)
	{
		if (!TabData) continue;

		UListDataObject_Collection* Tab = CreateCollection(TabData->TabId, TabData->TabName);
		TMap<FName, UListDataObject_Base*> EntryLookup;

		for (UMenuCategoryConfig* CatData : TabData->Categories)
		{
			if (!CatData) continue;

			if (!CatData->HeaderColumns.IsEmpty())
			{
				UListDataObject_HeaderRow* HeaderRow = NewObject<UListDataObject_HeaderRow>();
				HeaderRow->SetHeaderConfig(CatData->HeaderColumns, CatData->HeaderColumnGap, CatData->HeaderEdgePadding, CatData->HeaderVerticalPadding);
				Tab->AddChildListData(HeaderRow);

				for (UMenuEntryConfig* EntryData : CatData->Entries)
				{
					if (!EntryData) continue;

					UListDataObject_Base* Entry = CreateEntryFromConfig(EntryData);
					if (!Entry) continue;

					Tab->AddChildListData(Entry);
					EntryLookup.Add(EntryData->DataId, Entry);
				}
			}
			else
			{
				UListDataObject_Collection* Cat = CreateCollection(CatData->CategoryId, CatData->CategoryName);

				for (UMenuEntryConfig* EntryData : CatData->Entries)
				{
					if (!EntryData) continue;

					UListDataObject_Base* Entry = CreateEntryFromConfig(EntryData);
					if (!Entry) continue;

					Cat->AddChildListData(Entry);
					EntryLookup.Add(EntryData->DataId, Entry);
				}

				Tab->AddChildListData(Cat);
			}
		}

		// Wire edit dependencies + auto-create EditConditions
		for (UMenuCategoryConfig* CatData : TabData->Categories)
		{
			if (!CatData) continue;

			for (UMenuEntryConfig* EntryData : CatData->Entries)
			{
				if (!EntryData || EntryData->EditDependencyIds.IsEmpty()) continue;

				UListDataObject_Base* const* EntryPtr = EntryLookup.Find(EntryData->DataId);
				if (!EntryPtr || !*EntryPtr) continue;

				TArray<UListDataObject_StringBool*> BoolDeps;

				for (const FName& DepId : EntryData->EditDependencyIds)
				{
					UListDataObject_Base* const* DepPtr = EntryLookup.Find(DepId);
					if (DepPtr && *DepPtr)
					{
						(*EntryPtr)->AddEditDependencyData(*DepPtr);
						if (UListDataObject_StringBool* BoolDep = Cast<UListDataObject_StringBool>(*DepPtr))
						{
							BoolDeps.Add(BoolDep);
						}
					}
				}

				if (BoolDeps.IsEmpty()) continue;

				FOptionsDataEditConditionDescriptor Condition;
				Condition.SetEditConditionFunction(
					[BoolDeps]() -> bool
					{
						for (const UListDataObject_StringBool* Dep : BoolDeps)
						{
							if (Dep->GetCurrentStringValue() != TEXT("true")) return false;
						}
						return true;
					});

				if (!EntryData->DisableRichReason.IsEmpty())
				{
					Condition.SetDisableRichReason(EntryData->DisableRichReason.ToString());
				}

				(*EntryPtr)->AddEditConditionDescriptor(Condition);
			}
		}

		RegisteredTabs.Add(Tab);
	}
}

TArray<UListDataObject_Base*> UMultiplayerDataRegistry::GetListItemsForTab(const FName& TabId) const
{
	for (const UListDataObject_Collection* Tab : RegisteredTabs)
	{
		if (Tab && Tab->GetDataId() == TabId)
		{
			TArray<UListDataObject_Base*> FlatList;
			FindChildListDataRecursively(Tab, FlatList);
			return FlatList;
		}
	}
	return TArray<UListDataObject_Base*>();
}

void UMultiplayerDataRegistry::FindChildListDataRecursively(const UListDataObject_Base* InParentData,
                                                            TArray<UListDataObject_Base*>& OutFoundChildListData) const
{
	if (!InParentData || !InParentData->HasAnyChildListData()) return;

	for (UListDataObject_Base* SubChildListData : InParentData->GetAllChildListData())
	{
		if (!SubChildListData) continue;

		OutFoundChildListData.Add(SubChildListData);

		if (SubChildListData->HasAnyChildListData())
		{
			FindChildListDataRecursively(SubChildListData, OutFoundChildListData);
		}
	}
}

UListDataObject_Collection* UMultiplayerDataRegistry::CreateCollection(const FName& Id, const FText& DisplayName) const
{
	UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
	Col->SetDataId(Id);
	Col->SetDataDisplayName(DisplayName);
	return Col;
}

void UMultiplayerDataRegistry::RebuildSessionList(const TArray<FSessionListItem>& Sessions)
{
	for (UListDataObject_Collection* Tab : RegisteredTabs)
	{
		if (!Tab || Tab->GetDataId() != TabId_JoinGame) continue;

		for (UListDataObject_Base* Child : Tab->GetAllChildListData())
		{
			UListDataObject_Collection* JoinCat = Cast<UListDataObject_Collection>(Child);
			if (!JoinCat) continue;

			TArray<UListDataObject_Base*> ToRemove;
			for (UListDataObject_Base* CatChild : JoinCat->GetAllChildListData())
			{
				if (Cast<UListDataObject_Session>(CatChild) || CatChild->GetDataId() == FName(TEXT("ServerList")))
				{
					ToRemove.Add(CatChild);
				}
			}
			for (UListDataObject_Base* Rem : ToRemove)
			{
				JoinCat->RemoveChildListData(Rem);
			}

			for (const FSessionListItem& Session : Sessions)
			{
				UListDataObject_Session* SessionObj = NewObject<UListDataObject_Session>();
				SessionObj->SetSessionData(Session);
				JoinCat->AddChildListData(SessionObj);
			}
		}
		break;
	}
}
