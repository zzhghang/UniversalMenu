// HangHang All Rights Reserved


#include "Widgets/Options/W_OptionScreen.h"

#include "FrontendDebugHelper.h"
#include "Input/CommonUIInputTypes.h"
#include "ICommonInputModule.h"
#include "Components/FrontendCommonListView.h"
#include "Components/FrontendCommonTabListWidgetBase.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Widgets/Options/OptionsDataRegistry.h"
#include "Widgets/Options/W_OptionDetailView.h"
#include "Widgets/Options/DataObjects/ListDataObject_Collection.h"
#include "Widgets/Options/ListEntries/W_ListEntry_Base.h"
#include "Subsystems/FrontendUISubsystem.h"
#include "LocalizationKeys.h"

void UW_OptionScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (!ResetAction.IsNull())
	{
		ResetActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAction,
				true,
				FSimpleDelegate::CreateUObject(
					this,
					&ThisClass::OnResetBoundActionTriggered
				)
			)
		);
	}
	
	if (!ResetAllAction.IsNull())
	{
		ResetAllActionHandle = RegisterUIActionBinding(
			FBindUIActionArgs(
				ResetAllAction,
				true,
				FSimpleDelegate::CreateUObject(
					this,
					&ThisClass::OnResetAllBoundActionTriggered
				)
			)
		);
	}
	

	BackActionHandle = RegisterUIActionBinding(
	FBindUIActionArgs(
	ICommonInputModule::GetSettings().GetDefaultBackAction(),
	true,
	FSimpleDelegate::CreateUObject(
		this,
		&ThisClass::OnBackBoundActionTriggered
			)
		)
	);
	
	
	TabListWidget_OptionTabs->OnTabSelected.AddUniqueDynamic(this,&ThisClass::OnOptionsTabSelected);
	CommonListView_OptionList->OnItemIsHoveredChanged().AddUObject(this,&ThisClass::OnListViewItemHovered);
	CommonListView_OptionList->OnItemSelectionChanged().AddUObject(this,&ThisClass::OnListViewItemSelected);
}

void UW_OptionScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	for (const UListDataObject_Collection* CollectionTab : GetOrCreateDataRegistry()->GetRegisteredOptionsCollectionTabs())
	{
		if (!CollectionTab) continue;

		FName const TabId = CollectionTab->GetDataId();

		if (TabListWidget_OptionTabs->GetTabButtonBaseByID(TabId) != nullptr) continue;

		TabListWidget_OptionTabs->RequestRegisterTab(TabId, CollectionTab->GetDataDisplayName());
	}
	const FName SelectedTabId = TabListWidget_OptionTabs->GetSelectedTabId();
	if (!SelectedTabId.IsNone()) OnOptionsTabSelected(SelectedTabId);
}

UWidget* UW_OptionScreen::NativeGetDesiredFocusTarget() const
{
	if (const UObject* SelectedObject = CommonListView_OptionList->GetSelectedItem())
	{
		if (UUserWidget* SelectedWidget = CommonListView_OptionList->GetEntryWidgetFromItem(SelectedObject))
		{
			return SelectedWidget;
		} 
	}

	return Super::NativeGetDesiredFocusTarget();
}

void UW_OptionScreen::OnResetBoundActionTriggered()
{
	UListDataObject_Base* SelectedItem = CommonListView_OptionList->GetSelectedItem<UListDataObject_Base>();
	if (!SelectedItem) return;

	SelectedItem->TryResetBackToDefaultValue();
}

void UW_OptionScreen::OnResetAllBoundActionTriggered()
{
	if (ResettableDataArray.IsEmpty()) return;

	TWeakObjectPtr<UW_OptionScreen> WeakThis(this);
	UFrontendUISubsystem::Get(this)->PushConfirmWidgetToModalStackAsync(
		EConfirmScreenType::YesNo,
		FText::FromString(TEXT("")),
		TXT_OPT_RESET_CONFIRM(),
		[WeakThis](EConfirmScreenButtonType ClickedButtonType)
		{
			if (ClickedButtonType != EConfirmScreenButtonType::Confirmed) return;
			if (!WeakThis.IsValid()) return;

			TArray<UListDataObject_Base*> ItemsToReset = MoveTemp(WeakThis->ResettableDataArray);

			WeakThis->bIsResettingData = true;

			for (UListDataObject_Base* Item : ItemsToReset)
			{
				if (Item)
				{
					Item->TryResetBackToDefaultValue();
				}
			}

			WeakThis->bIsResettingData = false;

			for (UListDataObject_Base* Item : ItemsToReset)
			{
				if (Item && Item->CanResetBackToDefaultValue())
				{
					WeakThis->ResettableDataArray.AddUnique(Item);
				}
			}

			if (WeakThis->ResettableDataArray.IsEmpty())
			{
				WeakThis->RemoveActionBinding(WeakThis->ResetActionHandle);
				WeakThis->RemoveActionBinding(WeakThis->ResetAllActionHandle);
			}
			else
			{
				if (!WeakThis->GetActionBindings().Contains(WeakThis->ResetActionHandle))
				{
					WeakThis->AddActionBinding(WeakThis->ResetActionHandle);
				}
				if (!WeakThis->GetActionBindings().Contains(WeakThis->ResetAllActionHandle))
				{
					WeakThis->AddActionBinding(WeakThis->ResetAllActionHandle);
				}
			}
		}
	);
}

