## Why

游戏需要多人联机功能入口，让玩家能够创建主机或加入已有游戏。当前项目缺少多人游戏界面，需要在现有 W_MultiplayerScreen 框架上实现。

## What Changes

- 在现有 UW_MultiplayerScreen 类中实现完整的多人游戏界面逻辑
- 新增"创建游戏"和"加入游戏"两个 Tab，参考 OptionScreen 的 Tab 导航模式
- 新增多人游戏相关本地化文本到 StringTable
- 新增 LocalizationKeys.h 中的多人游戏文本宏定义
- 蓝图侧只需绑定对应 Widget 并配置 Tab 注册即可

## Capabilities

### New Capabilities
- `multiplayer-screen`: 多人游戏主界面，包含创建游戏和加入游戏两个 Tab 的导航与功能逻辑

### Modified Capabilities
<!-- 无现有 spec 受影响 -->

## Impact

- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/W_MultiplayerScreen.h` — 扩展类声明
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp` — 实现界面逻辑
- `Plugins/GameMainMenu/Source/GameMainMenu/Public/LocalizationKeys.h` — 新增多人游戏文本宏
- `Plugins/GameMainMenu/Content/BP/StringTable/` — 需要蓝图中新增 StringTable 条目
- `Plugins/GameMainMenu/Content/BP/UI/Widgets/WBP_MultiplayerScreen.uasset` — 蓝图 Widget 绑定
