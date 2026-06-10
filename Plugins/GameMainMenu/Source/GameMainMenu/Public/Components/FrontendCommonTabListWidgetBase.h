// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonTabListWidgetBase.h"
#include "FrontendCommonTabListWidgetBase.generated.h"

class UFrontendCommonButtonBase;


UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UFrontendCommonTabListWidgetBase : public UCommonTabListWidgetBase
{
	GENERATED_BODY()
	
public:

	void RequestRegisterTab(const FName& InTabId,const FText& InTabDisplayName);

private:

	//~ Begin UWidget Interface
#if WITH_EDITOR	
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
	//~ End UWidget Interface

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Frontend Tab List Settings",meta = (AllowPrivateAccess = "true",ClampMin = "1",ClampMax = "10"))
	int32 DebugEditorPreviewTabCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Frontend Tab List Settings", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UFrontendCommonButtonBase> TabButtonEntryWidgetClass;
	
};
