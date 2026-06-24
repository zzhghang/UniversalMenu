// HangHang All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Widgets/W_ActivatableBase.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "CommonLazyImage.h"
#include "W_CreditsScreen.generated.h"

class UCreditsDataAsset;
class UCommonTextStyle;

UCLASS(Abstract, BlueprintType, meta=(DisableNativeTick))
class GAMEMAINMENU_API UW_CreditsScreen : public UW_ActivatableBase
{
	GENERATED_BODY()

protected:
	// ~ begin UUserWidget interface
	virtual void NativeOnInitialized() override;
	// ~ end UUserWidget interface

	// ~ begin UCommonActivatableWidget interface
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;
	// ~ end UCommonActivatableWidget interface

private:
	void StartScrolling();
	void UpdateScroll();
	void OnCreditsFinished();
	void PopulateCreditsEntries();

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	UCreditsDataAsset* CreditsData;

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	TSubclassOf<UCommonTextStyle> CreditsTextStyle;

	UPROPERTY(EditDefaultsOnly, Category = "Credits")
	bool bAutoReturnOnFinish = true;

	/***** Bind Widgets *****/
	UPROPERTY(meta = (BindWidget))
	UScrollBox* CreditsScrollBox;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* CreditsContentBox;

	UPROPERTY(meta = (BindWidget))
	UCommonLazyImage* UELogoImage;

	/***** Bind Widgets *****/

	FTimerHandle StartDelayHandle;
	FTimerHandle ScrollTickHandle;

	float CurrentScrollOffset = 0.f;

	static constexpr float ScrollTickInterval = 0.016f;
};
