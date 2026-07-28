// HangHang All Rights Reserved


#include "Widgets/Multiplayer/MultiplayerDataRegistry.h"

#include "LocalizationKeys.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

static const FName TabId_CreateGame(TEXT("CreateGame"));
static const FName TabId_JoinGame(TEXT("JoinGame"));

void UMultiplayerDataRegistry::InitRegistry()
{
	RegisteredTabs.Empty();

	if (Config && !Config->Tabs.IsEmpty())
	{
		InitFromConfig();
	}
	else
	{
		InitHardcodedFallback();
	}
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

		// Wire edit dependencies
		for (UMenuCategoryConfig* CatData : TabData->Categories)
		{
			if (!CatData) continue;

			for (UMenuEntryConfig* EntryData : CatData->Entries)
			{
				if (!EntryData || EntryData->EditDependencyIds.IsEmpty()) continue;

				UListDataObject_Base* const* EntryPtr = EntryLookup.Find(EntryData->DataId);
				if (!EntryPtr || !*EntryPtr) continue;

				for (const FName& DepId : EntryData->EditDependencyIds)
				{
					UListDataObject_Base* const* DepPtr = EntryLookup.Find(DepId);
					if (DepPtr && *DepPtr)
					{
						(*EntryPtr)->AddEditDependencyData(*DepPtr);
					}
				}
			}
		}

		RegisteredTabs.Add(Tab);
	}
}

