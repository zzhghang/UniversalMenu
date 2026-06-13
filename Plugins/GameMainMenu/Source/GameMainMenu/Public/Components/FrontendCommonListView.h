// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonListView.h"
#include "FrontendCommonListView.generated.h"

class UDA_DataListEntryMapping;
/**
 * List view that resolves entry widget classes via UDA_DataListEntryMapping.
 *
 * Overrides OnGenerateEntryWidgetInternal to look up the appropriate widget class
 * from the DataAsset mapping instead of requiring a hardcoded EntryWidgetClass.
 */
UCLASS()
class GAMEMAINMENU_API UFrontendCommonListView : public UCommonListView
{
	GENERATED_BODY()

protected:

	// ~Begin UCommonListView Interface
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, const TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;

	virtual bool OnIsSelectableOrNavigableInternal(UObject* FirstSelectedItem) override;
	// ~End UCommonListView Interface


private:

	//~ Begin UWidget Interface
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface

	/** DataAsset mapping data object types to entry widget classes. */
	UPROPERTY(EditAnywhere,Category = "Frontend List View Settings")
	UDA_DataListEntryMapping* DataListEntryMapping;

};
