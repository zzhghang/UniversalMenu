#pragma once

#include "CoreMinimal.h"
#include "Internationalization/StringTableRegistry.h"

// ============================================================================
// StringTable asset paths — plain string literals (LOCTABLE does TEXT() internally)
// ============================================================================

#define ST_UI_COMMON        "/GameMainMenu/BP/StringTable/ST_UI_Common"
#define ST_OPTIONS_SCREEN   "/GameMainMenu/BP/StringTable/ST_OptionsScreen"
#define ST_OPTIONS_VALUES   "/GameMainMenu/BP/StringTable/ST_OptionsValues"
#define ST_GAME_MESSAGES    "/GameMainMenu/BP/StringTable/ST_GameMessages"
#define ST_MULTIPLAYER      "/GameMainMenu/BP/StringTable/ST_Multiplayer"

// ============================================================================
// UI Common — shared across all screens (OK, Cancel, Back, etc.)
// ============================================================================

#define TXT_UI_OK()         LOCTABLE(ST_UI_COMMON, "OK")
#define TXT_UI_CANCEL()     LOCTABLE(ST_UI_COMMON, "Cancel")
#define TXT_UI_BACK()       LOCTABLE(ST_UI_COMMON, "Back")
#define TXT_UI_CONFIRM()    LOCTABLE(ST_UI_COMMON, "Confirm")
#define TXT_UI_CLOSE()      LOCTABLE(ST_UI_COMMON, "Close")
#define TXT_UI_APPLY()      LOCTABLE(ST_UI_COMMON, "Apply")
#define TXT_UI_RESET()      LOCTABLE(ST_UI_COMMON, "Reset")
#define TXT_UI_SAVE()       LOCTABLE(ST_UI_COMMON, "Save")
#define TXT_UI_DELETE()     LOCTABLE(ST_UI_COMMON, "Delete")
#define TXT_UI_YES()        LOCTABLE(ST_UI_COMMON, "Yes")
#define TXT_UI_NO()         LOCTABLE(ST_UI_COMMON, "No")

// ============================================================================
// Shared Option Values — reused across multiple settings
// ============================================================================

// Quality presets
#define TXT_VAL_QUALITY_LOW()           LOCTABLE(ST_OPTIONS_VALUES, "Quality_Low")
#define TXT_VAL_QUALITY_MEDIUM()        LOCTABLE(ST_OPTIONS_VALUES, "Quality_Medium")
#define TXT_VAL_QUALITY_HIGH()          LOCTABLE(ST_OPTIONS_VALUES, "Quality_High")
#define TXT_VAL_QUALITY_EPIC()          LOCTABLE(ST_OPTIONS_VALUES, "Quality_Epic")
#define TXT_VAL_QUALITY_CINEMATIC()     LOCTABLE(ST_OPTIONS_VALUES, "Quality_Cinematic")

// Difficulty presets
#define TXT_VAL_DIFFICULTY_EASY()       LOCTABLE(ST_OPTIONS_VALUES, "Difficulty_Easy")
#define TXT_VAL_DIFFICULTY_NORMAL()     LOCTABLE(ST_OPTIONS_VALUES, "Difficulty_Normal")
#define TXT_VAL_DIFFICULTY_HARD()       LOCTABLE(ST_OPTIONS_VALUES, "Difficulty_Hard")
#define TXT_VAL_DIFFICULTY_NIGHTMARE()  LOCTABLE(ST_OPTIONS_VALUES, "Difficulty_Nightmare")

// Window mode
#define TXT_VAL_WINDOW_FULLSCREEN()     LOCTABLE(ST_OPTIONS_VALUES, "Window_Fullscreen")
#define TXT_VAL_WINDOW_BORDERLESS()     LOCTABLE(ST_OPTIONS_VALUES, "Window_Borderless")
#define TXT_VAL_WINDOW_WINDOWED()       LOCTABLE(ST_OPTIONS_VALUES, "Window_Windowed")

// Frame rate labels
#define TXT_VAL_FPS_UNLIMITED()         LOCTABLE(ST_OPTIONS_VALUES, "FPS_Unlimited")
#define TXT_VAL_FPS_30()                LOCTABLE(ST_OPTIONS_VALUES, "FPS_30")
#define TXT_VAL_FPS_60()                LOCTABLE(ST_OPTIONS_VALUES, "FPS_60")
#define TXT_VAL_FPS_120()               LOCTABLE(ST_OPTIONS_VALUES, "FPS_120")
#define TXT_VAL_FPS_144()               LOCTABLE(ST_OPTIONS_VALUES, "FPS_144")
#define TXT_VAL_FPS_240()               LOCTABLE(ST_OPTIONS_VALUES, "FPS_240")

