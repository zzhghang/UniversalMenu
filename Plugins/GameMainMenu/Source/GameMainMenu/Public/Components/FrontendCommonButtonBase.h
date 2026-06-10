// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FrontendCommonButtonBase.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;

UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UFrontendCommonButtonBase : public UCommonButtonBase
{
	GENERATED_BODY()
	
private:
	
	// begin UUserWidget Interface
	virtual void NativePreConstruct() override;
	// end UUserWidget Interface
	
	// begin UCommonButtonBase Interface
	virtual void NativeOnCurrentTextStyleChanged() override;
	
	virtual void NativeOnHovered() override;
	
	virtual void NativeOnUnhovered() override;
	// end UCommonButtonBase Interface
	
	// ***** Bound Widgets ***** //
	UPROPERTY(meta = (BindWidgetOptional))
	UCommonTextBlock* Ctb_ButtonText;

	UPROPERTY(BlueprintReadOnly,meta = (BindWidgetOptional,AllowPrivateAccess = "true"))
	UCommonLazyImage* Cli_Icon;   
	// ***** Bound Widgets ***** //
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A Frontend Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A Frontend Button", meta = (AllowPrivateAccess = "true"))
	bool bUseUpperCaseForButtonText = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A Frontend Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDescriptionText;
	
public:
	
	UFUNCTION(BlueprintCallable)
	void SetButtonText(const FText InText) const;
	
	UFUNCTION(BlueprintCallable)
	FText GetButtonDisplayText() const;
	
	UFUNCTION(BlueprintCallable)
	void SetButtonIcon(const FSlateBrush& InIcon);
	
};
