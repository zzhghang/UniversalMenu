// HangHang All Rights Reserved
#include "Widgets/Multiplayer/MultiplayerDataRegistry.h"
#include "LogGameMainMenu.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Multiplayer/MenuSessionData.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Session.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"

void UMultiplayerDataRegistry::InitRegistry()
{
	RegisteredTabs.Reset();
	FormEntries.Reset();
	SessionListParent = nullptr;
	bConfigurationErrors = false;
	if (!ensureMsgf(Config, TEXT("MultiplayerDataRegistry requires a Config DataAsset.")))
	{
		bConfigurationErrors = true;
		return;
	}
	InitFromConfig();
	WireConditions();
#if WITH_EDITOR
	// Keep designer preview data out of packaged games.
	if (Config->bPreviewSessionsInEditor)
	{
		TArray<FSessionListItem> TestSessions;
		TestSessions.SetNum(5);
		for (int32 i = 0; i < TestSessions.Num(); ++i)
		{
			TestSessions[i].SessionIndex = i;
			TestSessions[i].ServerName = FText::FromString(FString::Printf(TEXT("Test Server %d"), i + 1));
			TestSessions[i].MapName = FText::FromString(FString::Printf(TEXT("Map_0%d"), i + 1));
		}
		RebuildSessionList(TestSessions);
	}
#endif
}

void UMultiplayerDataRegistry::InitFromConfig()
{
	TSet<FName> TabIds;
	TSet<FName> SaveKeys;
	bool bExplicitSessionParent = false;
	for (UMenuTabConfig* TabData : Config->Tabs)
	{
		if (!TabData) continue;
		if (TabData->TabId.IsNone() || TabIds.Contains(TabData->TabId))
		{
			UE_LOG(LogGameMainMenu, Warning, TEXT("Multiplayer config: empty or duplicate TabId %s"), *TabData->TabId.ToString());
			bConfigurationErrors = true;
			continue;
		}
		TabIds.Add(TabData->TabId);
		UListDataObject_Collection* Tab = CreateCollection(TabData->TabId, TabData->TabName);
		RegisteredTabs.Add(Tab);
		for (UMenuCategoryConfig* CatData : TabData->Categories)
		{
			if (!CatData) continue;
			UListDataObject_Collection* Parent = Tab;
			if (CatData->HeaderColumns.IsEmpty())
			{
				Parent = CreateCollection(CatData->CategoryId, CatData->CategoryName);
				Tab->AddChildListData(Parent);
			}
			else
			{
				auto* Header = NewObject<UListDataObject_HeaderRow>(this);
				Header->SetHeaderConfig(CatData->HeaderColumns, CatData->HeaderColumnGap, CatData->HeaderEdgePadding, CatData->HeaderVerticalPadding);
				Parent->AddChildListData(Header);
			}
			const bool bLegacyList = CatData->Entries.ContainsByPredicate([](const UMenuEntryConfig* Entry)
			{
				return Entry && Entry->DataId == TEXT("ServerList");
			});
			if (TabData->TabId == Config->JoinGameTabId)
			{
				if (!SessionListParent) SessionListParent = Parent;
				if (CatData->bSessionList || bLegacyList)
				{
					if (bExplicitSessionParent)
					{
						UE_LOG(LogGameMainMenu, Warning, TEXT("Multiplayer config: multiple session list categories"));
						bConfigurationErrors = true;
					}
					else SessionListParent = Parent;
					bExplicitSessionParent = true;
				}
			}
			for (UMenuEntryConfig* EntryData : CatData->Entries)
			{
				if (!EntryData) continue;
				// Legacy marker selects the list container; it is not a text input or a saved field.
				if (TabData->TabId == Config->JoinGameTabId && EntryData->DataId == TEXT("ServerList")) continue;
				if (EntryData->DataId.IsNone() || FindEntry(TabData->TabId, EntryData->DataId))
				{
					UE_LOG(LogGameMainMenu, Warning, TEXT("Multiplayer config: empty or duplicate DataId %s in %s"), *EntryData->DataId.ToString(), *TabData->TabId.ToString());
					bConfigurationErrors = true;
					continue;
				}
				UListDataObject_Base* Entry = CreateEntryFromConfig(EntryData);
				if (!Entry) continue;
				Entry->SetShouldApplyChangeImmediately(false);
				Parent->AddChildListData(Entry);
				if (auto* Choice = Cast<UListDataObject_String>(Entry))
				{
					if (!Choice->TrySetValueFromString(Choice->GetCurrentStringValue()) && !Choice->GetOptionsTextArray().IsEmpty())
					{
						Choice->GotoNextOption();
						Choice->SetDefaultValueFromString(Choice->GetCurrentStringValue());
					}
				}
				FormEntries.Add({TabData->TabId, EntryData, Entry});
				Entry->TryGetValueAsString(FormEntries.Last().InitialValue);
				const FName Key = EntryData->SaveKey.IsNone() ? EntryData->DataId : EntryData->SaveKey;
				{
					if (SaveKeys.Contains(Key))
					{
						UE_LOG(LogGameMainMenu, Warning, TEXT("Multiplayer config: duplicate SaveKey %s"), *Key.ToString());
						bConfigurationErrors = true;
					}
					SaveKeys.Add(Key);
				}
			}
		}
		if (TabData->TabId == Config->JoinGameTabId && !SessionListParent) SessionListParent = Tab;
	}
}

