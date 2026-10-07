[ocr]   ✘ file_read failed: file "Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_StringResolution.h" not found: git show master:Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_StringResolution.h: exit status 128: fatal: path 'Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_StringResolution.h' does not exist in 'master'

[ocr] Summary: 85 file(s) reviewed, 53 comment(s), ~5096252 token(s) used (input: ~4797837, output: ~298415), cache(read: ~3815808, write: ~0), 11m5s elapsed

[2m─── Config/DefaultEngine.ini:122-122 ───[0m
[94m[style · low][0m The file is missing a trailing newline at the end. This is a POSIX standard and can
cause spurious diffs or warnings from tools like Git, compilers, and linters. Consider adding a
blank line at the end of the file.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m NetConnectionClassName="OnlineSubsystemSteam.SteamNetConnection"[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/FrontendSettings/FrontendLoadingScreenSettings.cpp:11-11 ───[0m
[94m[style · low][0m Typo in the log message: the class name is "FrontendLoadingScreenSettings" but the
message says "FrontendLoadScreenSettings" (missing "ing"). This would prevent finding this log entry
when searching for the actual class name during debugging.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	if (!ensureMsgf(!LoadingScreenWidget.IsNull(), TEXT("LoadScreenWidget is null in FrontendLoadScreenSettings"))) { return nullptr; }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	if (!ensureMsgf(!LoadingScreenWidget.IsNull(), TEXT("LoadingScreenWidget is null in FrontendLoadingScreenSettings"))) { return nullptr; }[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/FunctionLibrary/FrontendFunctionLibrary.cpp:15-17 ───[0m
[93m[maintainability · medium][0m Changing from checkf to ensureMsgf alters the failure behavior: the
function no longer halts on missing tags and instead returns a default (null) value via FindRef.
Callers that previously relied on the result always being valid may now receive null soft pointers,
potentially causing deferred null-dereference issues harder to trace than the original immediate
crash. Consider either: (a) keeping checkf if this truly represents an unrecoverable programmer
error, or (b) returning an explicit fallback/empty value and documenting that callers must handle
null returns.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m ensureMsgf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag), TEXT("Can not find widget under the tag %s"), *InWidgetTag.ToString());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Option A: Keep the hard check if this is truly an unrecoverable error[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m checkf(FrontendDeveloperSettings->FrontendWidgetMap.Contains(InWidgetTag), TEXT("Can not find widget under the tag %s"), *InWidgetTag.ToString());[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	return FrontendDeveloperSettings->FrontendWidgetMap.FindRef(InWidgetTag);[0m[0m


[2m─── Plugins/GameMainMenu/Config/DefaultGameMainMenu.ini:5-5 ───[0m
[91m[bug · high][0m Missing `Frontend.Widget.Multiplayer` entry in FrontendWidgetMap. The
`Frontend_Widget_Multiplayer` tag is declared in `FrontendGameplayTags.cpp` and
`W_MultiplayerScreen` is added in this changeset, but no tag-to-widget mapping exists here. This
will cause `GetFrontendSoftWidgetClassByTag` to return null when looking up the multiplayer screen,
triggering an ensure failure in non-shipping builds and preventing the screen from loading.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m FrontendWidgetMap=(((TagName="Frontend.Widget.PressAnyKey"), "/GameMainMenu/BP/UI/Widgets/WBP_PressAnyKey.WBP_PressAnyKey_C"),((TagName="Frontend.Widget.MainMenu"), "/GameMainMenu/BP/UI/Widgets/WBP_MainMenu.WBP_MainMenu_C"),((TagName="Frontend.Widget.Confirm"), "/GameMainMenu/BP/UI/Widgets/WBP_ConfirmScreen.WBP_ConfirmScreen_C"),((TagName="Frontend.Widget.Options"), "/GameMainMenu/BP/UI/Widgets/Options/WBP_OptionScreen.WBP_OptionScreen_C"),((TagName="Frontend.Widget.KeyRemap"), "/GameMainMenu/BP/UI/Widgets/WBP_KeyRemapScreen.WBP_KeyRemapScreen_C"),((TagName="Frontend.Widget.Credits"), "/GameMainMenu/BP/UI/Widgets/WBP_CreditsScreen.WBP_CreditsScreen_C"))[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m FrontendWidgetMap=(((TagName="Frontend.Widget.PressAnyKey"), "/GameMainMenu/BP/UI/Widgets/WBP_PressAnyKey.WBP_PressAnyKey_C"),((TagName="Frontend.Widget.MainMenu"), "/GameMainMenu/BP/UI/Widgets/WBP_MainMenu.WBP_MainMenu_C"),((TagName="Frontend.Widget.Confirm"), "/GameMainMenu/BP/UI/Widgets/WBP_ConfirmScreen.WBP_ConfirmScreen_C"),((TagName="Frontend.Widget.Options"), "/GameMainMenu/BP/UI/Widgets/Options/WBP_OptionScreen.WBP_OptionScreen_C"),((TagName="Frontend.Widget.KeyRemap"), "/GameMainMenu/BP/UI/Widgets/WBP_KeyRemapScreen.WBP_KeyRemapScreen_C"),((TagName="Frontend.Widget.Credits"), "/GameMainMenu/BP/UI/Widgets/WBP_CreditsScreen.WBP_CreditsScreen_C"),((TagName="Frontend.Widget.Multiplayer"), "/GameMainMenu/BP/UI/Widgets/Multiplayer/WBP_MultiplayerScreen.WBP_MultiplayerScreen_C"))[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/FunctionLibrary/FrontendFunctionLibrary.cpp:24-26 ───[0m
[93m[maintainability · medium][0m Same ensureMsgf → FindRef fallthrough issue as above. After ensureMsgf
fires, FindRef returns a null TSoftObjectPtr for the missing key, which may surprise callers
expecting a valid texture reference.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m ensureMsgf(FrontendDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("Can not find Image under the tag %s"), *InImageTag.ToString());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Option A: Keep the hard check if this is truly an unrecoverable error[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m checkf(FrontendDeveloperSettings->OptionsScreenSoftImageMap.Contains(InImageTag), TEXT("Can not find Image under the tag %s"), *InImageTag.ToString());[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	return FrontendDeveloperSettings->OptionsScreenSoftImageMap.FindRef(InImageTag);[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Interface/FrontendLoadingScreenInterface.cpp:7-7 ───[0m
[1;91m[bug · critical][0m BlueprintNativeEvent functions require default `_Implementation` definitions in the
interface's .cpp file. Without them, the generated UHT code (Execute_*) will produce unresolved
external symbol linker errors when any object implementing this interface does not override these
functions in Blueprint or C++. The file currently contains only a placeholder comment with no actual
implementations.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m // Add default functionality here for any IFrontendLoadingScreenInterface functions that are not pure virtual.[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m void IFrontendLoadingScreenInterface::OnLoadingScreenActivated_Implementation()[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m {[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m [0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m void IFrontendLoadingScreenInterface::OnLoadingScreenDeactivated_Implementation()[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m {[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m }[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/GameMainMenu.cpp:16-16 ───[0m
[94m[style · low][0m Potential typo: "ST_UI_Common" appears to be misspelled — likely intended as
"ST_UI_Common" (for "Common" UI strings). The asset file on disk is also named with this spelling
(ST_UI_Common.uasset), so the reference is consistent. However, if this is a naming error, both the
asset and this code path should be corrected together to avoid a broken reference later.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		TEXT("/GameMainMenu/BP/StringTable/ST_UI_Common"),[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Credits/W_CreditsScreen.cpp:118-122 ───[0m
[91m[bug · high][0m Division by zero when `LineHeight` is 0. Although the default is 30.f, `EditAnywhere`
allows a designer to set it to 0 in the data asset. `FMath::CeilToInt(NaN)` (from `SpacerPx / 0.f`)
produces undefined behavior and a likely crash.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m if (SpacerPx > 0.f)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m if (SpacerPx > 0.f && CreditsData->LineHeight > KINDA_SMALL_NUMBER)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			const int32 SpacerCount = FMath::Max(0, FMath::CeilToInt(SpacerPx / CreditsData->LineHeight));[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			AddSpacerLines(SpacerCount);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Credits/W_CreditsScreen.cpp:138-138 ───[0m
[93m[bug · medium][0m `GetWorld()` is called without a null check. The scroll tick timer is normally
cleared in `NativeOnDeactivated`, but if the world is torn down externally before deactivation, this
will null-dereference and crash.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m const float DeltaTime = FMath::Max(GetWorld()->GetDeltaSeconds(), 0.001f);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m UWorld* World = GetWorld();[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	if (!World)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		return;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	}[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	const float DeltaTime = FMath::Max(World->GetDeltaSeconds(), 0.001f);[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Credits/W_CreditsScreen.cpp:138-138 ───[0m
[93m[bug · medium][0m `FMath::Max` clamps the *minimum* delta time, not the maximum. This is almost
certainly a logic error — the typical UE pattern is `FMath::Min(DeltaTime, MaxDelta)` to cap hitch
spikes. At high frame rates (>1000 FPS), scrolling speed becomes artificially faster than intended.
If the goal is to prevent zero-delta, a simple `FMath::Max(DeltaTime, KINDA_SMALL_NUMBER)` is
acceptable, but using `0.001f` (1ms) is high enough to observably distort scroll speed at common
high refresh rates (e.g., 240Hz ≈ 4ms delta). Consider using a smaller epsilon or simply removing
the clamp.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m const float DeltaTime = FMath::Max(GetWorld()->GetDeltaSeconds(), 0.001f);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m const float DeltaTime = GetWorld()->GetDeltaSeconds();[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/LoadingScreenPhaseMapping.cpp:18-18 ───[0m
[94m[maintainability · low][0m The string table path "/GameMainMenu/BP/StringTable/ST_GameMessages" is
hardcoded directly in the function body. If the string table asset is ever moved or renamed, this
reference will silently return an empty or invalid FText at runtime. Consider making this path a
named constant or a configurable property (e.g., a UPROPERTY on the class) so it can be updated in
one place.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	Fallback.DisplayText = FText::FromStringTable(TEXT("/GameMainMenu/BP/StringTable/ST_GameMessages"), TEXT("Loading"));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	Fallback.DisplayText = FText::FromStringTable(DefaultStringTablePath, TEXT("Loading"));[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/DataObjects/ListDataObject_Editable.cpp:29-29 ───[0m
[93m[bug · medium][0m If MaxCharCount is set to a negative value after initialization (via SetMaxCharCount
from Blueprint or C++), FString::Left(MaxCharCount) will return an empty string, silently erasing
all text. Meanwhile, ValidateText skips the length check when MaxCharCount <= 0, creating an
inconsistency where validation passes but the text is wiped. Consider guarding the truncation the
same way as ValidateText: only truncate when MaxCharCount > 0.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m const FString Truncated = Filtered.Left(MaxCharCount);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m const FString Truncated = (MaxCharCount > 0) ? Filtered.Left(MaxCharCount) : Filtered;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Subsystems/FrontendLoadingScreenSubsystem.cpp:78-80 ───[0m
[94m[maintainability · low][0m Empty destructor is dead code. UGameInstanceSubsystem is a UObject whose
cleanup is handled entirely in Deinitialize() (delegate removal and loading screen removal already
occur there). An empty destructor provides no functional value and may mislead future maintainers.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m UFrontendLoadingScreenSubsystem::~UFrontendLoadingScreenSubsystem()[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m {[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Remove the destructor declaration from the header and this definition entirely.[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_Scalar.cpp:59-73 ───[0m
[93m[maintainability · medium][0m OnMouseCaptureEnd() and OnControllerCaptureEnd() have identical
implementations. If this save logic needs to change in the future, both methods must be updated,
increasing the risk of divergence. Consider extracting a shared private helper method (e.g.,
TrySaveSettingsOnCaptureEnd()) used by both callbacks.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m void UW_ListEntry_Scalar::OnMouseCaptureEnd()[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m {[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	if (UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get())[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 		Settings->SaveSettings();[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	}[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	TrySaveSettingsOnCaptureEnd();[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m }[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m void UW_ListEntry_Scalar::OnControllerCaptureEnd()[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m {[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	TrySaveSettingsOnCaptureEnd();[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m [0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m void UW_ListEntry_Scalar::TrySaveSettingsOnCaptureEnd()[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m {[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	if (UFrontendGameUserSettings* Settings = UFrontendGameUserSettings::Get())[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		Settings->SaveSettings();[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m }[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:213-214 ───[0m
[93m[bug · medium][0m The default case returns nullptr without creating an Entry. While the sole call site
in InitFromDataAsset does null-check, this silent failure could cause crashes if future callers omit
the check. Consider logging a warning via ensure/ensureMsgf when an unrecognized EMenuEntryValueType
is encountered, making the failure explicit.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	default:[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		ensureMsgf(false, TEXT("Unhandled EMenuEntryValueType %d for entry %s"), static_cast<int32>(Data->ValueType), *Data->DataId.ToString());[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		break;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:149-149 ───[0m
[93m[bug · medium][0m The condition `Data->DefaultIntValue != 0` prevents setting the default quality level
to 0 (Low). If a designer configures DefaultIntValue=0 in the DataAsset, it is silently overridden
to 3 (Epic). Use a sentinel or separate flag to distinguish 'unset' from an intentional 0.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			const int32 DefaultVal = Data->DefaultIntValue != 0 ? Data->DefaultIntValue : 3;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			// Use -1 as sentinel for 'unset' (0 = Low is a valid quality level)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			const int32 DefaultVal = Data->DefaultIntValue >= 0 ? Data->DefaultIntValue : 3;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:71-97 ───[0m
[93m[maintainability · medium][0m The FullscreenMode and quality-level handling logic is duplicated in both
EMenuEntryValueType::String and EMenuEntryValueType::Integer branches. This means any change to
window mode options or quality levels must be made in two places. Consider extracting shared logic
into helper functions (e.g., `CreateFullscreenModeEntry`, `CreateQualityPresetEntry`) to reduce
duplication and prevent drift.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:164-167 ───[0m
[93m[bug · medium][0m The condition `if (Data->DefaultIntValue != 0 ||
!Data->AvailableIntegerOptions.IsEmpty())` will set the default to '0' when AvailableIntegerOptions
is non-empty even if DefaultIntValue is 0 and 0 is not a valid option in the map. This may lead to
displaying an invalid default selection. Consider checking whether DefaultIntValue exists as a key
in AvailableIntegerOptions before setting it as default.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			if (Data->DefaultIntValue != 0 || !Data->AvailableIntegerOptions.IsEmpty())[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			if (Data->AvailableIntegerOptions.Contains(Data->DefaultIntValue))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 					TypedEntry->SetDefaultValueFromString(LexToString(Data->DefaultIntValue));[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:79-79 ───[0m
[94m[style · low][0m The comment contains 'ponytail' which appears to be a developer note rather than
meaningful documentation. Consider removing or replacing it with a proper explanation.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:73-73 ───[0m
[94m[style · low][0m Magic strings 'FullscreenMode', 'Quality', and 'Scalability' are used directly in
comparisons. Using named constants (e.g., in a namespace or as static FNames) would reduce the risk
of typos and make refactoring easier.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Subsystems/FrontendLoadingScreenSubsystem.cpp:90-90 ───[0m
[94m[style · low][0m The name "StartUp" uses nonstandard capitalization. The conventional spelling in
Unreal Engine is "Startup" (lowercase 'u'). Consider renaming to HoldLoadingScreenStartupTime for
consistency with UE naming conventions.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp:126-127 ───[0m
[93m[maintainability · medium][0m These tab IDs are duplicated from the file-static constants
`TabId_CreateGame`/`TabId_JoinGame` defined in `MultiplayerDataRegistry.cpp`. When config-driven
tabs are used, the registry may expose different tab IDs, causing `SelectTabByID` in
`NativeOnActivated()` and the action binding logic here to fail silently. Consider exposing these as
shared constants (e.g., via a common header or static members on `UMultiplayerDataRegistry`) to keep
them in sync.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m static const FName CreateGameTabId(TEXT("CreateGame"));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	static const FName JoinGameTabId(TEXT("JoinGame"));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Consider defining these in a shared location, e.g. as static members of UMultiplayerDataRegistry:[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	// static const FName CreateGameTabId;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	// static const FName JoinGameTabId;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp:73-73 ───[0m
[94m[bug · low][0m `SelectTabByID(FName(TEXT("CreateGame")))` has no fallback. If the registry provides a
config where the "CreateGame" tab does not exist (or uses a different ID), no tab will be selected,
leaving the screen content empty. Consider selecting the first available tab as a fallback.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m TabListWidget_MultiplayerTabs->SelectTabByID(FName(TEXT("CreateGame")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	// Fallback: if CreateGame tab wasn't found, select the first available tab[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	if (TabListWidget_MultiplayerTabs->GetSelectedTabId() == NAME_None)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		const TArray<UListDataObject_Collection*>& Tabs = Registry->GetRegisteredTabs();[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		if (Tabs.Num() > 0 && Tabs[0])[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			TabListWidget_MultiplayerTabs->SelectTabByID(Tabs[0]->GetDataId());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		}[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/W_OptionDetailView.cpp:32-32 ───[0m
[94m[style · low][0m Inconsistent indentation: this line has an extra level of indentation compared to the
surrounding code (lines 30, 33 and the rest of the function body). Remove the extra tab to maintain
consistent formatting.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	Ctb_DynamicDetails->SetText(InDataObject->GetDynamicDetailsRichText());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/W_OptionDetailView.cpp:37-37 ───[0m
[94m[style · low][0m Missing space before the `?` operator in the ternary expression. While this compiles
correctly, the standard convention is to add a space before `?` for readability and consistency with
the spacing around `:`.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m Ctb_DisabledReason->SetText(InDataObject->IsDataCurrentlyEditable()? FText::GetEmpty() : InDataObject->GetDisableRichText());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m Ctb_DisabledReason->SetText(InDataObject->IsDataCurrentlyEditable() ? FText::GetEmpty() : InDataObject->GetDisableRichText());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/W_OptionScreen.cpp:313-325 ───[0m
[1;91m[bug · critical][0m GetOrCreateDataRegistry() now returns nullptr on failure (instead of crashing via
checkf), but both call sites at line 73 (NativeOnActivated) and line 226 (OnOptionsTabSelected)
dereference the result without a null check. In shipping builds where ensureMsgf does not halt
execution, this will cause a null pointer crash.

Suggestion: either keep checkf (and don't return nullptr), or add null guards at each call site.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m UOptionsDataRegistry* UW_OptionScreen::GetOrCreateDataRegistry()[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m {[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	if (!CreatedDataRegistry)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		CreatedDataRegistry = NewObject<UOptionsDataRegistry>();[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		CreatedDataRegistry->SetOptionsConfig(OptionsConfig);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		CreatedDataRegistry->InitOptionsDataRegistry(GetOwningLocalPlayer());[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	if (!ensureMsgf(CreatedDataRegistry, TEXT("Data Registry for options screen is not valid"))) { return nullptr; }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	ensureMsgf(CreatedDataRegistry, TEXT("Data Registry for options screen is not valid"));[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	return CreatedDataRegistry;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m }[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:88-96 ───[0m
[93m[bug · medium][0m Fragile raw pointer capture in lambda: the TArray<UListDataObject_StringBool*>
BoolDeps is captured by value into a TFunction<bool()> stored inside
FOptionsDataEditConditionDescriptor. If the registry is ever reinitialized (InitRegistry clears
RegisteredTabs) while old entries with these lambdas are still referenced, IsEditConditionMet()
would dereference garbage-collected objects. Consider using TWeakObjectPtr inside the captures and
checking IsValid() before each dereference.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			Condition.SetEditConditionFunction([0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				[BoolDeps]() -> bool[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				[WeakBoolDeps]() -> bool[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				{[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 					for (const UListDataObject_StringBool* Dep : BoolDeps)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 					for (const TWeakObjectPtr<UListDataObject_StringBool>& Dep : WeakBoolDeps)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 					{[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 						if (Dep->GetCurrentStringValue() != TEXT("true")) return false;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 						if (!Dep.IsValid() || Dep->GetCurrentStringValue() != TEXT("true")) return false;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 					}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 					return true;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				});[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:145-146 ───[0m
[93m[bug · medium][0m Same fragile raw pointer capture pattern: the local HasPassword pointer is captured
by value in a TFunction<bool()> stored in an EditConditionDescriptor. If InitRegistry() is
re-invoked before stale entries are collected, HasPassword may dangle. Consider wrapping in
TWeakObjectPtr and checking IsValid() before dereferencing.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		PasswordCondition.SetEditConditionFunction([0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			[HasPassword]() -> bool { return HasPassword->GetCurrentStringValue() == TEXT("true"); });[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			[WeakHasPassword]() -> bool { return WeakHasPassword.IsValid() && WeakHasPassword->GetCurrentStringValue() == TEXT("true"); });[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Components/FrontendCommonTabListWidgetBase.h:25-26 ───[0m
[93m[maintainability · medium][0m Documentation mismatch: the comment states this is a no-op if a tab with
the same ID already exists, but the implementation unconditionally calls RegisterTab and
SetButtonText regardless. Either add a duplicate check (e.g., using GetTabButtonBaseByID before
registering) or update the comment to accurately describe the actual behavior.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m /** Register a tab dynamically. No-op if a tab with the same ID already exists. */[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	void RequestRegisterTab(const FName& InTabId,const FText& InTabDisplayName);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m /** Register a tab dynamically. */[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	void RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName);[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Components/FrontendCommonTabListWidgetBase.h:26-26 ───[0m
[94m[style · low][0m Missing space after comma in parameter list.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m void RequestRegisterTab(const FName& InTabId,const FText& InTabDisplayName);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m void RequestRegisterTab(const FName& InTabId, const FText& InTabDisplayName);[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Components/FrontendCommonTabListWidgetBase.h:26-26 ───[0m
[94m[style · low][0m Missing spaces after commas in function call arguments.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m void RequestRegisterTab(const FName& InTabId,const FText& InTabDisplayName);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	RegisterTab(InTabId, TabButtonEntryWidgetClass, nullptr);[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/FrontendSettings/FrontendLoadingScreenSettings.h:20-20 ───[0m
[93m[maintainability · medium][0m The method name `GetSoftLoadScreenWidgetClassCheck` appears to have an
unintended "Check" suffix. This is used as a plain getter in
`FrontendLoadingScreenSubsystem.cpp:216`. In UE conventions, methods with "Check" in their name
typically perform fatal assertions (e.g., `check()`), but this method returns a value. Consider
renaming to `GetSoftLoadScreenWidgetClass` or a name that better reflects its purpose.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m TSubclassOf<UUserWidget> GetSoftLoadScreenWidgetClassCheck() const;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m TSubclassOf<UUserWidget> GetSoftLoadScreenWidgetClass() const;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/FrontendSettings/FrontendLoadingScreenSettings.h:7-8 ───[0m
[94m[maintainability · low][0m Missing forward declaration for `UUserWidget`. The method returns
`TSubclassOf<UUserWidget>` but there is no `class UUserWidget;` forward declaration in this header.
While this may compile due to transitive includes, it makes the header fragile and dependent on
include order. Add a forward declaration for clarity and robustness.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m #include "Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h"[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m [0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m class UUserWidget;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m #include "FrontendLoadingScreenSettings.generated.h"[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Credits/CreditsDataAsset.h:25-26 ───[0m
[91m[bug · high][0m LineHeight is used as a divisor (in W_CreditsScreen.cpp). A value of 0 will cause a
division-by-zero crash, and a negative value will produce negative spacer counts. Consider adding
meta=(ClampMin="1.0") to prevent invalid values in the editor.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="1.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float LineHeight = 30.f;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Credits/CreditsDataAsset.h:19-20 ───[0m
[93m[bug · medium][0m ScrollSpeed could be set to a negative value by designers, causing backward scrolling
and potentially preventing the scroll-finished condition from ever being reached (infinite loop).
Consider adding meta=(ClampMin="0.0").

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float ScrollSpeed = 50.f;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Credits/CreditsDataAsset.h:22-23 ───[0m
[94m[maintainability · low][0m StartDelay can be set to a negative value, which may cause undefined
behavior when passed to the UE timer system. Consider adding meta=(ClampMin="0.0") for defensive
validation.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float StartDelay = 1.f;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Credits/CreditsDataAsset.h:28-32 ───[0m
[94m[maintainability · low][0m EntrySpacing and LogoSpacing are used as padding values; negative values
would cause overlapping UI elements. Consider adding meta=(ClampMin="0.0") to both.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float EntrySpacing = 0.f;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits")[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Credits", meta=(ClampMin="0.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float LogoSpacing = 120.f;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Subsystems/FrontendLoadingScreenSubsystem.h:32-32 ───[0m
[94m[maintainability · low][0m Missing `override` specifier on `GetTickableGameObjectWorld()`. All virtual
overrides from `FTickableGameObject` should be explicitly marked with `override` to improve code
clarity and enable compiler warnings about signature mismatches.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	virtual UWorld* GetTickableGameObjectWorld() const;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	virtual UWorld* GetTickableGameObjectWorld() const override;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuConfigData.h:10-11 ───[0m
[94m[maintainability · low][0m Unused forward declaration. `FOptionsDataInteractionHelper` is not
referenced anywhere in this header or its implementation file. Remove it to keep the code clean.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m class UListDataObject_Base;[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m class FOptionsDataInteractionHelper;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:103-105 ───[0m
[91m[bug · high][0m `sscanf_s` is a Microsoft-specific (MSVC) function that does not exist on Linux, macOS,
or console platforms. Combined with `TCHAR_TO_ANSI` (which is lossy for wide-character builds), this
will cause a compile error on non-Windows targets. Use UE's own parsing utilities instead, e.g.
`FParse::Value` or a combination of `FString::ParseIntoArray` / `FString::Split` to safely extract X
and Y integers from the `(X=%d,Y=%d)` format.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	BindingMap.Add("Resolution", MakeShared<FOptionsDataInteractionHelper>([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		[]() { const FIntPoint R = FSettings::Get()->GetScreenResolution(); return FString::Printf(TEXT("(X=%d,Y=%d)"), R.X, R.Y); },[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 		[](const FString& V) { FIntPoint R(0, 0); sscanf_s(TCHAR_TO_ANSI(*V), "(X=%d,Y=%d)", &R.X, &R.Y); if (R.X > 0 && R.Y > 0) FSettings::Get()->SetScreenResolution(R); }));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		[](const FString& V) { FIntPoint R(0, 0); FString Str = V; Str.RemoveFromStart(TEXT("(X=")); int32 CommaIdx; if (Str.FindChar(',', CommaIdx)) { R.X = FCString::Atoi(*Str.Left(CommaIdx)); Str.RightChopInline(CommaIdx + 3); Str.RemoveFromEnd(TEXT(")")); R.Y = FCString::Atoi(*Str); } if (R.X > 0 && R.Y > 0) FSettings::Get()->SetScreenResolution(R); }));[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:868-873 ───[0m
[93m[maintainability · medium][0m Hardcoded string IDs (`"Tab-Video"`, `"WindowMode"`, `"Resolution"`,
`"Entry-VSync"`, `"OverallQuality"`) are used to wire EditConditions. If the DataAsset uses
different IDs for these entries, the EditConditions will silently fail to apply — e.g. Resolution
won't be disabled in borderless mode, and VSync won't be restricted to fullscreen. Consider defining
these IDs as named constants (e.g. in `MenuConfigTypes.h`) shared between the DataAsset and this
wiring code, or derive the conditions from entry metadata (e.g. a new `EMenuEntryEditCondition` enum
on `UMenuEntryConfig`).



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:278-278 ───[0m
[93m[maintainability · medium][0m The `checkf` → `ensureMsgf` change avoids a crash when a tab ID is not
found, but returns an empty array. All callers (e.g. `OnOptionsTabSelected` in `W_OptionScreen.cpp`)
iterate over the result with a for-each loop — an empty result silently produces a blank options
panel with no user-facing error. Consider logging a warning in the caller when the returned array is
empty, or adding a dedicated `IsValid` check before populating the UI.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/ListEntries/W_ListEntry_Editable.h:31-32 ───[0m
[93m[bug · medium][0m In `TryCommitValidation()`, after syncing the widget text to the filtered text on
lines 102-105, the validation on line 115 still passes the old `WidgetText` (captured before the
sync) to `ValidateText()`. This means if the user typed invalid characters that were silently
filtered out, they will see an error dialog about characters that are no longer visible in the input
field.

Fix: validate the filtered text instead — change `ValidateText(WidgetText, Reason)` to
`ValidateText(Filtered, Reason)`, or re-read the widget text after syncing.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h:0-0 ───[0m
[93m[maintainability · medium][0m The hardcoded string table path and key should use the centralized macros
defined in `LocalizationKeys.h` for consistency across the project. The project already defines
`ST_GAME_MESSAGES` and `TXT_GAME_LOADING()` for this exact purpose. Using raw strings here creates a
maintenance risk — if the string table path or key is renamed, this call site will silently break
while other usages get updated via the macro.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m #include "Internationalization/StringTableRegistry.h"[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m #include "LocalizationKeys.h"[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuEntryConfig.h:21-23 ───[0m
[94m[maintainability · low][0m All UPROPERTY fields lack Category specifiers. Adding categories (e.g.,
Category="Common", Category="Scalar", Category="Editable") would improve editor organization and
make it easier for designers to navigate this DataAsset when configuring menu entries.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	// ---- Common (all ValueTypes) ----[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Common")[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	FName DataId;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuEntryConfig.h:50-52 ───[0m
[94m[maintainability · low][0m The Scalar section's default value is named DefaultValue (FString), while
the String section uses DefaultStringValue (also FString). Both serve the same semantic purpose but
have different names. Consider renaming to DefaultScalarValue for clarity and consistency,
especially since both are FString types and could confuse maintainers.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	// ---- Scalar fields ----[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	FString DefaultValue;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	FString DefaultScalarValue;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h:18-19 ───[0m
[93m[maintainability · medium][0m Use the centralized `ST_GAME_MESSAGES` and `TXT_GAME_LOADING()` macros
instead of hardcoded strings, consistent with the rest of the codebase.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m UPROPERTY(EditAnywhere, BlueprintReadOnly)[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	FText DisplayText;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	Fallback.DisplayText = TXT_GAME_LOADING();[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/DataObjects/LoadingScreenPhaseMapping.h:0-0 ───[0m
[94m[bug · low][0m `FindDisplayInfo` always returns a valid-looking `FLoadingPhaseDisplayInfo` — even when
no matching entry exists. It sets both `PhaseTag` and `DisplayText` on the fallback, so callers
cannot distinguish between a real match and the fallback. Consider adding a return parameter (e.g.,
`bool& bOutFound`), or returning a pointer/nullptr, or providing a `bool IsValid()` helper on the
struct so callers can detect missing configuration.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m FLoadingPhaseDisplayInfo ULoadingScreenPhaseMapping::FindDisplayInfo(FGameplayTag PhaseTag) const[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m bool ULoadingScreenPhaseMapping::FindDisplayInfo(FGameplayTag PhaseTag, FLoadingPhaseDisplayInfo& OutInfo) const[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/MultiplayerDataRegistry.h:46-46 ───[0m
[93m[bug · medium][0m FindChildListDataRecursively has no cycle detection (no visited-set or depth limit).
A circular reference in the data hierarchy — whether from a DataAsset authoring error or the
hardcoded fallback — would cause infinite recursion and stack overflow. Consider adding a TSet<const
UListDataObject_Base*> visited guard, or at minimum a recursion depth limit with an ensure.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m void FindChildListDataRecursively(const UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData) const;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m void FindChildListDataRecursively(const UListDataObject_Base* InParentData, TArray<UListDataObject_Base*>& OutFoundChildListData, TSet<const UListDataObject_Base*>& Visited) const;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/OptionsDataInteractionHelper.h:20-21 ───[0m
[94m[maintainability · low][0m The `explicit` keyword was removed from the constructor. While less critical
for a two-parameter constructor, `explicit` still prevents unintended implicit conversion from
brace-enclosed initializer lists (e.g., `FOptionsDataInteractionHelper h = {g, s};` or passing
`{getter, setter}` to a function expecting this type). Since the original code intentionally used
`explicit`, consider restoring it to maintain consistency and defensive coding practice.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	FOptionsDataInteractionHelper(FGetter InGetter, FSetter InSetter)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	explicit FOptionsDataInteractionHelper(FGetter InGetter, FSetter InSetter)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		: Getter(MoveTemp(InGetter)), Setter(MoveTemp(InSetter)) {}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/MultiplayerDataRegistry.h:30-31 ───[0m
[93m[maintainability · medium][0m The comment requires SetConfig before InitRegistry, but there's no
programmatic enforcement. If a future caller reverses this order, the registry silently uses
fallback data. Consider adding an ensure/check in InitRegistry to warn when Config is unexpectedly
null, or fold Config into InitRegistry as a parameter to make the dependency explicit.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	/** Assign a DataAsset config. Must be called before InitRegistry. */[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	void SetConfig(UMenuConfigData* InConfig) { Config = InConfig; }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	void SetConfig(UMenuConfigData* InConfig) { Config = InConfig; bConfigWasSet = true; }[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m [0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	bool bConfigWasSet = false;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/OptionsDataRegistry.h:63-67 ───[0m
[1;91m[bug · critical][0m `CreateEntryFromConfig` is called in `InitFromDataAsset` (line 797 of .cpp) but is
never declared in this header. This will cause a compilation error. It needs to be added to the
private section alongside the other `Create*` factory methods.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	void ApplyTabEditConditions([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		UListDataObject_Collection* Tab,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		UMenuTabConfig* TabData,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		const FOptionsDataEditConditionDescriptor& PackagedBuildOnlyCondition,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		const TMap<FName, UListDataObject_Base*>& EntryLookup);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m [0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	/** Creates an appropriate UListDataObject from a UMenuEntryConfig. Returns nullptr for unsupported types. */[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UListDataObject_Base* CreateEntryFromConfig(const UMenuEntryConfig* EntryData) const;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/OptionsDataRegistry.h:70-70 ───[0m
[93m[maintainability · medium][0m `ResolveBinding` returns nullptr when a `BindingKey` is not found in the
`BindingMap`, and all factory methods (`CreateBoolEntry`, `CreateScalarEntry`, `CreateStringEntry`,
`CreateQualityPresetEntry`) silently skip wiring the getter/setter. This produces option entries
that appear in the UI but have no effect. A warning log would help catch typos or missing bindings
during development. Consider adding `UE_LOG` inside `ResolveBinding` (or in each factory method
after a failed lookup) to surface these configuration errors.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	TSharedPtr<FOptionsDataInteractionHelper> ResolveBinding(const FName& BindingKey) const;[0m[0m

