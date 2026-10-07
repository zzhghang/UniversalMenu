## Why

OptionsScreen 与 MultiplayerScreen 各自维护了两套并存的构建实现:DataAsset(`UMenuConfigData`)配置驱动路径,以及配置缺失时触发的硬编码回退路径。两套内容重复(硬编码的 Tab/条目与 DataAsset 资产一一对应),改菜单要改两处,且硬编码路径绕过了配置工厂,行为容易漂移。应删除硬编码路径,让 DataAsset 成为唯一数据源,并清理其余残留的硬编码点。

## What Changes

- 删除 `UMultiplayerDataRegistry::InitHardcodedFallback()` 及 `InitRegistry()` 中的回退分支;配置缺失时改为 `ensure` + 日志 + 空注册表。**BREAKING**:未指定配置的 MultiplayerScreen 将不再显示硬编码菜单。
- 删除 `UOptionsDataRegistry` 中四个硬编码 Tab 构建函数(`InitGameplayCollectionTab`/`InitAudioCollectionTab`/`InitVideoCollectionTab`/`InitControlCollectionTab`)及 `InitOptionsDataRegistry()` 中的回退分支;配置缺失时同样降级为空注册表。**BREAKING**:OptionsScreen 将只显示 `DA_OptionsConfig` 中定义的 Tab(当前资产含 Gameplay/Audio/Video,不含硬编码版曾有的 Control Tab)。
- 删除仅被硬编码路径使用的辅助函数 `CreateBoolEntry`/`CreateStringEntry`/`CreateScalarEntry` 及其头文件声明。
- 删除过时备份文件 `OptionsDataRegistry.cpp.bak`。
- 删除回退删除后无引用的本地化键 `TXT_MP_TAB_CREATE_GAME`/`TXT_MP_TAB_JOIN_GAME`。
- `W_MultiplayerScreen` 中的 `"CreateGame"`/`"JoinGame"` Tab ID 字面量改为配置驱动:`UMenuTabConfig` 新增 `TabRole` 字段(None/CreateGame/JoinGame),动作绑定与初始 Tab 选择基于配置。
- `CreateEntryFromConfig()` 中窗口模式与画质档位的硬编码选项列表,在配置提供 `AvailableOptions`/`AvailableIntegerOptions` 时优先使用配置,否则沿用引擎默认值(现有资产行为不变)。

## Capabilities

### New Capabilities

- `config-driven-screen-registries`: Options/Multiplayer 屏幕注册表仅从 `UMenuConfigData` 构建,不保留硬编码回退;Tab 语义角色与初始 Tab 由配置声明。

### Modified Capabilities

<!-- 无现有主规格覆盖 Options/Multiplayer 注册表行为,无需修改既有规格 -->

## Impact

- 代码:`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp/.h`、`Private/Widgets/Options/OptionsDataRegistry.cpp/.h`、`Private/Widgets/Options/OptionsDataRegistry.cpp.bak`(删除)、`Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp`、`Public/Widgets/MenuConfig/MenuTabConfig.h`、`Private/Widgets/MenuConfig/MenuConfigData.cpp`、`Public/LocalizationKeys.h`。
- 资产(需在编辑器手动修改):`DA_CreateGame`/`DA_JoinGame` 设置 TabRole;确认 `WBP_OptionScreen`/`WBP_MultiplayerScreen` 已指定 `DA_OptionsConfig`/`DA_MultiplayerConfig`;如需要 Control Tab,在 `DA_OptionsConfig` 中补充对应资产。
- 依赖:无新增依赖。编译需 UE 5.7 环境验证。
