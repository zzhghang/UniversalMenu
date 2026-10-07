## Why

`UMenuConfigData` DataAsset 驱动的初始化路径存在多个 bug，导致通过 DataAsset 配置的 Options/Multiplayer 条目无法正常读写设置、缺失类型支持、以及 WindowMode-Resolution 联动关系不完整。这些问题在 DataAsset 配置刚迁移完成、尚未正式跑通时暴露。

## What Changes

- **修复 `CreateEntryFromConfig` 缺失 BindingKey 绑定**: 从 DataAsset 创建的条目无法读写 `UFrontendGameUserSettings`，因为 getter/setter 从未被注入。需要在 Registry 层统一绑定。
- **修复 `MultiplayerDataRegistry::InitFromConfig` 类型支持不全**: String/Integer 类型的条目（Map、GameMode、MaxPlayers 等）掉入 default 分支变成空 `Collection`，需改用共享工厂 `CreateEntryFromConfig`。
- **修复 `InitFromDataAsset` 中 Resolution→WindowMode 的 EditDependencyData 缺失**: `ApplyTabEditConditions` 只添加了 EditCondition，但没有调用 `AddEditDependencyData`，导致 WindowMode 变化时 Resolution 的 UI 不刷新。
- **新增 `FullscreenMode` BindingKey 的工厂特殊处理**: `CreateEntryFromConfig` 对 WindowMode 条目（BindingKey=FullscreenMode）没有特殊处理，需创建带 Fullscreen/WindowedFullscreen/Windowed 枚举选项的 `UListDataObject_StringEnum`。
- **清理硬编码 Fallback 路径**: 既然 DataAsset 已配置，移除 `InitFromConfig`/`InitFromDataAsset` 路径中的残留问题，对齐 Document 中的配置约定。

## Capabilities

### Modified Capabilities
- `frontend-settings-config`: 修复 DataAsset 驱动路径条目无法绑定设置的问题，新增 FullscreenMode 工厂处理

## Impact

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp` — `CreateEntryFromConfig` 新增 FullscreenMode 分支
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp` — `InitFromDataAsset` 绑定 BindingKey；`ApplyTabEditConditions` 补全 EditDependencyData
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp` — `InitFromConfig` 改用共享工厂
- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuConfigData.h` — 可能新增 `CreateEntryFromConfig` 签名参数
