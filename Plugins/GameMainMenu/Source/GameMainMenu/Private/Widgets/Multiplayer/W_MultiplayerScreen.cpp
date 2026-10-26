// HangHang All Rights Reserved


#include "Widgets/Multiplayer/W_MultiplayerScreen.h"

#include "Components/FrontendCommonListView.h"
#include "Components/FrontendCommonTabListWidgetBase.h"
#include "FrontendGameInstance.h"
#include "Input/CommonUIInputTypes.h"
#include "LogGameMainMenu.h"
#include "ICommonInputModule.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h"
#include "Widgets/Multiplayer/DataObjects/ListDataObject_Session.h"
#include "Widgets/Multiplayer/MenuSessionData.h"
#include "Widgets/Multiplayer/MultiplayerDataRegistry.h"
#include "Widgets/MenuConfig/MenuConfigData.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/DataObjects/ListDataObject_String.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "Widgets/Options/W_OptionDetailView.h"

void UW_MultiplayerScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!StartGameAction.IsNull())
	{
		StartGameActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				StartGameAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnStartGameActionTriggered)
			)
		);
	}

	if (!JoinGameAction.IsNull())
	{
		JoinGameActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				JoinGameAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnJoinGameActionTriggered)
			)
		);
	}

	if (!RefreshAction.IsNull())
	{
		RefreshActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				RefreshAction,
				true,
				FSimpleDelegate::CreateUObject(this, &ThisClass::OnRefreshActionTriggered)
			)
		);
	}

	BackActionHandle = RegisterUIActionBinding(
	FBindUIActionArgs(
		ICommonInputModule::GetSettings().GetDefaultBackAction(),
		true,
		FSimpleDelegate::CreateUObject(this, &ThisClass::OnBackBoundActionTriggered)
	)
);

	TabListWidget_MultiplayerTabs->SetIsFocusable(true);
	TabListWidget_MultiplayerTabs->OnTabSelected.AddUniqueDynamic(this, &ThisClass::OnMultiplayerTabSelected);
	CommonListView_MultiplayerList->OnItemIsHoveredChanged().AddUObject(this, &ThisClass::OnListViewItemHovered);
	CommonListView_MultiplayerList->OnItemSelectionChanged().AddUObject(this, &ThisClass::OnListViewItemSelected);
}

void UW_MultiplayerScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	UMultiplayerDataRegistry* Registry = GetOrCreateDataRegistry();

	for (const UListDataObject_Collection* CollectionTab : Registry->GetRegisteredTabs())
	{
		if (!CollectionTab) continue;

		const FName TabId = CollectionTab->GetDataId();

		if (TabListWidget_MultiplayerTabs->GetTabButtonBaseByID(TabId) != nullptr) continue;

		TabListWidget_MultiplayerTabs->RequestRegisterTab(TabId, CollectionTab->GetDataDisplayName());
	}

	LoadSessionData();
	const FName DesiredTab = CurrentTabId.IsNone() && MultiplayerConfig ? MultiplayerConfig->CreateGameTabId : CurrentTabId;
	if (TabListWidget_MultiplayerTabs->GetTabButtonBaseByID(DesiredTab))
		TabListWidget_MultiplayerTabs->SelectTabByID(DesiredTab);
	else if (!Registry->GetRegisteredTabs().IsEmpty())
		TabListWidget_MultiplayerTabs->SelectTabByID(Registry->GetRegisteredTabs()[0]->GetDataId());
}

void UW_MultiplayerScreen::NativeOnDeactivated()
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();

	Super::NativeOnDeactivated();
}

UWidget* UW_MultiplayerScreen::NativeGetDesiredFocusTarget() const
{
	if (const UObject* SelectedObject = CommonListView_MultiplayerList->GetSelectedItem())
	{
		if (UUserWidget* SelectedWidget = CommonListView_MultiplayerList->GetEntryWidgetFromItem(SelectedObject))
		{
			return SelectedWidget;
		}
	}

	return Super::NativeGetDesiredFocusTarget();
}

