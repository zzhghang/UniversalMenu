// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "W_ListEntry_Base.generated.h"

class UListDataObject_Base;
class UCommonTextBlock;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ListEntry_Base : public UCommonUserWidget,public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintImplementableEvent,meta = (DisplayName = "On List Entry Widget Hovered"))
	void BP_OnListEntryWidgetHovered(bool bIsHovered, bool bIsEntryWidgetStillSelected);
	
	void NativeOnListEntryWidgetHovered(bool bIsHovered);
	
protected:
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Get Widget To Focus For Gamepad"))
	UWidget* BP_GetWidgetToFocusForGamepad() const;

	UFUNCTION(BlueprintImplementableEvent,meta = (DisplayName = "On Toggle Entry Widget HighLight State"))
	void BP_OnToggleEntryWidgetHighLightState(bool bIsHighlighted) const;
	
	// ~Begin IUserObjectListEntry Interface
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	virtual void NativeOnItemSelectionChanged(const bool bIsSelected) override;
	
	virtual void NativeOnEntryReleased() override;
	// ~End IUserObjectListEntry Interface
	
	// ~Begin UUserWidget Interface
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	// ~End UUserWidget Interface
	
	// 子类需要重写进行初始化,super是必须的
	virtual void OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject);
	
	// 子类需要重写更新ui界面上的值,在数据改变被修改时,super不需要
	virtual void OnOwningListDataObjectModified(UListDataObject_Base* OwningModifyData, EOptionsListDataModifyReason OwningModifyReason);
	
	virtual void OnOwningDependencyDataModified(UListDataObject_Base* DependencyModifyData, EOptionsListDataModifyReason DependencyModifyReason);
	
	// 子类需要重写进行切换条目可编辑状态,super是必须的
	virtual void OnToggleEditableState(bool bIsEditable);
	
	void SelectedThisEntryWidget();
	
private:

	/***** Bind Widget *****/
	UPROPERTY(BlueprintReadOnly,meta = (BindWidgetOptional,AllowPrivateAccess = "true"))
	UCommonTextBlock* Ctb_EntryDisplayName;
	/***** Bind Widget *****/
	
	UPROPERTY(Transient)
	UListDataObject_Base* CachedOwningListDataObject;
};