UListDataObject_Base* UMultiplayerDataRegistry::FindEntry(FName TabId, FName DataId) const
{
	for (const FFormEntry& Entry : FormEntries)
		if (Entry.TabId == TabId && Entry.Item.IsValid() && Entry.Item->GetDataId() == DataId) return Entry.Item.Get();
	return nullptr;
}

void UMultiplayerDataRegistry::WireConditions()
{
	for (const FFormEntry& Entry : FormEntries)
	{
		UMenuEntryConfig* EntryConfig = Entry.Config.Get();
		UListDataObject_Base* Target = Entry.Item.Get();
		if (!EntryConfig || !Target) continue;
		TArray<FMenuEntryCondition> Rules = EntryConfig->EditConditions;
		const bool bLegacy = Rules.IsEmpty();
		TSet<UListDataObject_Base*> Sources;
		for (FName Id : EntryConfig->EditDependencyIds)
		{
			UListDataObject_Base* Source = FindEntry(Entry.TabId, Id);
			if (Source) Sources.Add(Source);
			if (!Source || (bLegacy && Source->IsA<UListDataObject_StringBool>()))
			{
				FMenuEntryCondition Rule;
				Rule.SourceDataId = Id;
				Rule.Comparison = EMenuComparison::IsTrue;
				Rules.Add(Rule);
			}
		}
		for (const FMenuEntryCondition& Rule : Rules)
		{
			UListDataObject_Base* Source = FindEntry(Rule.SourceTabId.IsNone() ? Entry.TabId : Rule.SourceTabId, Rule.SourceDataId);
			if (Source) Sources.Add(Source);
			else
			{
				UE_LOG(LogGameMainMenu, Warning, TEXT("Multiplayer config: missing condition source %s for %s"), *Rule.SourceDataId.ToString(), *EntryConfig->DataId.ToString());
				bConfigurationErrors = true;
			}
		}
		for (UListDataObject_Base* Source : Sources)
		{
			const TWeakObjectPtr<UListDataObject_Base> WeakTarget(Target);
			// Only refresh editability, do not rebroadcast a value edit (cycles remain bounded).
			Source->OnListDataModified.AddWeakLambda(Target, [WeakTarget](UListDataObject_Base* Changed, EOptionsListDataModifyReason Reason)
			{
				if (WeakTarget.IsValid()) WeakTarget->OnDependencyDataModified.Broadcast(Changed, Reason);
			});
		}
		if (Rules.IsEmpty()) continue;
		const TWeakObjectPtr<UMultiplayerDataRegistry> WeakRegistry(this);
		const FName TabId = Entry.TabId;
		const EMenuConditionMatch Match = bLegacy ? EMenuConditionMatch::All : EntryConfig->ConditionMatch;
		FOptionsDataEditConditionDescriptor Condition;
		Condition.SetDisableRichReason(EntryConfig->DisableRichReason.ToString());
		Condition.SetEditConditionFunction([WeakRegistry, Rules, TabId, Match]()
		{
			if (!WeakRegistry.IsValid()) return false;
			bool bResult = Match == EMenuConditionMatch::All;
			for (const FMenuEntryCondition& Rule : Rules)
			{
				UListDataObject_Base* Source = WeakRegistry->FindEntry(Rule.SourceTabId.IsNone() ? TabId : Rule.SourceTabId, Rule.SourceDataId);
				FString Value;
				if (!Source || !Source->TryGetValueAsString(Value)) return false;
				const bool bMatches = Rule.Matches(Value);
				bResult = Match == EMenuConditionMatch::All ? bResult && bMatches : bResult || bMatches;
			}
			return bResult;
		});
		Target->AddEditConditionDescriptor(Condition);
	}
}

