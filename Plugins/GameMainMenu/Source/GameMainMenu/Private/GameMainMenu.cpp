// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameMainMenu.h"

#include "LogGameMainMenu.h"
#include "UObject/UObjectGlobals.h"

DEFINE_LOG_CATEGORY(LogGameMainMenu);

#define LOCTEXT_NAMESPACE "FGameMainMenuModule"

void FGameMainMenuModule::StartupModule()
{
	// ponytail: pre-load StringTables so LOCTABLE macros resolve on first startup.
	// Without this, the registry lookup fails until the assets are loaded async.
	static const TCHAR* StringTables[] = {
		TEXT("/GameMainMenu/BP/StringTable/ST_Multiplayer"),
		TEXT("/GameMainMenu/BP/StringTable/ST_GameMessages"),
		TEXT("/GameMainMenu/BP/StringTable/ST_UI_Common"),
		TEXT("/GameMainMenu/BP/StringTable/ST_OptionsScreen"),
		TEXT("/GameMainMenu/BP/StringTable/ST_OptionsValues"),
	};
	for (const TCHAR* Path : StringTables)
	{
		LoadObject<UObject>(nullptr, Path);
	}
}

void FGameMainMenuModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGameMainMenuModule, GameMainMenu)