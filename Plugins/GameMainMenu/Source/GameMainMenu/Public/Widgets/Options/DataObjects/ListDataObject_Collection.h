// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Options/DataObjects/ListDataObject_Base.h"
#include "ListDataObject_Collection.generated.h"

/**
 * Container data object representing a tab or category in the options hierarchy.
 *
 * Holds child entries (other collections, value entries, key bindings).
 * The OptionsDataRegistry flattens this tree for display in the list view.
 */
UCLASS()
class GAMEMAINMENU_API UListDataObject_Collection : public UListDataObject_Base
{
	GENERATED_BODY()

public:

	/** Add a child data object (entry or sub-category) to this collection. */
	void AddChildListData(UListDataObject_Base* InChildListData);

	// ~Begin UListDataObject_Base Interface
	virtual TArray<UListDataObject_Base*> GetAllChildListData() const override;
	virtual bool HasAnyChildListData() const override;
	// ~End UListDataObject_Base Interface


private:

	UPROPERTY(Transient)
	TArray<UListDataObject_Base*> ChildListDataArray;

};
