// HangHang All Rights Reserved


#include "FrontendGameplayTags.h"

namespace FrontendGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Frontend_WidgetStack_Modal, "Frontend.WidgetStack.Modal");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_WidgetStack_GameMenu, "Frontend.WidgetStack.GameMenu");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_WidgetStack_GameHud, "Frontend.WidgetStack.GameHud");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_WidgetStack_Frontend, "Frontend.WidgetStack.Frontend");
	
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_PressAnyKey, "Frontend.Widget.PressAnyKey");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_MainMenu, "Frontend.Widget.MainMenu");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_Confirm, "Frontend.Widget.Confirm");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_Options, "Frontend.Widget.Options");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_KeyRemap, "Frontend.Widget.KeyRemap");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_Credits, "Frontend.Widget.Credits");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Widget_Multiplayer, "Frontend.Widget.Multiplayer");

	UE_DEFINE_GAMEPLAY_TAG(Frontend_Image_Test, "Frontend.Image.Test");

	// Audio Volume Tags
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Audio_Volume_Overall, "Frontend.Audio.Volume.Overall");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Audio_Volume_Music, "Frontend.Audio.Volume.Music");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Audio_Volume_SFX, "Frontend.Audio.Volume.SFX");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Audio_Volume_Voice, "Frontend.Audio.Volume.Voice");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Audio_Volume_Ambient, "Frontend.Audio.Volume.Ambient");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Audio_Volume_UI, "Frontend.Audio.Volume.UI");

	// Loading Screen Phase Tags
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Loading_Phase_LevelLoad, "Frontend.Loading.Phase.LevelLoad");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Loading_Phase_WorldInit, "Frontend.Loading.Phase.WorldInit");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Loading_Phase_BeginPlay, "Frontend.Loading.Phase.BeginPlay");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Loading_Phase_PlayerController, "Frontend.Loading.Phase.PlayerController");
	UE_DEFINE_GAMEPLAY_TAG(Frontend_Loading_Phase_TextureStreaming, "Frontend.Loading.Phase.TextureStreaming");

}
