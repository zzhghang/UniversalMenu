// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_MultiplayerScreen.generated.h"

class UFrontendCommonTabListWidgetBase;
class UFrontendCommonListView;
class UW_OptionDetailView;
class UListDataObject_Base;
class UMultiplayerDataRegistry;
class UMultiplayerConfigData;

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
	UMultiplayerConfigData* MultiplayerConfig;

	FUIActionBindingHandle BackActionHandle;

	UPROPERTY(Transient)
	UMultiplayerDataRegistry* CreatedDataRegistry;

	/***** Bind Widget *****/
	UPROPERTY(meta = (BindWidget))
	UFrontendCommonTabListWidgetBase* TabListWidget_MultiplayerTabs;

	UPROPERTY(meta = (BindWidget))
	UFrontendCommonListView* CommonListView_MultiplayerList;

	UPROPERTY(meta = (BindWidget))
	UW_OptionDetailView* DetailsView_ListEntryInfo;
	/***** Bind Widget *****/
};
