# Unreal visual reference

These values were decoded from the source `.uasset` property streams. Fonts and texture source pixels were extracted directly; no Unreal content was changed.

## Fonts

The real Source Han Sans CN OpenType font files used by the widgets are in `../assets/`. `Default` in the composite font maps to `Regular`.

The stored `FSlateFontInfo.Size` is in points. UE 5.7 renders at `FontConstants::RenderDPI = 96`, so the nominal browser size at UI scale 1 is `stored size * 96 / 72`. The project's `FontDPIPreset=Standard` affects the editor's displayed units, not the renderer's constant. Screen/UI scaling must be applied separately.

| CommonTextStyle | Typeface | Stored size | Browser px at scale 1 | Linear color | Approx. sRGB |
| --- | --- | ---: | ---: | ---: | --- |
| SText_Default | Regular | 26.25 | 35 | 0.3 | #959595 |
| SText_Default_HighLight | Regular | 26.25 | 35 | 1 | #FFFFFF |
| SText_ListEntry | Normal | 18.75 | 25 | 0.3 | #959595 |
| SText_ListEntry_HighLight | Normal | 18.75 | 25 | 1 | #FFFFFF |
| SText_ListEntry_Header | Normal | 22.5 | 30 | 0.8 | #E7E7E7 |
| SText_ListEntry_HeaderRow | Normal | 22.5 | 30 | 0.8 | #E7E7E7 |
| SText_ListEntry_Session | Normal | 21 | 28 | 0.3 | #959595 |
| SText_ListEntry_Session_HighLight | Normal | 21 | 28 | 1 | #FFFFFF |
| SText_OptionDetailViewTtile | Normal | 30 | 40 | 1 | #FFFFFF |
| SText_Button_Desc | Regular | 15 | 20 | 0.6 | #CBCBCB |
| SText__ButtonBoundAction | Light | 15 | 20 | 0.5 | #BCBCBC |
| SText_CopyRight | Regular | 11.25 | 15 | 0.5 | #BCBCBC |
| SText_Confirm | Regular | 21 | 28 | 0.3 | #959595 |
| SText_Confirm_HightLight | Regular | 21 | 28 | 1 | #FFFFFF |
| SText_CreditsScreen | Regular | 26.25 | 35 | 1 | #FFFFFF |

All styles above use a one-unit black outline at alpha 0.5. Their text color alpha is 1. `SText_ListEntry_Header` also has `SkewAmount=0.3`; HeaderRow does not. Magenta `ColorAndOpacity` stored on several CommonTextBlock instances is overridden by the CommonTextStyle and is not the rendered color.

## Layout constants

- Template top band: minimum height 80, top padding 20.
- Template center columns: flexible ratio 1.3 to 1, separated by a 100-unit spacer.
- Main-menu left named slot: left padding 250; first button top padding 150; subsequent button top padding 36.
- Footer: padding left 100, top 16, right 16, bottom 32. Bottom columns ratio 1.4 to 1 with 64-unit spacer, right contents have 30-unit right padding.
- Options/multiplayer: black background alpha 1, blur strength 5; tabs centered at top with horizontal padding 25 per button; action-bar spacing 16; scrollbar thickness 5 and left padding 10.
- Confirmation: centered vertical content; caution icon 36 by 32, opacity 0.6; vertical spacer sizes 20, 10, 25; button gap 25; background blur 5.
- Key-remap overlay: full screen, black alpha 0.7, blur strength 7, centered rich text.
- Loading: black full screen, bottom-right content with padding 50; loading title uses detail-title style; throbber 64 by 64 with gap 20; additional spacer 90; loading reason uses ListEntry style and top padding 10.
- Credits: black full screen; top content padding 200; UE logo is 371 by 450; text style is 35 px at scale 1.
- Press-any-key: centered text; copyright footer is `Copyright © 2026 Hangning. All rights reserved.`; version is at bottom right.

Widget object properties and named-slot bindings are preserved in the `WBP_*-properties.json` files.

## Images and extraction

`texture-manifest.json` records the original `.uasset`, exported PNG, and pixel dimensions. It includes 86 menu-relevant textures: caution, horizontal arrow, slider handle, Unreal logos, all configured keyboard/mouse bindings, Xbox bindings, Esc, four keyboard arrows, and alternate LT/RT/LB/RB labels. Textures are lossless source-pixel exports, decoded from UE compressed source data and the ImageCore row-delta transform. They are not thumbnail captures.

`../assets-manifest.json` has a browser-friendly `keyToFile` lookup, font mappings and styles, texture dimensions, and startup-video metadata. `configuredKeyToFile` reproduces 72 mappings from CData_MouseKeyboard/CData_Gamepad exactly; five Escape/arrow mappings are supplemental. The original project uses XboxElite paddle textures for trigger axes and shoulders, despite LT/RT/LB/RB textures being available. Both sets are present and distinguished in the manifest. `UELogoAnim.mp4` is a byte-for-byte copy verified by SHA-256.

`extract_texture_png.py` contains the decoding procedure and uses the locally installed UE 5.7 Oodle decompressor. `extract_widget_properties.py` extracts tagged property streams using package tables from `UnrealPackageTool`.

The project config and menu C++ source did not specify a custom cursor asset. Browser/native default cursor is the closest available baseline; a custom cursor should not be invented.
