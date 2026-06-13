// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FrontendCommonTabListWidgetBase.generated.h"

class UFrontendCommonButtonBase;


/**
 * Tab list widget that dynamically registers tabs at runtime.
 *
 * Unlike CommonTabListWidgetBase which requires pre-configured tabs,
 * this allows options screens to register tabs from data (DataAsset or hardcoded).
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UFrontendCommonTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()

public:

	/** Register a tab dynamically. No-op if a tab with the same ID already exists. */
	void RequestRegisterTab(const FName& InTabId,const FText& InTabDisplayName);

private:

	//~ Begin UWidget Interface
#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface

	/** Number of placeholder tabs shown in the editor for preview purposes. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Frontend Tab List Settings",meta = (AllowPrivateAccess = "true",ClampMin = "1",ClampMax = "10"))
	int32 DebugEditorPreviewTabCount = 3;

	/** Widget class used for each tab button entry. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Tab List Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFrontendCommonButtonBase> TabButtonEntryWidgetClass;

};
