[ocr] Summary: 19 file(s) reviewed, 23 comment(s), ~1639150 token(s) used (input: ~1544802, output: ~94348), cache(read: ~1247232, write: ~0), 3m52s elapsed

[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/W_OptionDetailView.cpp:32-32 ───[0m
[94m[style · low][0m Inconsistent indentation: this line uses two levels of indentation (\t\t) while the
surrounding code in this function — including the section header comments and all other Ctb_* calls
(lines 16, 28, 37) — uses a single level of indentation (\t). Consider removing the extra tab for
consistency.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		Ctb_DynamicDetails->SetText(InDataObject->GetDynamicDetailsRichText());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_Base.cpp:42-43 ───[0m
[93m[bug · medium][0m If GetDisableRichReason() returns an empty string while CachedDisableRichReason
already contains text from a previous iteration, a trailing "\n" separator will be appended with no
content following it. This can result in unintended blank lines in the disable rich text displayed
in the UI. Consider guarding the separator append with a check that the incoming reason is also
non-empty.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m if (!CachedDisableRichReason.IsEmpty()) CachedDisableRichReason.Append(TEXT("\n"));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m if (!CachedDisableRichReason.IsEmpty() && !EditConditionDesc.GetDisableRichReason().IsEmpty()) CachedDisableRichReason.Append(TEXT("\n"));[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		CachedDisableRichReason.Append(EditConditionDesc.GetDisableRichReason());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuConfigData.h:10-11 ───[0m
[94m[maintainability · low][0m Unused forward declaration: `FOptionsDataInteractionHelper` is declared but
never referenced anywhere in this header. Remove it to avoid dead code and confusion.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m class UListDataObject_Base;[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m class FOptionsDataInteractionHelper;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuConfigData.h:7-7 ───[0m
[93m[maintainability · medium][0m `UMenuEntryConfig` is used directly in `CreateEntryFromConfig` but relies
on a transitive include chain (MenuTabConfig.h → MenuCategoryConfig.h → MenuEntryConfig.h). This is
fragile — if the include chain changes, this header will break. Add an explicit `#include
"Widgets/MenuConfig/MenuEntryConfig.h"`.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m #include "Widgets/MenuConfig/MenuTabConfig.h"[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m #include "Widgets/MenuConfig/MenuEntryConfig.h"[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuConfigData.h:10-11 ───[0m
[94m[maintainability · low][0m Inconsistent forward-declaration style: `UListDataObject_Base` is declared
at the top, but `UListDataObject_Collection` is declared inline as `class
UListDataObject_Collection*` in the function signature. Move `UListDataObject_Collection` to the
forward-declaration block for consistency.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m class UListDataObject_Base;[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m class FOptionsDataInteractionHelper;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m class UListDataObject_Collection;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuConfigData.h:35-35 ───[0m
[94m[maintainability · low][0m Related to the above: after moving the forward declaration, the inline
`class` keyword in the function signature becomes redundant. Remove it for consistency with the
`CreateEntryFromConfig` style.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m GAMEMAINMENU_API class UListDataObject_Collection* CreateCollectionFromConfig(const FName& Id, const FText& DisplayName);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m GAMEMAINMENU_API UListDataObject_Collection* CreateCollectionFromConfig(const FName& Id, const FText& DisplayName);[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp:126-127 ───[0m
[93m[maintainability · medium][0m Hardcoded tab IDs 'CreateGame'/'JoinGame' are duplicated across this file
and MultiplayerDataRegistry.cpp. Since UMenuTabConfig::TabId is designer-configurable via
DataAssets, using a DataAsset with different tab IDs (e.g., 'HostGame'/'FindGame') will cause the
binding toggle logic in OnMultiplayerTabSelected to silently never match, leaving both action
bindings active. Similarly, NativeOnActivated:73 hardcodes 'CreateGame' for default tab selection,
which would fail for config-driven tab sets.

Consider: (1) deriving tab IDs dynamically from the registry's registered tabs (e.g., using the
first/second tab's GetDataId()), or (2) exposing the constants from MultiplayerDataRegistry (where
TabId_CreateGame/TabId_JoinGame are defined) so both files share a single source of truth.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m static const FName CreateGameTabId(TEXT("CreateGame"));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	static const FName JoinGameTabId(TEXT("JoinGame"));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Use the registry's tab IDs dynamically, e.g.:[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	// const TArray<UListDataObject_Collection*>& Tabs = GetOrCreateDataRegistry()->GetRegisteredTabs();[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	// const FName CreateGameTabId = Tabs.IsValidIndex(0) ? Tabs[0]->GetDataId() : FName(TEXT("CreateGame"));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	// const FName JoinGameTabId  = Tabs.IsValidIndex(1) ? Tabs[1]->GetDataId() : FName(TEXT("JoinGame"));[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp:20-40 ───[0m
[94m[bug · low][0m Both StartGame and JoinGame action bindings are registered and immediately active after
NativeOnInitialized. They remain dual-active until NativeOnActivated calls SelectTabByID →
OnMultiplayerTabSelected, which removes the mismatched binding. During this window, triggering the
JoinGame action while on the (default) CreateGame tab would invoke OnJoinGameActionTriggered
unexpectedly.

Consider removing the non-default binding immediately after registration in NativeOnInitialized, or
deferring registration to NativeOnActivated where the default tab is known.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m if (!StartGameAction.IsNull())[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		StartGameActionHandle = RegisterUIActionBinding([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			FBindUIActionArgs([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				StartGameAction,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				true,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				FSimpleDelegate::CreateUObject(this, &ThisClass::OnStartGameActionTriggered)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	if (!JoinGameAction.IsNull())[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		JoinGameActionHandle = RegisterUIActionBinding([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			FBindUIActionArgs([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				JoinGameAction,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				true,[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				FSimpleDelegate::CreateUObject(this, &ThisClass::OnJoinGameActionTriggered)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		// Remove immediately; OnMultiplayerTabSelected will re-add when JoinGame tab is selected[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		RemoveActionBinding(JoinGameActionHandle);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:149-149 ───[0m
[91m[bug · high][0m Logic flaw: `Data->DefaultIntValue != 0` treats 0 (which corresponds to the "Low"
quality level) as a sentinel for "not set" and forces the default to 3 (Epic). If a designer
intentionally sets `DefaultIntValue = 0` in the DataAsset, the user will silently get "Epic" quality
instead of "Low". The `DefaultIntValue` property defaults to 0 in the header, so there's no way to
distinguish "explicitly set to 0" from "not set".

Compare with the String quality branch which correctly uses `DefaultStringValue.IsEmpty()` as its
sentinel.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m const int32 DefaultVal = Data->DefaultIntValue != 0 ? Data->DefaultIntValue : 3;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Use the value directly — the DataAsset default (0 = Low) is a valid quality level.[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // If a sentinel is needed to indicate "not set", consider adding a separate flag[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // (e.g., bHasExplicitDefault) or defaulting to 3 in the DataAsset property itself.[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m const int32 DefaultVal = Data->DefaultIntValue;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:93-97 ───[0m
[93m[bug · medium][0m Hardcoded English strings for quality level names and window mode names will not be
localized. The codebase already defines localized macros in LocalizationKeys.h:
- `TXT_VAL_QUALITY_LOW()`, `TXT_VAL_QUALITY_MEDIUM()`, `TXT_VAL_QUALITY_HIGH()`,
`TXT_VAL_QUALITY_EPIC()`, `TXT_VAL_QUALITY_CINEMATIC()`
- `TXT_VAL_WINDOW_FULLSCREEN()`, `TXT_VAL_WINDOW_BORDERLESS()`, `TXT_VAL_WINDOW_WINDOWED()`

Using `TEXT("Low")` etc. bypasses the string table and will display as untranslated English text
regardless of the user's selected language.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m TypedEntry->AddIntegerOption(0, FText::FromString(TEXT("Low")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(1, FText::FromString(TEXT("Medium")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(2, FText::FromString(TEXT("High")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(3, FText::FromString(TEXT("Epic")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m TypedEntry->AddIntegerOption(0, TXT_VAL_QUALITY_LOW());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(1, TXT_VAL_QUALITY_MEDIUM());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(2, TXT_VAL_QUALITY_HIGH());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(3, TXT_VAL_QUALITY_EPIC());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(4, TXT_VAL_QUALITY_CINEMATIC());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:80-82 ───[0m
[93m[bug · medium][0m Hardcoded English strings for window mode options will not be localized. Use the
existing localized macros: `TXT_VAL_WINDOW_FULLSCREEN()`, `TXT_VAL_WINDOW_BORDERLESS()`,
`TXT_VAL_WINDOW_WINDOWED()`.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m TypedEntry->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m TypedEntry->AddEnumOption(EWindowMode::Fullscreen, TXT_VAL_WINDOW_FULLSCREEN());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddEnumOption(EWindowMode::WindowedFullscreen, TXT_VAL_WINDOW_BORDERLESS());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddEnumOption(EWindowMode::Windowed, TXT_VAL_WINDOW_WINDOWED());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:71-85 ───[0m
[93m[maintainability · medium][0m The FullscreenMode handling is duplicated identically in both the String
and Integer branches. If either the window mode options or default value need to change, both
branches must be updated separately, creating a maintenance risk. Consider extracting this into a
shared helper or handling `BindingKey == "FullscreenMode"` in a single early-branch before the
ValueType switch.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:111-114 ───[0m
[94m[maintainability · low][0m Iterating over `Data->AvailableOptions` (TMap) and
`Data->AvailableIntegerOptions` (TMap) does not guarantee a stable iteration order — TMap uses
hash-based ordering. This may cause menu options to appear in different sequences across game
sessions or platforms. If consistent ordering is expected, consider using a `TArray` of pairs or
sorting the keys before iteration.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:70-96 ───[0m
[93m[bug · medium][0m The lambda captures raw UObject pointers (UListDataObject_StringBool*) by value in
the TArray. These pointers are not protected by UE's garbage collection. If any of the dependency
objects are garbage collected before this condition descriptor is destroyed or evaluated, the lambda
will dereference dangling pointers, causing undefined behavior or crashes.

Consider using TWeakObjectPtr<UListDataObject_StringBool> in the capture array and validating with
IsValid() inside the lambda, or use FGCObjectScopeGuard to protect the references.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m TArray<UListDataObject_StringBool*> BoolDeps;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m TArray<TWeakObjectPtr<UListDataObject_StringBool>> BoolDeps;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m for (const FName& DepId : EntryData->EditDependencyIds)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m {[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	UListDataObject_Base* const* DepPtr = EntryLookup.Find(DepId);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	if (DepPtr && *DepPtr)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		(*EntryPtr)->AddEditDependencyData(*DepPtr);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		if (UListDataObject_StringBool* BoolDep = Cast<UListDataObject_StringBool>(*DepPtr))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		{[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			BoolDeps.Add(BoolDep);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m }[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m if (BoolDeps.IsEmpty()) continue;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m FOptionsDataEditConditionDescriptor Condition;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m Condition.SetEditConditionFunction([0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	[BoolDeps]() -> bool[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	{[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 		for (const UListDataObject_StringBool* Dep : BoolDeps)[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 		for (const TWeakObjectPtr<UListDataObject_StringBool>& Dep : BoolDeps)[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			if (!Dep.IsValid()) return false;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			if (Dep->GetCurrentStringValue() != TEXT("true")) return false;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		}[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 		return true;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	});[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:93-97 ───[0m
[93m[bug · medium][0m The same localization issue exists here in the String branch's quality preset —
hardcoded `TEXT("Low")`, `TEXT("Medium")`, etc. should use the localized macros from
LocalizationKeys.h: `TXT_VAL_QUALITY_LOW()`, `TXT_VAL_QUALITY_MEDIUM()`, `TXT_VAL_QUALITY_HIGH()`,
`TXT_VAL_QUALITY_EPIC()`, `TXT_VAL_QUALITY_CINEMATIC()`.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			TypedEntry->AddIntegerOption(0, FText::FromString(TEXT("Low")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(1, FText::FromString(TEXT("Medium")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(2, FText::FromString(TEXT("High")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(3, FText::FromString(TEXT("Epic")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddIntegerOption(4, FText::FromString(TEXT("Cinematic")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			TypedEntry->AddIntegerOption(0, TXT_VAL_QUALITY_LOW());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(1, TXT_VAL_QUALITY_MEDIUM());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(2, TXT_VAL_QUALITY_HIGH());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(3, TXT_VAL_QUALITY_EPIC());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddIntegerOption(4, TXT_VAL_QUALITY_CINEMATIC());[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:126-135 ───[0m
[93m[bug · medium][0m Hardcoded English strings for window mode options will not be localized. Use the
existing localized macros: `TXT_VAL_WINDOW_FULLSCREEN()`, `TXT_VAL_WINDOW_BORDERLESS()`,
`TXT_VAL_WINDOW_WINDOWED()`.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 			UListDataObject_StringEnum* TypedEntry = NewObject<UListDataObject_StringEnum>();[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				TypedEntry->SetDataId(Data->DataId);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				TypedEntry->SetDataDisplayName(Data->DisplayName);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				TypedEntry->SetDescriptionRichText(Data->Description);[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddEnumOption(EWindowMode::Fullscreen, FText::FromString(TEXT("Fullscreen")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddEnumOption(EWindowMode::WindowedFullscreen, FText::FromString(TEXT("Windowed Fullscreen")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 				TypedEntry->AddEnumOption(EWindowMode::Windowed, FText::FromString(TEXT("Windowed")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddEnumOption(EWindowMode::Fullscreen, TXT_VAL_WINDOW_FULLSCREEN());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddEnumOption(EWindowMode::WindowedFullscreen, TXT_VAL_WINDOW_BORDERLESS());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				TypedEntry->AddEnumOption(EWindowMode::Windowed, TXT_VAL_WINDOW_WINDOWED());[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				TypedEntry->SetDefaultValueFromEnumOption(EWindowMode::WindowedFullscreen);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				TypedEntry->SetShouldApplyChangeImmediately(Data->bApplyImmediately);[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 				Entry = TypedEntry;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:378-379 ───[0m
[91m[bug · high][0m Direct assignment of ResolveBinding result without null check. If the key
"CurrentCulture" is ever missing from BindingMap, Language->SetDataDynamicGetter/Setter will be
passed a null TSharedPtr. Unlike the factory methods (CreateBoolEntry, CreateScalarEntry, etc.)
which guard with `if (const auto Helper = ResolveBinding(...))`, and the data-asset path (lines
806-813) which also checks for null, these three special-cased entries (CurrentCulture,
FullscreenMode, Resolution) bypass the check. Consider adding a null guard for consistency and
safety, or at minimum add an `ensure`/`check` to catch missing keys during development.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m Language->SetDataDynamicGetter(ResolveBinding(FName("CurrentCulture")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			Language->SetDataDynamicSetter(ResolveBinding(FName("CurrentCulture")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m if (const auto Helper = ResolveBinding(FName("CurrentCulture")))[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				Language->SetDataDynamicGetter(Helper);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				Language->SetDataDynamicSetter(Helper);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:513-514 ───[0m
[91m[bug · high][0m Same pattern as the CurrentCulture entry — ResolveBinding result is assigned directly
without a null check. If "FullscreenMode" is missing from the BindingMap, the getter/setter will be
null, potentially causing a crash when the option is accessed.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m WindowMode->SetDataDynamicGetter(ResolveBinding(FName("FullscreenMode")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			WindowMode->SetDataDynamicSetter(ResolveBinding(FName("FullscreenMode")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m if (const auto Helper = ResolveBinding(FName("FullscreenMode")))[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				WindowMode->SetDataDynamicGetter(Helper);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				WindowMode->SetDataDynamicSetter(Helper);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:528-529 ───[0m
[91m[bug · high][0m Same pattern — ResolveBinding result for "Resolution" is assigned directly without a
null check.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m Resolution->SetDataDynamicGetter(ResolveBinding(FName("Resolution")));[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 			Resolution->SetDataDynamicSetter(ResolveBinding(FName("Resolution")));[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m if (const auto Helper = ResolveBinding(FName("Resolution")))[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			{[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				Resolution->SetDataDynamicGetter(Helper);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 				Resolution->SetDataDynamicSetter(Helper);[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 			}[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:164-168 ───[0m
[93m[maintainability · medium][0m When ResolveBinding returns null (e.g., due to a misspelled or missing
binding key in InitBindingMap), the factory methods silently skip setting the getter/setter. The
option will be non-functional with no diagnostic. Consider logging a warning (e.g., `UE_LOG(LogTemp,
Warning, TEXT("OptionsDataRegistry: Binding key '%s' not found"), *BindingKey.ToString())`) to help
catch configuration errors during development.



[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Options/OptionsDataRegistry.h:70-70 ───[0m
[93m[maintainability · medium][0m The factory methods (CreateBoolEntry, CreateScalarEntry,
CreateStringEntry, CreateQualityPresetEntry) guard against null from ResolveBinding via `if (const
auto Helper = ResolveBinding(...))`, but the direct call sites in the hardcoded init path
(Language:378-379, WindowMode:513-514, Resolution:528-529 in the .cpp) pass the result directly
without null-checking. Consider adding null checks at these call sites, or alternatively, add an
`ensureMsgf` warning inside ResolveBinding itself when a binding key is not found to make missing
keys immediately diagnosable.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m +TSharedPtr<FOptionsDataInteractionHelper> ResolveBinding(const FName& BindingKey) const;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // Consider: add ensureMsgf in the implementation to catch missing binding keys early:[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m // ensureMsgf(Found, TEXT("Binding key '%s' not found in BindingMap"), *BindingKey.ToString());[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m TSharedPtr<FOptionsDataInteractionHelper> ResolveBinding(const FName& BindingKey) const;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuEntryConfig.h:54-58 ───[0m
[93m[bug · medium][0m Scalar range members (ScalarMin/ScalarMax, ScalarOutMin/ScalarOutMax) lack
validation. If a data asset is authored with ScalarMin >= ScalarMax, the resulting TRange<float>
will be empty, causing GetMappedRangeValueClamped to always clamp to the lower bound — effectively
breaking the slider with no visible error. Consider adding an Ensure or a
PostLoad/PostEditChangeProperty validation that logs a warning when the bounds are invalid.

[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides, ClampMin = "0.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float ScalarMin = 0.f;[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m [0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides, ClampMin = "0.0"))[0m[0m
[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	float ScalarMax = 1.f;[0m[0m


[2m─── Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuEntryConfig.h:51-52 ───[0m
[93m[bug · medium][0m The scalar default value is stored as FString (DefaultValue) and parsed via
LexFromString in ListDataObject_Scalar::StringToFloat. An empty or malformed string silently yields
0.0f, which may fall outside the authored display range. This makes it easy for a designer to
accidentally ship a non-functional default. Consider using a float-typed property instead, or at
minimum logging a warning when the parse fails.

[2m[48;2;38;38;38m [0m[48;2;38;38;38m 	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "ValueType == EMenuEntryValueType::Scalar", EditConditionHides))[0m[0m
[91m[48;2;70;0;0m-[0m[48;2;70;0;0m 	FString DefaultValue;[0m[0m
[92m[48;2;0;60;0m+[0m[48;2;0;60;0m 	float DefaultScalarValue = 0.f;[0m[0m

