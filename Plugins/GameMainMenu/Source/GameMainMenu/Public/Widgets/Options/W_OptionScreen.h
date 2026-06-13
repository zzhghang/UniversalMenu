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
class UOptionsConfigData;
/**
 * 
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
	
	void OnResetBoundActionTriggered();
	
	void OnResetAllBoundActionTriggered();
	
	void OnBackBoundActionTriggered();
	
	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);
	
	void OnListViewItemSelected(UObject* InSelectedItem);
	
	UFUNCTION()
	void OnOptionsTabSelected(const FName TabId);
	
	void OnListViewListDataModified(UListDataObject_Base* ModifiedData, EOptionsListDataModifyReason ModifiedReason);

	void UpdateResetActionBindingForDataItem(UListDataObject_Base* DataItem);

	UOptionsDataRegistry* GetOrCreateDataRegistry();
	
	FString TryGetEntryWidgetClassName(const UObject* InListItem) const;

	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAction;
	
	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle ResetAllAction;

	UPROPERTY(EditDefaultsOnly, Category = "Frontend Options Screen")
	UOptionsConfigData* OptionsConfig;
	
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
	
};
