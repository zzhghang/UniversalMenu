## Why

联机界面目前缺少可编辑文本输入控件（房间名、密码等），且 MultiplayerDataRegistry 的数据层级只有 tab→entry 的扁平结构，无法支持分类分组。需要新增可编辑文本控件及其数据对象，同时将 MultiplayerDataRegistry 重构为 tab→category→entry 的三层结构，对齐 OptionsDataRegistry 的数据驱动模式，避免硬编码。

## What Changes

- **新增 `UListDataObject_Editable`**：可编辑文本数据对象，支持房间名字符数限制、密码位数限制、中文字符支持、密码哈希存储（不存明文）
- **新增 `UW_ListEntry_Editable` 实现**：绑定 UE EditableText 控件，响应文本变更并通过数据对象存储
- **重构 `MultiplayerDataRegistry`**：将 CreateGame tab 拆分为"基础"和"高级"两个 category，增加密码开关等新条目，结构对齐 OptionsDataRegistry 的 tab→category→entry 模式
- **扩展 `MultiplayerConfigData`**：支持 category 层级，增加条目编辑依赖条件（密码开关控制密码输入框可见/可用）
- **新增硬编码 fallback 数据**：房间名、密码开关、密码、最大玩家数、地图、游戏模式、服务器地区等

## Capabilities

### New Capabilities
- `editable-list-entry`: 可编辑文本控件及数据对象，支持字符限制、密码哈希、中文输入
- `multiplayer-category-hierarchy`: MultiplayerDataRegistry 的三层数据结构（tab→category→entry），含密码开关对密码输入的编辑依赖控制

### Modified Capabilities
<!-- 无现有 spec 需要修改 -->

## Impact

- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/DataObjects/ListDataObject_Editable.h` — 从空壳扩展为完整数据对象
- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/ListEntries/W_ListEntry_Editable.h` — 从空壳扩展为完整控件
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp/h` — 重构为三层结构
- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/MultiplayerConfigData.h` — 扩展 DataAsset 支持 category
- `Plugins/GameMainMenu/Content/BP/UI/Widgets/Multiplayer/` — 新增/更新蓝图 Widget
