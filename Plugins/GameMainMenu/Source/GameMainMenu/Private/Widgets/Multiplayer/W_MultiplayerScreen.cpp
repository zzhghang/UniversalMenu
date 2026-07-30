// HangHang All Rights Reserved


#include "Widgets/Multiplayer/W_MultiplayerScreen.h"

#include "Components/FrontendCommonListView.h"
#include "Components/FrontendCommonTabListWidgetBase.h"
#include "FrontendDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Widgets/Multiplayer/MultiplayerDataRegistry.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
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

	TabListWidget_MultiplayerTabs->SelectTabByID(FName(TEXT("CreateGame")));
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
	DeactivateWidget();
}

void UW_MultiplayerScreen::OnStartGameActionTriggered()
{
	Debug::Print(TEXT("MultiplayerScreen - StartGame action triggered"));
}

void UW_MultiplayerScreen::OnJoinGameActionTriggered()
{
	Debug::Print(TEXT("MultiplayerScreen - JoinGame action triggered"));
}

void UW_MultiplayerScreen::OnMultiplayerTabSelected(const FName TabId)
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();

	const TArray<UListDataObject_Base*> ListItems = GetOrCreateDataRegistry()->GetListItemsForTab(TabId);

	CommonListView_MultiplayerList->SetListItems(ListItems);
	CommonListView_MultiplayerList->RequestRefresh();

	if (CommonListView_MultiplayerList->GetNumItems() != 0)
	{
		CommonListView_MultiplayerList->NavigateToIndex(0);
		CommonListView_MultiplayerList->SetSelectedIndex(0);
	}

	static const FName CreateGameTabId(TEXT("CreateGame"));
	static const FName JoinGameTabId(TEXT("JoinGame"));

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
		CreatedDataRegistry = NewObject<UMultiplayerDataRegistry>();
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