void UMultiplayerDataRegistry::InitHardcodedFallback()
{
	// --- Create Game tab ---
	UListDataObject_Collection* CreateGameTab = CreateCollection(TabId_CreateGame, TXT_MP_TAB_CREATE_GAME());

	// -- Basic category --
	UListDataObject_Collection* BasicCat = CreateCollection(FName(TEXT("Cat-Basic")), TXT_MP_CAT_BASIC());

	UListDataObject_Editable* GameName = NewObject<UListDataObject_Editable>();
	GameName->SetDataId(FName(TEXT("GameName")));
	GameName->SetDataDisplayName(TXT_MP_LABEL_GAME_NAME());
	GameName->SetDescriptionRichText(TXT_MP_DESC_GAME_NAME());
	GameName->SetHintText(TXT_MP_HINT_GAME_NAME());
	BasicCat->AddChildListData(GameName);

	UListDataObject_StringBool* HasPassword = NewObject<UListDataObject_StringBool>();
	HasPassword->SetDataId(FName(TEXT("HasPassword")));
	HasPassword->SetDataDisplayName(TXT_MP_LABEL_HAS_PASSWORD());
	HasPassword->SetDescriptionRichText(TXT_MP_DESC_HAS_PASSWORD());
	HasPassword->SetFalseAsDefaultValue();
	BasicCat->AddChildListData(HasPassword);

	UListDataObject_Editable* Password = NewObject<UListDataObject_Editable>();
	Password->SetDataId(FName(TEXT("Password")));
	Password->SetDataDisplayName(TXT_MP_LABEL_PASSWORD());
	Password->SetDescriptionRichText(TXT_MP_DESC_PASSWORD());
	Password->SetHintText(TXT_MP_HINT_PASSWORD());
	Password->SetbIsPassword(true);
	Password->SetMaxCharCount(6);
	Password->SetCharFilter(EEditableCharFilter::NumbersOnly);
	// Password depends on HasPassword — disabled when HasPassword is false
	Password->AddEditDependencyData(HasPassword);
	{
		FOptionsDataEditConditionDescriptor PasswordCondition;
		PasswordCondition.SetEditConditionFunction(
			[HasPassword]() -> bool { return HasPassword->GetCurrentStringValue() == TEXT("true"); });
		Password->AddEditConditionDescriptor(PasswordCondition);
	}
	BasicCat->AddChildListData(Password);

	CreateGameTab->AddChildListData(BasicCat);

	// -- Advanced category --
	UListDataObject_Collection* AdvancedCat = CreateCollection(FName(TEXT("Cat-Advanced")), TXT_MP_CAT_ADVANCED());

	// MaxPlayers — selectable player count
	{
		UListDataObject_StringInteger* Entry = NewObject<UListDataObject_StringInteger>();
		Entry->SetDataId(FName(TEXT("MaxPlayers")));
		Entry->SetDataDisplayName(TXT_MP_LABEL_MAX_PLAYERS());
		Entry->SetDescriptionRichText(TXT_MP_DESC_MAX_PLAYERS());
		Entry->AddIntegerOption(2, FText::FromString(TEXT("2")));
		Entry->AddIntegerOption(4, FText::FromString(TEXT("4")));
		Entry->AddIntegerOption(8, FText::FromString(TEXT("8")));
		Entry->AddIntegerOption(16, FText::FromString(TEXT("16")));
		AdvancedCat->AddChildListData(Entry);
	}

	// Map — selectable map names
	{
		UListDataObject_String* Entry = NewObject<UListDataObject_String>();
		Entry->SetDataId(FName(TEXT("Map")));
		Entry->SetDataDisplayName(TXT_MP_LABEL_MAP());
		Entry->SetDescriptionRichText(TXT_MP_DESC_MAP());
		Entry->AddDynamicString(TEXT("Map_01"), FText::FromString(TEXT("Map 01")));
		Entry->AddDynamicString(TEXT("Map_02"), FText::FromString(TEXT("Map 02")));
		Entry->AddDynamicString(TEXT("Map_03"), FText::FromString(TEXT("Map 03")));
		AdvancedCat->AddChildListData(Entry);
	}

	// GameMode — selectable modes
	{
		UListDataObject_String* Entry = NewObject<UListDataObject_String>();
		Entry->SetDataId(FName(TEXT("GameMode")));
		Entry->SetDataDisplayName(TXT_MP_LABEL_GAME_MODE());
		Entry->SetDescriptionRichText(TXT_MP_DESC_GAME_MODE());
		Entry->AddDynamicString(TEXT("Deathmatch"), FText::FromString(TEXT("Deathmatch")));
		Entry->AddDynamicString(TEXT("TeamDeathmatch"), FText::FromString(TEXT("Team Deathmatch")));
		Entry->AddDynamicString(TEXT("CaptureTheFlag"), FText::FromString(TEXT("Capture the Flag")));
		AdvancedCat->AddChildListData(Entry);
	}

	// ServerRegion — selectable regions
	{
		UListDataObject_String* Entry = NewObject<UListDataObject_String>();
		Entry->SetDataId(FName(TEXT("ServerRegion")));
		Entry->SetDataDisplayName(TXT_MP_LABEL_SERVER_REGION());
		Entry->SetDescriptionRichText(TXT_MP_DESC_SERVER_REGION());
		Entry->AddDynamicString(TEXT("Asia"), FText::FromString(TEXT("Asia")));
		Entry->AddDynamicString(TEXT("Europe"), FText::FromString(TEXT("Europe")));
		Entry->AddDynamicString(TEXT("NorthAmerica"), FText::FromString(TEXT("North America")));
		AdvancedCat->AddChildListData(Entry);
	}

	CreateGameTab->AddChildListData(AdvancedCat);

	RegisteredTabs.Add(CreateGameTab);

	// --- Join Game tab ---
	UListDataObject_Collection* JoinGameTab = CreateCollection(TabId_JoinGame, TXT_MP_TAB_JOIN_GAME());

	UListDataObject_Collection* JoinCat = CreateCollection(FName(TEXT("Cat-Join")), TXT_MP_TAB_JOIN_GAME());

	UListDataObject_Collection* ServerListEntry = NewObject<UListDataObject_Collection>();
	ServerListEntry->SetDataId(FName(TEXT("ServerList")));
	ServerListEntry->SetDataDisplayName(TXT_MP_LABEL_SERVER_LIST());
	ServerListEntry->SetDescriptionRichText(TXT_MP_DESC_SERVER_LIST());
	JoinCat->AddChildListData(ServerListEntry);
	JoinGameTab->AddChildListData(JoinCat);

	RegisteredTabs.Add(JoinGameTab);
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