// Control / Key binding
#define TXT_VAL_GAMEPAD()               LOCTABLE(ST_OPTIONS_VALUES, "Device_Gamepad")
#define TXT_VAL_KEYBOARD()              LOCTABLE(ST_OPTIONS_VALUES, "Device_Keyboard")
#define TXT_VAL_GENERAL()               LOCTABLE(ST_OPTIONS_VALUES, "Category_General")

// Generic value labels
#define TXT_VAL_INVALID()               LOCTABLE(ST_OPTIONS_VALUES, "Generic_Invalid")
#define TXT_VAL_ON()                    LOCTABLE(ST_OPTIONS_VALUES, "Generic_On")
#define TXT_VAL_OFF()                   LOCTABLE(ST_OPTIONS_VALUES, "Generic_Off")
#define TXT_VAL_CUSTOM()                LOCTABLE(ST_OPTIONS_VALUES, "Generic_Custom")
#define TXT_VAL_UNBOUND()               LOCTABLE(ST_OPTIONS_VALUES, "Generic_Unbound")

// ============================================================================
// Options Screen — Tab names
// ============================================================================

#define TXT_OPT_TAB_GAMEPLAY()      LOCTABLE(ST_OPTIONS_SCREEN, "Tab_Gameplay")
#define TXT_OPT_TAB_AUDIO()         LOCTABLE(ST_OPTIONS_SCREEN, "Tab_Audio")
#define TXT_OPT_TAB_VIDEO()         LOCTABLE(ST_OPTIONS_SCREEN, "Tab_Video")
#define TXT_OPT_TAB_CONTROL()       LOCTABLE(ST_OPTIONS_SCREEN, "Tab_Control")

// ============================================================================
// Options Screen — Category names
// ============================================================================

#define TXT_OPT_CAT_HUD()               LOCTABLE(ST_OPTIONS_SCREEN, "Category_HUD")
#define TXT_OPT_CAT_GAME()              LOCTABLE(ST_OPTIONS_SCREEN, "Category_Game")
#define TXT_OPT_CAT_VOLUME()            LOCTABLE(ST_OPTIONS_SCREEN, "Category_Volume")
#define TXT_OPT_CAT_SOUND()             LOCTABLE(ST_OPTIONS_SCREEN, "Category_Sound")
#define TXT_OPT_CAT_DISPLAY()           LOCTABLE(ST_OPTIONS_SCREEN, "Category_Display")
#define TXT_OPT_CAT_GRAPHICS()          LOCTABLE(ST_OPTIONS_SCREEN, "Category_Graphics")
#define TXT_OPT_CAT_ADVANCED_GRAPHICS() LOCTABLE(ST_OPTIONS_SCREEN, "Category_AdvancedGraphics")

// ============================================================================
// Options Screen — Gameplay tab entries
// ============================================================================

// HUD
#define TXT_OPT_HEALTH_BAR()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_HealthBar")
#define TXT_OPT_HEALTH_BAR_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_HealthBar_Desc")
#define TXT_OPT_DAMAGE_NUMBERS()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DamageNumbers")
#define TXT_OPT_DAMAGE_NUMBERS_DESC()       LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DamageNumbers_Desc")
#define TXT_OPT_HIGHLIGHT_INTERACTION()         LOCTABLE(ST_OPTIONS_SCREEN, "Entry_HighlightInteraction")
#define TXT_OPT_HIGHLIGHT_INTERACTION_DESC()    LOCTABLE(ST_OPTIONS_SCREEN, "Entry_HighlightInteraction_Desc")
#define TXT_OPT_SUBTITLES()                 LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Subtitles")
#define TXT_OPT_SUBTITLES_DESC()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Subtitles_Desc")

// Game
#define TXT_OPT_DIFFICULTY()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Difficulty")
#define TXT_OPT_DIFFICULTY_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Difficulty_Desc")
#define TXT_OPT_AUTO_SAVE()                 LOCTABLE(ST_OPTIONS_SCREEN, "Entry_AutoSave")
#define TXT_OPT_AUTO_SAVE_DESC()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_AutoSave_Desc")
#define TXT_OPT_LANGUAGE()                  LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Language")
#define TXT_OPT_LANGUAGE_DESC()             LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Language_Desc")

// ============================================================================
// Options Screen — Audio tab entries
// ============================================================================

