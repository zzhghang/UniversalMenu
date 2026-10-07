## Why

OptionScreen 通过输入动作绑定（`FDataTableRowHandle` + `RegisterUIActionBinding`）实现了 reset entry / reset category 功能，并通过 `AddActionBinding` / `RemoveActionBinding` 动态管理按钮可见性。MultiplayerScreen 有两个 Tab（CreateGame / JoinGame），需要在 Tab 切换时显示不同的操作按钮（开始游戏 / 加入游戏），参考 OptionScreen 的动态绑定管理模式。

## What Changes

- 参考 OptionScreen 中 reset entry 和 reset category 的动态绑定模式（`RegisterUIActionBinding` + `AddActionBinding`/`RemoveActionBinding`），在 MultiplayerScreen 中新增两个 Tab 专属的输入动作绑定
- CreateGame Tab：显示"开始游戏"按钮，回调暂用 Debug::Print 占位
- JoinGame Tab：显示"加入游戏"按钮，回调暂用 Debug::Print 占位
- Tab 切换时动态切换显示对应的动作绑定（Remove 旧 Tab 的绑定，Add 新 Tab 的绑定）

## Capabilities

### New Capabilities
- `multiplayer-tab-action-buttons`: Tab 切换时动态显示对应按钮（CreateGame → 开始游戏，JoinGame → 加入游戏），按钮逻辑暂用 Debug::Print 占位，绑定管理参考 OptionScreen 的 AddActionBinding/RemoveActionBinding 模式

### Modified Capabilities
<!-- None -->

## Impact

- `W_MultiplayerScreen.h/.cpp` — 新增两个输入动作绑定属性、回调方法、Tab 切换时按钮切换逻辑
- 无新增依赖，无 API 变更
