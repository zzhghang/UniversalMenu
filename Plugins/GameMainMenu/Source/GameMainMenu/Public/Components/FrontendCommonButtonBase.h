// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "FrontendCommonButtonBase.generated.h"

class UCommonTextBlock;
class UCommonLazyImage;

/**
 * Common button base with text/icon support and description text broadcasting.
 *
 * On hover, broadcasts description text via UFrontendUISubsystem::OnButtonDescriptionTextUpdate
 * for display in a shared description panel.
 */
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

	/** Default display text shown on the button when not overridden via SetButtonText. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A Frontend Button", meta = (AllowPrivateAccess = "true"))
	FText ButtonDisplayText;

	/** When true, button text is forced to uppercase via NativeOnCurrentTextStyleChanged. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "A Frontend Button", meta = (AllowPrivateAccess = "true"))
	bool bUseUpperCaseForButtonText = false;

	/** Description text broadcast on hover for tooltip/help panels. */
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
