// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_ConfirmScreen.generated.h"

class UCommonLazyImage;
class UDynamicEntryBox;
class UCommonTextBlock;


/** Describes a single button on a confirm dialog (type + display text). */
USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::UnKnown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonTextToDisplay;
};

/**
 * Transient info object that configures a confirm dialog before it is pushed.
 *
 * Created via static factory methods (CreateOkScreen, CreateYesNoScreen, CreateOkCancelScreen).
 */
UCLASS()
class GAMEMAINMENU_API UConfirmScreenInfoObject : public UObject
{
	GENERATED_BODY()

public:

	static UConfirmScreenInfoObject* CreateOkScreen(const FText& InTitle, const FText& InMessage);

	static UConfirmScreenInfoObject* CreateYesNoScreen(const FText& InTitle, const FText& InMessage);

	static UConfirmScreenInfoObject* CreateOkCancelScreen(const FText& InTitle, const FText& InMessage);

	UPROPERTY(Transient)
	FText Title;

	UPROPERTY(Transient)
	FText Message;

	UPROPERTY(Transient)
	TArray<FConfirmScreenButtonInfo> AvailableScreenButtons;
};


/**
 * Modal confirm dialog with configurable buttons (OK, Yes/No, OK/Cancel).
 *
 * Buttons are created dynamically via UDynamicEntryBox and each fires
 * the provided callback with the corresponding EConfirmScreenButtonType.
 */
UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ConfirmScreen : public UW_ActivatableBase
{
	GENERATED_BODY()

public:

	/** Initialize the dialog with screen info and a per-button click callback. */
	void InitConfirmScreen(UConfirmScreenInfoObject* InScreenInfoObject, TFunction<void(EConfirmScreenButtonType)> ClickButtonCallBack);

protected:

	// ~Begin UCommonActivatableWidget Interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	// ~End UCommonActivatableWidget Interface

private:
	/********  Bind Widget  *********/

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ctb_Title;

	UPROPERTY(meta = (BindWidget))
	UCommonTextBlock* Ctb_Message;

	UPROPERTY(meta = (BindWidget))
	UDynamicEntryBox* DynamicEntryBox_Buttons;

	UPROPERTY(BlueprintReadOnly,meta = (BindWidgetOptional,AllowPrivateAccess = "true"))
	UCommonLazyImage* Cli_Icon;

	/********  Bind Widget  *********/


};
