## Why

`FrontendDeveloperSettings` 和 `FrontendLoadingScreenSettings` 当前使用 `Config = Game`，导致配置保存在主项目的 `Config/DefaultGame.ini` 中，而非插件自身的配置文件。这两类是 GameMainMenu 插件内部使用的设置，理应属于插件配置，迁移至插件的 `Config/DefaultGameMainMenu.ini` 可改善模块边界和数据归属。

## What Changes

- `UFrontendDeveloperSettings` 的 `UCLASS` 宏从 `Config = Game` 改为 `Config = GameMainMenu`
- `UFrontendLoadingScreenSettings` 的 `UCLASS` 宏从 `Config = Game` 改为 `Config = GameMainMenu`
- 将 `Config/DefaultGame.ini` 中对应的两个 section 迁移至 `Plugins/GameMainMenu/Config/DefaultGameMainMenu.ini`
- `UFrontendGameUserSettings` 保持不变（继承自 `UGameUserSettings`，用户级配置，继续使用 `Config = Game`）

## Capabilities

### New Capabilities
<!-- No new capabilities — this is a configuration relocation, not a feature change -->

### Modified Capabilities
<!-- No existing capability requirements change — config values and behavior remain identical -->

## Impact

- `Plugins/GameMainMenu/Source/GameMainMenu/Public/FrontendSettings/FrontendDeveloperSettings.h` — UCLASS 宏修改
- `Plugins/GameMainMenu/Source/GameMainMenu/Public/FrontendSettings/FrontendLoadingScreenSettings.h` — UCLASS 宏修改
- `Config/DefaultGame.ini` — 删除两个 section
- `Plugins/GameMainMenu/Config/DefaultGameMainMenu.ini` — 新增两个 section
