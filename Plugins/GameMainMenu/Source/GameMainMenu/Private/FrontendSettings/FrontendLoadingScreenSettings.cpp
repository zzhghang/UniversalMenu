// HangHang All Rights Reserved


#include "FrontendSettings/FrontendLoadingScreenSettings.h"

#include "Blueprint/UserWidget.h"


TSubclassOf<UUserWidget> UFrontendLoadingScreenSettings::GetSoftLoadScreenWidgetClassCheck() const
{
	if (!ensureMsgf(!LoadingScreenWidget.IsNull(), TEXT("LoadScreenWidget is null in FrontendLoadScreenSettings"))) { return nullptr; }

	return LoadingScreenWidget.LoadSynchronous();
}