// Volume
#define TXT_OPT_OVERALL_VOLUME()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_OverallVolume")
#define TXT_OPT_OVERALL_VOLUME_DESC()       LOCTABLE(ST_OPTIONS_SCREEN, "Entry_OverallVolume_Desc")
#define TXT_OPT_MUSIC_VOLUME()              LOCTABLE(ST_OPTIONS_SCREEN, "Entry_MusicVolume")
#define TXT_OPT_MUSIC_VOLUME_DESC()         LOCTABLE(ST_OPTIONS_SCREEN, "Entry_MusicVolume_Desc")
#define TXT_OPT_SFX_VOLUME()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_SFXVolume")
#define TXT_OPT_SFX_VOLUME_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_SFXVolume_Desc")
#define TXT_OPT_VOICE_VOLUME()              LOCTABLE(ST_OPTIONS_SCREEN, "Entry_VoiceVolume")
#define TXT_OPT_VOICE_VOLUME_DESC()         LOCTABLE(ST_OPTIONS_SCREEN, "Entry_VoiceVolume_Desc")
#define TXT_OPT_AMBIENT_VOLUME()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_AmbientVolume")
#define TXT_OPT_AMBIENT_VOLUME_DESC()       LOCTABLE(ST_OPTIONS_SCREEN, "Entry_AmbientVolume_Desc")
#define TXT_OPT_UI_VOLUME()                 LOCTABLE(ST_OPTIONS_SCREEN, "Entry_UIVolume")
#define TXT_OPT_UI_VOLUME_DESC()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_UIVolume_Desc")

// Sound
#define TXT_OPT_BACKGROUND_SOUND()          LOCTABLE(ST_OPTIONS_SCREEN, "Entry_BackgroundSound")
#define TXT_OPT_BACKGROUND_SOUND_DESC()     LOCTABLE(ST_OPTIONS_SCREEN, "Entry_BackgroundSound_Desc")
#define TXT_OPT_SUBTITLES_SOUND()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_SubtitlesSound")
#define TXT_OPT_SUBTITLES_SOUND_DESC()      LOCTABLE(ST_OPTIONS_SCREEN, "Entry_SubtitlesSound_Desc")
#define TXT_OPT_3D_SPATIAL_AUDIO()          LOCTABLE(ST_OPTIONS_SCREEN, "Entry_3DSpatialAudio")
#define TXT_OPT_3D_SPATIAL_AUDIO_DESC()     LOCTABLE(ST_OPTIONS_SCREEN, "Entry_3DSpatialAudio_Desc")
#define TXT_OPT_MONO_OUTPUT()               LOCTABLE(ST_OPTIONS_SCREEN, "Entry_MonoOutput")
#define TXT_OPT_MONO_OUTPUT_DESC()          LOCTABLE(ST_OPTIONS_SCREEN, "Entry_MonoOutput_Desc")
#define TXT_OPT_DYNAMIC_RANGE()             LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DynamicRange")
#define TXT_OPT_DYNAMIC_RANGE_DESC()        LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DynamicRange_Desc")
#define TXT_OPT_DIALOGUE_BOOST()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DialogueBoost")
#define TXT_OPT_DIALOGUE_BOOST_DESC()       LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DialogueBoost_Desc")

// ============================================================================
// Options Screen — Video tab entries
// ============================================================================

// Display
#define TXT_OPT_WINDOW_MODE()               LOCTABLE(ST_OPTIONS_SCREEN, "Entry_WindowMode")
#define TXT_OPT_WINDOW_MODE_DESC()          LOCTABLE(ST_OPTIONS_SCREEN, "Entry_WindowMode_Desc")
#define TXT_OPT_RESOLUTION()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Resolution")
#define TXT_OPT_RESOLUTION_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_Resolution_Desc")

