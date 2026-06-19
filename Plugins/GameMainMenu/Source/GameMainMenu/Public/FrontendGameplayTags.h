// HangHang All Rights Reserved

#pragma once

#include "NativeGameplayTags.h"

/**
 * Gameplay tags for frontend UI routing and resource resolution.
 *
 * Tags are grouped into:
 * - Widget stacks (Modal, GameMenu, GameHud, Frontend)
 * - Widget types (PressAnyKey, MainMenu, Confirm, Options, KeyRemap)
 * - Image references (Test)
 * - Audio volume channels (Overall, Music, SFX, Voice, Ambient, UI)
 */
namespace FrontendGameplayTags
{
	// ---- Widget Stacks ----
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Modal);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameMenu);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_GameHud);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_WidgetStack_Frontend);

	// ---- Widget Types ----
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_PressAnyKey);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_MainMenu);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_Confirm);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_Options);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Widget_KeyRemap);

	// ---- Image References ----
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Image_Test);

	// ---- Audio Volume Channels ----
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Audio_Volume_Overall);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Audio_Volume_Music);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Audio_Volume_SFX);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Audio_Volume_Voice);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Audio_Volume_Ambient);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Audio_Volume_UI);

	// ---- Loading Screen Phases ----
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Loading_Phase_LevelLoad);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Loading_Phase_WorldInit);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Loading_Phase_BeginPlay);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Loading_Phase_PlayerController);
	GAMEMAINMENU_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Frontend_Loading_Phase_TextureStreaming);

}
