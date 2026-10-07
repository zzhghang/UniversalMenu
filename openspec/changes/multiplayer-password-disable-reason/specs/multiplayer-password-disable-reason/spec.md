## ADDED Requirements

### Requirement: Password 条目禁用时显示原因
当 `HasPassword` 为 `false` 时，`Password` 条目 SHALL 处于禁用状态并在详情面板中显示禁用原因富文本。

#### Scenario: HasPassword 关闭时 Password 显示禁用原因
- **WHEN** 用户在 MultiplayerScreen 中将 HasPassword 设为 Off
- **THEN** Password 条目变为不可编辑
- **AND** 详情面板的 DisabledReason 显示密码禁用提示文本（来自 `Warning_PasswordDisabled`）

#### Scenario: HasPassword 开启时 Password 可编辑
- **WHEN** 用户在 MultiplayerScreen 中将 HasPassword 设为 On
- **THEN** Password 条目可编辑
- **AND** 详情面板不显示密码禁用原因

### Requirement: 密码禁用原因的本地化
密码禁用原因的文本 SHALL 通过本地化字符串表获取，key 为 `Warning_PasswordDisabled`，资源表为 `ST_Multiplayer`。

#### Scenario: 切换语言后禁用原因跟随变化
- **WHEN** 游戏语言切换到非英语
- **AND** Password 条目处于禁用状态
- **THEN** 详情面板的 DisabledReason 显示对应语言的文本
