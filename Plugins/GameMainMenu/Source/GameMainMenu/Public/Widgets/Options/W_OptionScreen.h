// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_OptionScreen.generated.h"

class UListDataObject_Base;
class UW_OptionDetailView;
class UFrontendCommonListView;
class UFrontendCommonTabListWidgetBase;
class UOptionsDataRegistry;
class UMenuConfigData;

/**
 * Main options screen widget with tabbed navigation.
 *
 * Owns a UOptionsDataRegistry to build the option hierarchy and manages
 * reset-to-default action bindings for modified settings.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_OptionScreen : public UW_ActivatableBase
{
	GENERATED_BODY()

protected:

	// ~ begin UUserWidget interface
	virtual void NativeOnInitialized() override;
	// ~ end UUserWidget interface

	// ~ begin UCommonActivableWidget interface
	virtual void NativeOnActivated() override;

	virtual void NativeOnDeactivated() override;

	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	// ~ end UCommonActivableWidget interface

private:

	/** Reset the currently selected data item to its default value. */
	void OnResetBoundActionTriggered();

	/** Reset all modified items on the current tab after user confirmation. */
	void OnResetAllBoundActionTriggered();

	void OnBackBoundActionTriggered();

	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);

	void OnListViewItemSelected(UObject* InSelectedItem);

	UFUNCTION()
	void OnOptionsTabSelected(const FName TabId);

	void OnListViewListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason);

	/** Show or hide reset action bindings based on whether a data item can be reset. */
	void UpdateResetActionBindingForDataItem(UListDataObject_Base* DataItem);

	/** Lazy-creates the data registry and initializes it from the assigned config (or fallback). */
	UOptionsDataRegistry* GetOrCreateDataRegistry();

	/** Returns the widget class name for the entry representing an item (for debug display). */
	FString TryGetEntryWidgetClassName(const UObject* InListItem) const;

	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAllAction;

	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen")
	UMenuConfigData* OptionsConfig;
	
	FUIActionBindingHandle ResetActionHandle;
	
	FUIActionBindingHandle ResetAllActionHandle;
	
	FUIActionBindingHandle BackActionHandle;
	
	UPROPERTY(Transient)
	UOptionsDataRegistry* CreatedDataRegistry;
	
	/***** Bind Widget *****/
	UPROPERTY(meta = (BindWidget))
	UFrontendCommonTabListWidgetBase* TabListWidget_OptionTabs;
	
	UPROPERTY(meta = (BindWidget))
	UFrontendCommonListView* CommonListView_OptionList;
	
	UPROPERTY(meta = (BindWidget))
	UW_OptionDetailView* DetailsView_ListEntryInfo;
	/***** Bind Widget *****/
	
	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ResettableDataArray;

	bool bIsResettingData = false;

	bool bIsSelectingTab = false;

};
