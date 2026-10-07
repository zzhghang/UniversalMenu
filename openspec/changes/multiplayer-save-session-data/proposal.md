## Why

当前 MultiplayerScreen 用户在 Create Game / Join Game 面板中输入的数据（服务器名、密码、人数等）在界面切换后即丢失，且 `OnStartGameActionTriggered` / `OnJoinGameActionTriggered` 仍是空壳。需要提供持久化机制保存这些会话数据，供后续关卡使用。

## What Changes

- 新增 `UMenuSessionData : USaveGame` 类，用通用 `TMap<FName, FString>` 按 DataId 存储所有可编辑字段的值，适配 DataAsset 动态配置的字段增删
- 在 `W_MultiplayerScreen` 中实现 Save/Load：从 Registry 遍历所有条目，自动收集 `UListDataObject_Editable` 等类型的数据并持久化
- 在 `NativeOnActivated` 时自动加载上次保存的数据回填字段
- 在 `OnStartGameActionTriggered` / `OnJoinGameActionTriggered` 时保存当前数据到 SaveGame slot

## Capabilities

### New Capabilities
- `multiplayer-session-persistence`: 通过 USaveGame + 通用键值 Map 实现 MultiplayerScreen 输入数据的跨会话持久化和关卡间传递

### Modified Capabilities
<!-- 无现有 spec 需求变更 -->

## Impact

- 新增文件: `MultiplayerSessionData.h`
- 修改文件: `W_MultiplayerScreen.h/.cpp` — 增加 Save/Load 调用
- 依赖: `UGameplayStatics::SaveGameToSlot` / `LoadGameFromSlot`（引擎内置 API）
- 无破坏性变更
