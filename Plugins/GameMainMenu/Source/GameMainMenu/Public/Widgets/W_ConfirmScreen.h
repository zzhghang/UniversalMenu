// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "FrontendTypes/FrontendEnumTypes.h"
#include "Widgets/W_ActivatableBase.h"
#include "W_ConfirmScreen.generated.h"

class UCommonLazyImage;
class UDynamicEntryBox;
class UCommonTextBlock;


USTRUCT(BlueprintType)
struct FConfirmScreenButtonInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EConfirmScreenButtonType ConfirmScreenButtonType = EConfirmScreenButtonType::UnKnown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ButtonTextToDisplay;
};

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


UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_ConfirmScreen : public UW_ActivatableBase
{
	GENERATED_BODY()
	
public:

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
