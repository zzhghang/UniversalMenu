## Why

JoinGame tab 目前只有 HeaderRow，下方缺少实际的服务器 Session 列表。玩家无法看到可加入的房间，JoinGame 流程不完整。需要在 HeaderRow 下方填充从 OnlineSubsystem 搜索到的 Session 数据行。

## What Changes

- 新增 `UListDataObject_Session` 数据对象，持有单个 Session 的展示数据（服务器名、玩家数、Ping、状态）
- 新增 `W_ListEntry_Session` 列表项控件，以多列形式展示 Session 信息，列对齐 HeaderRow
- `MultiplayerDataRegistry` 增加 Session 列表的替换/刷新能力
- `W_MultiplayerScreen` 暴露 `PopulateSessionList` BlueprintCallable，供 BP 在 FindSessions 回调中注入 Session 数据
- `DA_ListEntryMapping` DataAsset 增加 Session 数据对象到 Entry 控件的映射

## Capabilities

### New Capabilities

- `session-list-display`: JoinGame tab 的 Session 列表展示，包含数据对象、列表项控件、数据填充接口

### Modified Capabilities

<!-- 无现有 specs 需要修改 -->

## Impact

- **新增文件**: `ListDataObject_Session.h/.cpp`, `W_ListEntry_Session.h/.cpp`
- **修改文件**: `MultiplayerDataRegistry.h/.cpp`, `W_MultiplayerScreen.h/.cpp`
- **新增 Blueprint**: `WBP_ListEntry_Session` (编辑器中创建)
- **修改 DataAsset**: `DA_ListEntryMapping` (编辑器中添加映射)
- **依赖**: AdvancedSessions 插件、OnlineSubsystem — BP 侧消费，C++ 侧无直接依赖
