## Why

`UW_ListEntry_Session` 目前只负责展示 session 信息,选中条目时没有可让蓝图处理的回调,数据链路中也未携带 `FBlueprintSessionResult`。蓝图侧无法实现"选中某条 session 后调用 Join Session 节点加入游戏"的流程。

## What Changes

- `FSessionListItem` 增加 `FBlueprintSessionResult` 字段,让搜索结果随现有 `PopulateSessionList` → `RebuildSessionList` 数据链路流到列表条目。
- ~~`UW_ListEntry_Session` 增加绑定按钮 `Cbb_Join`(`UFrontendCommonButtonBase`),按下时调用新的蓝图实现事件 `BP_OnJoinButtonClicked`。~~(已撤销,触发方式待定)
- `UW_ListEntry_Session` 缓存当前 session 的 `FBlueprintSessionResult`,并通过 `BlueprintPure` 函数 `GetSessionResult()` 暴露给蓝图,供 Join Session 节点使用。
- `GameMainMenu.Build.cs` 增加 `OnlineSubsystem` 模块依赖(仅为获取 `FBlueprintSessionResult` 所需头文件,实际加入逻辑仍在蓝图侧)。

## Capabilities

### New Capabilities

- `session-entry-selection`: session 列表条目的选中回调与加入游戏所需 session 数据的暴露。

### Modified Capabilities

(无)

## Impact

- **代码**: `Widgets/Multiplayer/DataObjects/ListDataObject_Session.h`、`Widgets/Multiplayer/ListEntries/W_ListEntry_Session.h/.cpp`、`GameMainMenu.Build.cs`
- **依赖**: 新增 `OnlineSubsystemUtils` 模块依赖(仅头文件,用于 `FBlueprintSessionResult` / `FindSessionsCallbackProxy.h`;UE 5.7 中该类型已从 Engine 移至 OnlineSubsystemUtils 插件)
- **蓝图**: `WBP_ListEntry_Session` 需实现 `BP_OnSessionEntrySelected` 事件;搜索流程蓝图中构建 `FSessionListItem` 时需填充 `SessionResult`
