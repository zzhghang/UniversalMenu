// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_MultiplayerScreen.generated.h"

class UFrontendCommonTabListWidgetBase;
class UFrontendCommonListView;
class UW_OptionDetailView;
class UListDataObject_Base;
class UMenuSessionData;
class UMultiplayerDataRegistry;
class UMenuConfigData;

/**
 * Multiplayer screen with two tabs: Create Game and Join Game.
 *
 * Mirrors UW_OptionScreen architecture:
 *   MultiplayerConfigData (DataAsset) → MultiplayerDataRegistry → Screen
 * Fallback hardcoded tabs when no DataAsset is assigned.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_MultiplayerScreen : public UW_ActivatableBase
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

	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Screen")
	void OnStartGameActionTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Screen")
	void OnJoinGameActionTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Screen")
	void OnRefreshActionTriggered();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Screen")
	void OnHandleCreateGameTabSelected();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Multiplayer Screen")
	void OnHandleJoinGameTabSelected();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Screen")
	UMenuSessionData* SaveSessionData();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Screen")
	void LoadSessionData();

	UFUNCTION(BlueprintPure, Category = "Multiplayer Screen")
	UMenuSessionData* GetCachedSessionData() const { return CachedSessionData; }

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Screen")
	void PopulateSessionList(const TArray<struct FSessionListItem>& Sessions);

private:

	void OnBackBoundActionTriggered();

	UFUNCTION()
	void OnMultiplayerTabSelected(const FName TabId);

	void OnListViewItemHovered(UObject* InHoveredItem, bool bWasHovered);

	void OnListViewItemSelected(UObject* InSelectedItem);

	/** Lazy-creates the data registry and initializes from config or fallback. */
	UMultiplayerDataRegistry* GetOrCreateDataRegistry();

	/** Returns the widget class name for the entry representing an item (for debug display). */
	FString TryGetEntryWidgetClassName(const UObject* InListItem) const;

	/** Optional DataAsset for designer-configurable tabs. Falls back to hardcoded when null. */
	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer Screen")
	UMenuConfigData* MultiplayerConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle StartGameAction;

	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle JoinGameAction;

	UPROPERTY(EditDefaultsOnly, Category = "Multiplayer Screen", meta = (RowType = "/Script/CommonUI.CommonInputActionDataBase"))
	FDataTableRowHandle RefreshAction;

	FUIActionBindingHandle StartGameActionHandle;

	FUIActionBindingHandle JoinGameActionHandle;

	FUIActionBindingHandle RefreshActionHandle;

	FUIActionBindingHandle BackActionHandle;

	FName CurrentTabId;

	UPROPERTY(Transient)
	UMultiplayerDataRegistry* CreatedDataRegistry;

	UPROPERTY(Transient)
	UMenuSessionData* CachedSessionData;

	/***** Bind Widget *****/
	UPROPERTY(meta = (BindWidget))
	UFrontendCommonTabListWidgetBase* TabListWidget_MultiplayerTabs;

	UPROPERTY(meta = (BindWidget))
	UFrontendCommonListView* CommonListView_MultiplayerList;

	UPROPERTY(meta = (BindWidget))
	UW_OptionDetailView* DetailsView_ListEntryInfo;
	/***** Bind Widget *****/
};