void UW_MultiplayerScreen::OnBackBoundActionTriggered()
{
	SaveSessionData();
	DeactivateWidget();
}


void UW_MultiplayerScreen::OnMultiplayerTabSelected(const FName TabId)
{
	CurrentTabId = TabId;

	DetailsView_ListEntryInfo->ClearDetailsViewInfo();

	const TArray<UListDataObject_Base*> ListItems = GetOrCreateDataRegistry()->GetListItemsForTab(TabId);

	CommonListView_MultiplayerList->SetListItems(ListItems);
	CommonListView_MultiplayerList->RequestRefresh();

	if (CommonListView_MultiplayerList->GetNumItems() != 0)
	{
		CommonListView_MultiplayerList->NavigateToIndex(0);
		CommonListView_MultiplayerList->SetSelectedIndex(0);
	}

	const FName CreateGameTabId = MultiplayerConfig ? MultiplayerConfig->CreateGameTabId : FName(TEXT("CreateGame"));
	const FName JoinGameTabId = MultiplayerConfig ? MultiplayerConfig->JoinGameTabId : FName(TEXT("JoinGame"));

	if (TabId == CreateGameTabId)
	{
		if (!StartGameAction.IsNull() && !GetActionBindings().Contains(StartGameActionHandle))
		{
			AddActionBinding(StartGameActionHandle);
		}
		if (!JoinGameAction.IsNull() && GetActionBindings().Contains(JoinGameActionHandle))
		{
			RemoveActionBinding(JoinGameActionHandle);
		}
		if (!RefreshAction.IsNull() && GetActionBindings().Contains(RefreshActionHandle))
		{
			RemoveActionBinding(RefreshActionHandle);
		}
		OnHandleCreateGameTabSelected();
	}
	else if (TabId == JoinGameTabId)
	{
		if (!JoinGameAction.IsNull() && !GetActionBindings().Contains(JoinGameActionHandle))
		{
			AddActionBinding(JoinGameActionHandle);
		}
		if (!StartGameAction.IsNull() && GetActionBindings().Contains(StartGameActionHandle))
		{
			RemoveActionBinding(StartGameActionHandle);
		}
		if (!RefreshAction.IsNull() && !GetActionBindings().Contains(RefreshActionHandle))
		{
			AddActionBinding(RefreshActionHandle);
		}
		OnHandleJoinGameTabSelected();
	}
}

void UW_MultiplayerScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem) return;

	UW_ListEntry_Base* HoveredEntryWidget = CommonListView_MultiplayerList->GetEntryWidgetFromItem<UW_ListEntry_Base>(InHoveredItem);
	if (!ensure(HoveredEntryWidget)) { return; }

	HoveredEntryWidget->NativeOnListEntryWidgetHovered(bWasHovered);

	UListDataObject_Base* HoveredDataObject = CastChecked<UListDataObject_Base>(InHoveredItem);

	if (bWasHovered)
	{
		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			HoveredDataObject,
			TryGetEntryWidgetClassName(InHoveredItem)
		);
	}
	else if (UListDataObject_Base* SelectedItem = CommonListView_MultiplayerList->GetSelectedItem<UListDataObject_Base>())
	{
		DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
			SelectedItem,
			TryGetEntryWidgetClassName(SelectedItem)
		);
	}
}

void UW_MultiplayerScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem) return;

	UListDataObject_Base* SelectedDataObject = CastChecked<UListDataObject_Base>(InSelectedItem);
	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
		SelectedDataObject,
		TryGetEntryWidgetClassName(InSelectedItem)
	);
}

UMultiplayerDataRegistry* UW_MultiplayerScreen::GetOrCreateDataRegistry()
{
	if (!CreatedDataRegistry)
	{
		CreatedDataRegistry = NewObject<UMultiplayerDataRegistry>(this);
		CreatedDataRegistry->SetConfig(MultiplayerConfig);
		CreatedDataRegistry->InitRegistry();
	}

	if (!ensureMsgf(CreatedDataRegistry, TEXT("Data Registry for multiplayer screen is not valid"))) { return nullptr; }

	return CreatedDataRegistry;
}

