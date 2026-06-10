// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "OptionsDataRegistry.generated.h"

class UListDataObject_Base;
class UListDataObject_Collection;

UCLASS()
class GAMEMAINMENU_API UOptionsDataRegistry : public UObject
{
	GENERATED_BODY()
	
public:

	void InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer);

	const TArray<UListDataObject_Collection*>& GetRegisteredOptionsCollectionTabs() { return RegisteredOptionsCollectionTabs;}

	TArray<UListDataObject_Base*> GetListSourceItemBySelectedTabId(const FName& InSelectedTabId) const;

private:
	
	void FindChildListDataRecursively(UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;

	void InitGameplayCollectionTab();
	
	void InitAudioCollectionTab();
	
	void InitVideoCollectionTab();
	
	void InitControlCollectionTab(ULocalPlayer* InOwningLocalPlayer);

	UPROPERTY(Transient)
	TArray<UListDataObject_Collection*> RegisteredOptionsCollectionTabs;
	
};
