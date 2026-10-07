## 1. Header 声明

- [x] 1.1 在 `W_MultiplayerScreen.h` 中新增 `StartGameAction` / `JoinGameAction` 两个 `FDataTableRowHandle` 属性和对应 `FUIActionBindingHandle`
- [x] 1.2 声明回调方法：`OnStartGameActionTriggered`、`OnJoinGameActionTriggered`

## 2. 输入动作绑定注册

- [x] 2.1 在 `NativeOnInitialized` 中参考 OptionScreen 的 `RegisterUIActionBinding` 模式注册 `StartGameAction` 和 `JoinGameAction`
- [x] 2.2 实现 `OnStartGameActionTriggered`：调用 `Debug::Print` 输出占位日志
- [x] 2.3 实现 `OnJoinGameActionTriggered`：调用 `Debug::Print` 输出占位日志

## 3. Tab 切换时动态切换绑定

- [x] 3.1 修改 `OnMultiplayerTabSelected`：根据 TabId 参考 OptionScreen 的 `AddActionBinding` / `RemoveActionBinding` 模式切换按钮绑定
- [x] 3.2 CreateGame Tab：Add `StartGameActionBinding`，Remove `JoinGameActionBinding`
- [x] 3.3 JoinGame Tab：Add `JoinGameActionBinding`，Remove `StartGameActionBinding`