FString UW_MultiplayerScreen::TryGetEntryWidgetClassName(const UObject* InListItem) const
{
	if (const UUserWidget* FoundEntryWidget = CommonListView_MultiplayerList->GetEntryWidgetFromItem(InListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}

	return TEXT("Entry Widget Not Valid");
}

UMenuSessionData* UW_MultiplayerScreen::SaveSessionData()
{
	bLastSessionSaveSucceeded = false;
	UFrontendGameInstance* GameInstance = Cast<UFrontendGameInstance>(GetGameInstance());
	if (!GameInstance) { OnSessionSaveCompleted(false); return nullptr; }
	UMenuSessionData* SessionData = GameInstance->GetOrCreateMenuSessionData();
	UMultiplayerDataRegistry* Registry = GetOrCreateDataRegistry();
	if (!Registry || !Registry->SaveFormValues(SessionData))
	{
		UE_LOG(LogGameMainMenu, Warning, TEXT("Multiplayer form was not saved: invalid configuration."));
		OnSessionSaveCompleted(false);
		return nullptr;
	}
	bLastSessionSaveSucceeded = GameInstance->TrySaveMenuSessionData();
	OnSessionSaveCompleted(bLastSessionSaveSucceeded);
	return SessionData;
}

void UW_MultiplayerScreen::LoadSessionData()
{
	UFrontendGameInstance* GameInstance = Cast<UFrontendGameInstance>(GetGameInstance());
	if (!GameInstance) return;
	if (UMultiplayerDataRegistry* Registry = GetOrCreateDataRegistry())
		Registry->LoadFormValues(GameInstance->LoadMenuSessionData());
}
UMenuSessionData* UW_MultiplayerScreen::GetCachedSessionData() const
{
	const UFrontendGameInstance* GameInstance = Cast<UFrontendGameInstance>(GetGameInstance());
	return GameInstance ? GameInstance->GetMenuSessionData() : nullptr;
}

FBlueprintSessionResult UW_MultiplayerScreen::GetSelectedSessionResult() const
{
	if (UListDataObject_Session* SelectedSession = CommonListView_MultiplayerList->GetSelectedItem<UListDataObject_Session>())
	{
		return SelectedSession->GetSessionData().SessionResult;
	}
	return FBlueprintSessionResult();
}

void UW_MultiplayerScreen::PopulateSessionList(const TArray<FSessionListItem>& Sessions)
{
	UMultiplayerDataRegistry* Registry = GetOrCreateDataRegistry();
	if (!Registry) return;

	FString SelectedSessionId;
	if (const auto* Selected = CommonListView_MultiplayerList->GetSelectedItem<UListDataObject_Session>(); Selected && Selected->GetSessionData().SessionResult.OnlineResult.IsValid())
		SelectedSessionId = Selected->GetSessionData().SessionResult.OnlineResult.GetSessionIdStr();
	Registry->RebuildSessionList(Sessions);

	const FName JoinGameTabId = MultiplayerConfig ? MultiplayerConfig->JoinGameTabId : FName(TEXT("JoinGame"));
	if (CurrentTabId == JoinGameTabId)
	{
		const TArray<UListDataObject_Base*> ListItems = Registry->GetListItemsForTab(JoinGameTabId);
		CommonListView_MultiplayerList->ClearSelection();
		CommonListView_MultiplayerList->SetListItems(ListItems);
		CommonListView_MultiplayerList->RequestRefresh();
		DetailsView_ListEntryInfo->ClearDetailsViewInfo();
		if (!SelectedSessionId.IsEmpty())
		{
			for (UListDataObject_Base* Item : ListItems)
			{
				const auto* Session = Cast<UListDataObject_Session>(Item);
				if (Session && Session->GetSessionData().SessionResult.OnlineResult.IsValid() && Session->GetSessionData().SessionResult.OnlineResult.GetSessionIdStr() == SelectedSessionId)
				{
					CommonListView_MultiplayerList->SetSelectedItem(Item);
					break;
				}
			}
		}
	}
}
