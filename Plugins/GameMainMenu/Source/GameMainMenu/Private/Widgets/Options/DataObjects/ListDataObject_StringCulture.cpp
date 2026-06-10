// HangHang All Rights Reserved

#include "Widgets/Options/DataObjects/ListDataObject_StringCulture.h"

#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"

void UListDataObject_StringCulture::OnDataObjectInitialized()
{
	InitializeCultureOptions();

	Super::OnDataObjectInitialized();
}

void UListDataObject_StringCulture::InitializeCultureOptions()
{
	const UFrontendDeveloperSettings* DevSettings = GetDefault<UFrontendDeveloperSettings>();
	if (!DevSettings) return;

	const TArray<FString>& CultureNames = DevSettings->SupportedCultures;
	if (CultureNames.IsEmpty()) return;

	const TArray<FCultureRef> AvailableCultures = FInternationalization::Get().GetAvailableCultures(CultureNames, false);
	for (const FCultureRef& Culture : AvailableCultures)
	{
		AddDynamicString(Culture->GetName(), FText::FromString(Culture->GetDisplayName()));
	}
}