// Graphics
#define TXT_OPT_DISPLAY_GAMMA()             LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DisplayGamma")
#define TXT_OPT_DISPLAY_GAMMA_DESC()        LOCTABLE(ST_OPTIONS_SCREEN, "Entry_DisplayGamma_Desc")
#define TXT_OPT_OVERALL_QUALITY()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_OverallQuality")
#define TXT_OPT_OVERALL_QUALITY_DESC()      LOCTABLE(ST_OPTIONS_SCREEN, "Entry_OverallQuality_Desc")
#define TXT_OPT_RESOLUTION_SCALE()          LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ResolutionScale")
#define TXT_OPT_RESOLUTION_SCALE_DESC()     LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ResolutionScale_Desc")
#define TXT_OPT_GI_QUALITY()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_GIQuality")
#define TXT_OPT_GI_QUALITY_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_GIQuality_Desc")
#define TXT_OPT_SHADOW_QUALITY()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ShadowQuality")
#define TXT_OPT_SHADOW_QUALITY_DESC()       LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ShadowQuality_Desc")
#define TXT_OPT_AA_QUALITY()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_AAQuality")
#define TXT_OPT_AA_QUALITY_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_AAQuality_Desc")
#define TXT_OPT_VIEW_DISTANCE_QUALITY()         LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ViewDistanceQuality")
#define TXT_OPT_VIEW_DISTANCE_QUALITY_DESC()    LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ViewDistanceQuality_Desc")
#define TXT_OPT_TEXTURE_QUALITY()            LOCTABLE(ST_OPTIONS_SCREEN, "Entry_TextureQuality")
#define TXT_OPT_TEXTURE_QUALITY_DESC()       LOCTABLE(ST_OPTIONS_SCREEN, "Entry_TextureQuality_Desc")
#define TXT_OPT_VFX_QUALITY()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_VFXQuality")
#define TXT_OPT_VFX_QUALITY_DESC()           LOCTABLE(ST_OPTIONS_SCREEN, "Entry_VFXQuality_Desc")
#define TXT_OPT_REFLECTION_QUALITY()         LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ReflectionQuality")
#define TXT_OPT_REFLECTION_QUALITY_DESC()    LOCTABLE(ST_OPTIONS_SCREEN, "Entry_ReflectionQuality_Desc")
#define TXT_OPT_POSTPROCESS_QUALITY()        LOCTABLE(ST_OPTIONS_SCREEN, "Entry_PostProcessQuality")
#define TXT_OPT_POSTPROCESS_QUALITY_DESC()   LOCTABLE(ST_OPTIONS_SCREEN, "Entry_PostProcessQuality_Desc")

// Advanced Graphics
#define TXT_OPT_VSYNC()                     LOCTABLE(ST_OPTIONS_SCREEN, "Entry_VSync")
#define TXT_OPT_VSYNC_DESC()                LOCTABLE(ST_OPTIONS_SCREEN, "Entry_VSync_Desc")
#define TXT_OPT_FRAME_RATE_LIMIT()          LOCTABLE(ST_OPTIONS_SCREEN, "Entry_FrameRateLimit")
#define TXT_OPT_FRAME_RATE_LIMIT_DESC()     LOCTABLE(ST_OPTIONS_SCREEN, "Entry_FrameRateLimit_Desc")

// ============================================================================
// Options Screen — Rich-text warning messages
// ============================================================================

#define TXT_OPT_WARN_PACKAGED_ONLY() \
    LOCTABLE(ST_OPTIONS_SCREEN, "Warning_PackagedBuildOnly")
#define TXT_OPT_WARN_BORDERLESS_DISABLED() \
    LOCTABLE(ST_OPTIONS_SCREEN, "Warning_BorderlessDisabled")
#define TXT_OPT_WARN_FULLSCREEN_ONLY() \
    LOCTABLE(ST_OPTIONS_SCREEN, "Warning_FullscreenOnly")

#define TXT_OPT_RESET_CONFIRM() \
    LOCTABLE(ST_OPTIONS_SCREEN, "Confirm_ResetCategory")

// ============================================================================
// Key Remap Screen
// ============================================================================

#define TXT_KEYREMAP_PROMPT_KEYBOARD()  LOCTABLE(ST_OPTIONS_SCREEN, "KeyRemap_Prompt_Keyboard")
#define TXT_KEYREMAP_PROMPT_GAMEPAD()   LOCTABLE(ST_OPTIONS_SCREEN, "KeyRemap_Prompt_Gamepad")

// ============================================================================
// Game Messages — in-game notifications, tips, alerts
// ============================================================================

#define TXT_GAME_SAVED()            LOCTABLE(ST_GAME_MESSAGES, "Game_Saved")
#define TXT_GAME_LOADING()          LOCTABLE(ST_GAME_MESSAGES, "Loading")
#define TXT_GAME_AUTOSAVE()         LOCTABLE(ST_GAME_MESSAGES, "AutoSave")
#define TXT_GAME_QUIT_CONFIRM()     LOCTABLE(ST_GAME_MESSAGES, "Quit_Confirm")

// ============================================================================
// Multiplayer Screen — Tab names
// ============================================================================

#define TXT_MP_TAB_CREATE_GAME()    LOCTABLE(ST_MULTIPLAYER, "Tab_CreateGame")
#define TXT_MP_TAB_JOIN_GAME()      LOCTABLE(ST_MULTIPLAYER, "Tab_JoinGame")

// ============================================================================
// Multiplayer Screen — Category names
// ============================================================================

