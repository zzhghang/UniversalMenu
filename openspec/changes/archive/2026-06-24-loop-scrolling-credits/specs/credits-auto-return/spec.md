## MODIFIED Requirements

### Requirement: 可配置的自动返回行为
系统 SHALL 提供 `bAutoReturnOnFinish` 布尔属性，默认值为 `true`，用于控制 Credits 滚动结束后是否自动返回上一界面。当循环滚动（`bLoopScrolling`）启用时，此属性无效——界面不会自动返回。

#### Scenario: 开启自动返回
- **WHEN** `bAutoReturnOnFinish` 为 `true`、`bLoopScrolling` 为 `false`，且 Credits 滚动到底部
- **THEN** 系统自动关闭 Credits 界面并返回上一界面

#### Scenario: 关闭自动返回
- **WHEN** `bAutoReturnOnFinish` 为 `false`、`bLoopScrolling` 为 `false`，且 Credits 滚动到底部
- **THEN** 系统停留在 Credits 界面，不自动返回

#### Scenario: 循环模式下忽略自动返回
- **WHEN** `bLoopScrolling` 为 `true`（无论 `bAutoReturnOnFinish` 为何值）
- **THEN** 系统循环滚动 Credits，不自动返回上一界面