bool UMultiplayerDataRegistry::CanMigrateLegacyKey(const UMenuEntryConfig* Entry) const
{
	if (Entry->SaveKey.IsNone() || Entry->SaveKey == Entry->DataId) return false;
	for (const FFormEntry& Other : FormEntries)
	{
		const UMenuEntryConfig* OtherConfig = Other.Config.Get();
		if (!OtherConfig || OtherConfig == Entry) continue;
		const FName OtherKey = OtherConfig->SaveKey.IsNone() ? OtherConfig->DataId : OtherConfig->SaveKey;
		if (OtherConfig->DataId == Entry->DataId || OtherKey == Entry->DataId) return false;
	}
	return true;
}

bool UMultiplayerDataRegistry::SaveFormValues(UMenuSessionData* Data) const
{
	if (!Data || bConfigurationErrors) return false;
	for (const FFormEntry& Entry : FormEntries)
	{
		const UMenuEntryConfig* EntryConfig = Entry.Config.Get();
		if (!EntryConfig || !Entry.Item.IsValid()) continue;
		const FName Key = EntryConfig->SaveKey.IsNone() ? EntryConfig->DataId : EntryConfig->SaveKey;
		if (CanMigrateLegacyKey(EntryConfig)) Data->RemoveSavedValue(EntryConfig->DataId);
		FString Value;
		if (EntryConfig->Persistence == EMenuPersistence::None) Data->RemoveSavedValue(Key);
		else if (Entry.Item->TryGetValueAsString(Value))
		{
			if (EntryConfig->Persistence == EMenuPersistence::SessionOnly) Data->SetSessionValue(Key, Value);
			else Data->SetSavedValue(Key, Value);
		}
	}
	return true;
}

void UMultiplayerDataRegistry::LoadFormValues(const UMenuSessionData* Data)
{
	if (!Data || bConfigurationErrors) return;
	for (const FFormEntry& Entry : FormEntries)
	{
		const UMenuEntryConfig* EntryConfig = Entry.Config.Get();
		if (!EntryConfig || !Entry.Item.IsValid()) continue;
		if (EntryConfig->Persistence == EMenuPersistence::None)
		{
			Entry.Item->TrySetValueFromString(Entry.InitialValue);
			continue;
		}
		const FName Key = EntryConfig->SaveKey.IsNone() ? EntryConfig->DataId : EntryConfig->SaveKey;
		FString Value;
		bool bFound = false;
		if (EntryConfig->Persistence == EMenuPersistence::SessionOnly)
		{
			if (const FString* Found = Data->SessionValues.Find(Key)) { Value = *Found; bFound = true; }
		}
		else
		{
			bFound = Data->TryGetSavedValue(Key, Value);
			if (!bFound && CanMigrateLegacyKey(EntryConfig)) bFound = Data->TryGetSavedValue(EntryConfig->DataId, Value);
		}
		if (!bFound || !Entry.Item->TrySetValueFromString(Value)) Entry.Item->TrySetValueFromString(Entry.InitialValue);
	}
}

TArray<UListDataObject_Base*> UMultiplayerDataRegistry::GetListItemsForTab(const FName& TabId) const
{
	TArray<UListDataObject_Base*> Result;
	for (const UListDataObject_Collection* Tab : RegisteredTabs)
		if (Tab && Tab->GetDataId() == TabId) { FindChildListDataRecursively(Tab, Result); break; }
	return Result;
}

void UMultiplayerDataRegistry::FindChildListDataRecursively(const UListDataObject_Base* Parent, TArray<UListDataObject_Base*>& Result) const
{
	if (!Parent) return;
	for (UListDataObject_Base* Child : Parent->GetAllChildListData())
	{
		if (!Child) continue;
		Result.Add(Child);
		FindChildListDataRecursively(Child, Result);
	}
}

UListDataObject_Collection* UMultiplayerDataRegistry::CreateCollection(const FName& Id, const FText& DisplayName) const
{
	auto* Collection = NewObject<UListDataObject_Collection>();
	Collection->SetDataId(Id);
	Collection->SetDataDisplayName(DisplayName);
	return Collection;
}

void UMultiplayerDataRegistry::RebuildSessionList(const TArray<FSessionListItem>& Sessions)
{
	if (!SessionListParent) return;
	for (UListDataObject_Base* Child : SessionListParent->GetAllChildListData())
		if (Child && Child->IsA<UListDataObject_Session>()) SessionListParent->RemoveChildListData(Child);
	for (const FSessionListItem& Session : Sessions)
	{
		auto* Item = NewObject<UListDataObject_Session>(this);
		Item->SetSessionData(Session);
		SessionListParent->AddChildListData(Item);
	}
}
