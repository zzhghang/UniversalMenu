## Context

OptionScreen 通过 `FDataTableRowHandle` 绑定输入动作，在 `NativeOnInitialized` 中用 `RegisterUIActionBinding` 注册，在 Tab 切换或数据变更时通过 `AddActionBinding` / `RemoveActionBinding` 动态管理按钮可见性。MultiplayerScreen 目前只有 `BackActionHandle`，缺少 Tab 专属操作按钮。

当前 MultiplayerScreen 有两个 Tab：`CreateGame` 和 `JoinGame`，Tab 切换在 `OnMultiplayerTabSelected` 中处理。

## Goals / Non-Goals

**Goals:**
- 参考 OptionScreen 的动态绑定管理方式，为 MultiplayerScreen 增加两个 Tab 专属按钮
- CreateGame Tab 显示"开始游戏"按钮，JoinGame Tab 显示"加入游戏"按钮
- Tab 切换时自动切换按钮可见性
- 按钮回调暂用 Debug::Print 占位

**Non-Goals:**
- 不新增 reset entry / reset category 功能
- 不实现实际的创建游戏/加入游戏逻辑
- 不修改 OptionScreen 现有代码

## Decisions

1. **参考 OptionScreen 的绑定模式但不复制 reset 逻辑**：OptionScreen 中 `RegisterUIActionBinding` 注册 + `AddActionBinding`/`RemoveActionBinding` 动态切换的模式是通用的。MultiplayerScreen 直接使用该模式管理 Tab 专属按钮，但不引入 `ResettableDataArray` 等 reset 相关逻辑。

2. **在 `OnMultiplayerTabSelected` 中集中切换绑定**：Tab 切换到 `CreateGame` 时 Add `StartGameAction` 并 Remove `JoinGameAction`，反之亦然。选择原因：Tab 切换是唯一的状态变更点，逻辑集中在一个函数内。

3. **按钮回调用 `FrontendDebugHelper::Print` 占位**：创建/加入的实际逻辑后续实现，当前用 Debug::Print 标记入口点。

## Risks / Trade-offs

- [Risk] 新增 2 个 `FDataTableRowHandle` 属性需要在 Blueprint 中配置，若忘记配置则对应按钮不生效 → 代码中用 `IsNull()` 检查跳过绑定，不会崩溃