void UW_OptionScreen::OnBackBoundActionTriggered()
{
	if (CreatedDataRegistry)
	{
		if (UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get())
		{
			CreatedDataRegistry->CommitPendingSettings(Settings);
			Settings->ApplySettings(true);
		}
	}
	DeactivateWidget();
	// Reopening starts a fresh draft from the applied settings.
	CreatedDataRegistry = nullptr;
}

void UW_OptionScreen::OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered)
{
	if (!InHoveredItem) return;

	UW_ListEntry_Base* HoveredEntryWidget = CommonListView_OptionList->GetEntryWidgetFromItem<UW_ListEntry_Base>(InHoveredItem);

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
	else
	{
		if (UListDataObject_Base* SelectedItem = CommonListView_OptionList->GetSelectedItem<UListDataObject_Base>())
		{
			DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
				SelectedItem,
				TryGetEntryWidgetClassName(SelectedItem)
			);
		}
	}
	
	//UpdateResetActionBindingForDataItem(HoveredDataObject);
}

void UW_OptionScreen::OnListViewItemSelected(UObject* InSelectedItem)
{
	if (!InSelectedItem) return;

	UListDataObject_Base* SelectedDataObject = CastChecked<UListDataObject_Base>(InSelectedItem);
	
	DetailsView_ListEntryInfo->UpdateDetailsViewInfo(
	SelectedDataObject,
	TryGetEntryWidgetClassName(InSelectedItem)
	);
	
	UpdateResetActionBindingForDataItem(SelectedDataObject);

}

void UW_OptionScreen::OnOptionsTabSelected(const FName TabId)
{
	DetailsView_ListEntryInfo->ClearDetailsViewInfo();
	
	const TArray<UListDataObject_Base*> FoundListSourceItems = GetOrCreateDataRegistry()->GetListSourceItemBySelectedTabId(TabId);
	
	CommonListView_OptionList->SetListItems(FoundListSourceItems);
	CommonListView_OptionList->RequestRefresh();
	
	if (CommonListView_OptionList->GetNumItems() != 0)
	{
		CommonListView_OptionList->NavigateToIndex(0);
		CommonListView_OptionList->SetSelectedIndex(0);
	}
	
	ResettableDataArray.Empty();
	bIsSelectingTab = true;
	
	for (UListDataObject_Base* FoundListSourceItem : FoundListSourceItems)
	{
		if (!FoundListSourceItem)
		{
			continue;
		}
		if (!FoundListSourceItem->OnListDataModified.IsBoundToObject(this))
		{
			FoundListSourceItem->OnListDataModified.AddUObject(this,&ThisClass::OnListViewListDataModified);

		}
		if (FoundListSourceItem->IsDataCurrentlyEditable() && FoundListSourceItem->CanResetBackToDefaultValue())
		{
			ResettableDataArray.AddUnique(FoundListSourceItem);
		}
	}
	bIsSelectingTab = false;
	
	if (ResettableDataArray.IsEmpty())
	{
		RemoveActionBinding(ResetActionHandle);
		RemoveActionBinding(ResetAllActionHandle);
	}
	else
	{
		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
		if (!GetActionBindings().Contains(ResetAllActionHandle))
		{
			AddActionBinding(ResetAllActionHandle);
		} 
	}
}

void UW_OptionScreen::OnListViewListDataModified(UListDataObject_Base* ModifiedData,
	EOptionsListDataModifyReason ModifiedReason)
{
	if (!ModifiedData || bIsResettingData || bIsSelectingTab) return;

	UpdateResetActionBindingForDataItem(ModifiedData);
}

void UW_OptionScreen::UpdateResetActionBindingForDataItem(UListDataObject_Base* DataItem)
{
	if (!DataItem) return;

	if (DataItem->IsDataCurrentlyEditable() && DataItem->CanResetBackToDefaultValue())
	{
		ResettableDataArray.AddUnique(DataItem);

		if (!GetActionBindings().Contains(ResetActionHandle))
		{
			AddActionBinding(ResetActionHandle);
		}
		if (!GetActionBindings().Contains(ResetAllActionHandle))
		{
			AddActionBinding(ResetAllActionHandle);
		}
	}
	else
	{
		ResettableDataArray.Remove(DataItem);
		RemoveActionBinding(ResetActionHandle);

		if (ResettableDataArray.IsEmpty())
		{
			RemoveActionBinding(ResetAllActionHandle);
		}
	}
}

UOptionsDataRegistry* UW_OptionScreen::GetOrCreateDataRegistry()
{
	if (!CreatedDataRegistry)
	{
		CreatedDataRegistry = NewObject<UOptionsDataRegistry>(this);
		CreatedDataRegistry->SetOptionsConfig(OptionsConfig);
		CreatedDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());
	}

	ensureMsgf(CreatedDataRegistry, TEXT("Data Registry for options screen is not valid"));

	return CreatedDataRegistry;
}

FString UW_OptionScreen::TryGetEntryWidgetClassName(const UObject* InListItem) const
{
	if (const UUserWidget* FoundEntryWidget = CommonListView_OptionList->GetEntryWidgetFromItem(InListItem))
	{
		return FoundEntryWidget->GetClass()->GetName();
	}

	return TEXT("Entry Widget Not Valid");
}