#define TXT_MP_CAT_BASIC()          LOCTABLE(ST_MULTIPLAYER, "Category_Basic")
#define TXT_MP_CAT_ADVANCED()       LOCTABLE(ST_MULTIPLAYER, "Category_Advanced")

// ============================================================================
// Multiplayer Screen — Labels
// ============================================================================

#define TXT_MP_LABEL_GAME_NAME()    LOCTABLE(ST_MULTIPLAYER, "Label_GameName")
#define TXT_MP_LABEL_HAS_PASSWORD() LOCTABLE(ST_MULTIPLAYER, "Label_HasPassword")
#define TXT_MP_LABEL_PASSWORD()     LOCTABLE(ST_MULTIPLAYER, "Label_Password")
#define TXT_MP_LABEL_MAX_PLAYERS()  LOCTABLE(ST_MULTIPLAYER, "Label_MaxPlayers")
#define TXT_MP_LABEL_MAP()          LOCTABLE(ST_MULTIPLAYER, "Label_Map")
#define TXT_MP_LABEL_GAME_MODE()    LOCTABLE(ST_MULTIPLAYER, "Label_GameMode")
#define TXT_MP_LABEL_SERVER_REGION() LOCTABLE(ST_MULTIPLAYER, "Label_ServerRegion")
#define TXT_MP_LABEL_PRIVACY()      LOCTABLE(ST_MULTIPLAYER, "Label_Privacy")
#define TXT_MP_LABEL_SERVER_LIST()  LOCTABLE(ST_MULTIPLAYER, "Label_ServerList")

// ============================================================================
// Multiplayer Screen — Descriptions
// ============================================================================

#define TXT_MP_HINT_GAME_NAME()     LOCTABLE(ST_MULTIPLAYER, "Hint_GameName")
#define TXT_MP_HINT_PASSWORD()      LOCTABLE(ST_MULTIPLAYER, "Hint_Password")

#define TXT_MP_DESC_GAME_NAME()     LOCTABLE(ST_MULTIPLAYER, "Desc_GameName")
#define TXT_MP_DESC_HAS_PASSWORD()  LOCTABLE(ST_MULTIPLAYER, "Desc_HasPassword")
#define TXT_MP_DESC_PASSWORD()      LOCTABLE(ST_MULTIPLAYER, "Desc_Password")
#define TXT_MP_DESC_MAX_PLAYERS()   LOCTABLE(ST_MULTIPLAYER, "Desc_MaxPlayers")
#define TXT_MP_DESC_MAP()           LOCTABLE(ST_MULTIPLAYER, "Desc_Map")
#define TXT_MP_DESC_GAME_MODE()     LOCTABLE(ST_MULTIPLAYER, "Desc_GameMode")
#define TXT_MP_DESC_SERVER_REGION() LOCTABLE(ST_MULTIPLAYER, "Desc_ServerRegion")
#define TXT_MP_DESC_PRIVACY()       LOCTABLE(ST_MULTIPLAYER, "Desc_Privacy")
#define TXT_MP_DESC_SERVER_LIST()   LOCTABLE(ST_MULTIPLAYER, "Desc_ServerList")

// ============================================================================
// Multiplayer Screen — Buttons
// ============================================================================

#define TXT_MP_BUTTON_CREATE()      LOCTABLE(ST_MULTIPLAYER, "Button_Create")
#define TXT_MP_BUTTON_JOIN()        LOCTABLE(ST_MULTIPLAYER, "Button_Join")
#define TXT_MP_BUTTON_REFRESH()     LOCTABLE(ST_MULTIPLAYER, "Button_Refresh")

// ============================================================================
// Validation Messages
// ============================================================================

#define TXT_MP_VALIDATION_TITLE()           LOCTABLE(ST_MULTIPLAYER, "Validation_Title")
#define TXT_MP_VALIDATION_FILTERED()        LOCTABLE(ST_MULTIPLAYER, "Validation_Filtered")
#define TXT_MP_VALIDATION_NUMBERS_ONLY()    LOCTABLE(ST_MULTIPLAYER, "Validation_NumbersOnly")
#define TXT_MP_VALIDATION_LETTERS_ONLY()    LOCTABLE(ST_MULTIPLAYER, "Validation_LettersOnly")
#define TXT_MP_VALIDATION_ALPHANUMERIC()    LOCTABLE(ST_MULTIPLAYER, "Validation_Alphanumeric")
#define TXT_MP_VALIDATION_MAX_CHARS()       LOCTABLE(ST_MULTIPLAYER, "Validation_MaxChars")
