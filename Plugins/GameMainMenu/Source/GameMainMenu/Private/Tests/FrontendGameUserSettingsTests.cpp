// HangHang All Rights Reserved

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "FrontendSettings/FrontendDeveloperSettings.h"
#include "FrontendSettings/FrontendGameUserSettings.h"
#include "Internationalization/Culture.h"
#include "Internationalization/Internationalization.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCultureFollowsSystemWhenUnsetTest,
	"GameMainMenu.Settings.Culture.FollowsSystemWhenUnset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCultureFollowsSystemWhenUnsetTest::RunTest(const FString& Parameters)
{
	UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get();
	if (!TestNotNull(TEXT("Frontend settings are available"), Settings)) return false;
	if (!TestTrue(TEXT("Internationalization is initialized"), FInternationalization::IsAvailable())) return false;

	FInternationalization& Internationalization = FInternationalization::Get();
	UFrontendDeveloperSettings* DeveloperSettings = GetMutableDefault<UFrontendDeveloperSettings>();

	const FString OriginalLanguage = Internationalization.GetCurrentLanguage()->GetName();
	const FString OriginalLocale = Internationalization.GetCurrentLocale()->GetName();
	const FString OriginalSetting = Settings->GetCurrentCulture();
	const TArray<FString> OriginalSupportedCultures = DeveloperSettings->SupportedCultures;

	int32 CultureSwitchCount = 0;
	const FDelegateHandle SwitchHandle = Internationalization.OnCultureChanged().AddLambda(
		[&CultureSwitchCount]() { ++CultureSwitchCount; });

	ON_SCOPE_EXIT
	{
		Internationalization.OnCultureChanged().Remove(SwitchHandle);
		DeveloperSettings->SupportedCultures = OriginalSupportedCultures;
		Settings->SetCurrentCulture(OriginalSetting);
		Internationalization.SetCurrentLanguage(OriginalLanguage);
		Internationalization.SetCurrentLocale(OriginalLocale);
	};

	// A Simplified Chinese OS: the engine reports the language "zh" and the locale "zh-CN", while the
	// game ships "zh-Hans". An unset language must resolve to that shipped culture, not to English.
	DeveloperSettings->SupportedCultures = { TEXT("en"), TEXT("zh-Hans"), TEXT("zh-Hant") };
	Settings->SetCurrentCulture(FString());
	Internationalization.SetCurrentLanguage(TEXT("zh"));
	Internationalization.SetCurrentLocale(TEXT("zh-CN"));

	const FString ResolvedCulture = Settings->GetCurrentCulture();
	AddInfo(FString::Printf(TEXT("OS language %s / locale %s resolves to %s"),
		*Internationalization.GetCurrentLanguage()->GetName(),
		*Internationalization.GetCurrentLocale()->GetName(),
		*ResolvedCulture));
	TestEqual(TEXT("Unset language resolves to the supported culture for the OS locale"), ResolvedCulture, FString(TEXT("zh-Hans")));

	CultureSwitchCount = 0;
	Settings->ApplyCurrentCultureSetting();
	TestEqual(TEXT("Resolved language reaches the runtime"), Internationalization.GetCurrentCulture()->GetName(), FString(TEXT("zh-Hans")));
	TestEqual(TEXT("Applying a different language switches once"), CultureSwitchCount, 1);

	// Every immediate setting change re-applies the settings, which must not refresh all text again.
	CultureSwitchCount = 0;
	Settings->ApplyCurrentCultureSetting();
	TestEqual(TEXT("Re-applying an unchanged language does not refresh text"), CultureSwitchCount, 0);

	// A language the user picked wins over the operating system.
	Settings->SetCurrentCulture(TEXT("en"));
	CultureSwitchCount = 0;
	Settings->ApplyCurrentCultureSetting();
	TestEqual(TEXT("Stored language is applied"), Internationalization.GetCurrentCulture()->GetName(), FString(TEXT("en")));
	TestEqual(TEXT("Switching to the stored language refreshes text"), CultureSwitchCount, 1);

	// An OS language the game does not ship falls back to the primary supported culture.
	Internationalization.SetCurrentLanguage(TEXT("es"));
	Internationalization.SetCurrentLocale(TEXT("es-ES"));
	Settings->SetCurrentCulture(FString());
	TestEqual(TEXT("Unshipped OS language falls back to the primary supported culture"), Settings->GetCurrentCulture(), FString(TEXT("en")));

	// With no configured cultures there is nothing to match: the runtime language is left alone.
	DeveloperSettings->SupportedCultures.Reset();
	Settings->SetCurrentCulture(FString());
	CultureSwitchCount = 0;
	Settings->ApplyCurrentCultureSetting();
	TestTrue(TEXT("No supported cultures means no language to resolve"), Settings->GetCurrentCulture().IsEmpty());
	TestEqual(TEXT("Unconfigured culture list leaves the language alone"), Internationalization.GetCurrentCulture()->GetName(), FString(TEXT("es")));
	TestEqual(TEXT("Unconfigured culture list does not refresh text"), CultureSwitchCount, 0);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
