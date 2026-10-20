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

	// Native-name labels for the cultures this project ships with.
	// Kept in sync with WebPreview/reference/extract_content.py.
	static const TMap<FString, FString> CultureDisplayNames = []()
	{
		TMap<FString, FString> Names;
		Names.Add(TEXT("en"), TEXT("English"));
		Names.Add(TEXT("zh-Hans"), TEXT("简体中文"));
		Names.Add(TEXT("zh-Hant"), TEXT("繁體中文"));
		Names.Add(TEXT("ja"), TEXT("日本語"));
		Names.Add(TEXT("ko"), TEXT("한국어"));
		Names.Add(TEXT("fr"), TEXT("Français"));
		Names.Add(TEXT("de"), TEXT("Deutsch"));
		Names.Add(TEXT("it"), TEXT("Italiano"));
		Names.Add(TEXT("ru"), TEXT("Русский"));
		return Names;
	}();

	const TArray<FCultureRef> AvailableCultures = FInternationalization::Get().GetAvailableCultures(CultureNames, false);
	for (const FCultureRef& Culture : AvailableCultures)
	{
		const FString CultureName = Culture->GetName();
		const FString* DisplayName = CultureDisplayNames.Find(CultureName);
		AddDynamicString(CultureName, FText::FromString(DisplayName ? *DisplayName : Culture->GetDisplayName()));
	}
}
