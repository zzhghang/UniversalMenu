## ADDED Requirements

### Requirement: Tab 专属按钮动态显示
系统 SHALL 参考 OptionScreen 中 AddActionBinding / RemoveActionBinding 的动态绑定管理模式，根据当前选中的 Tab 动态显示对应的操作按钮：CreateGame Tab 显示"开始游戏"按钮，JoinGame Tab 显示"加入游戏"按钮。按钮的输入动作绑定 MUST 在 Tab 切换时切换。

#### Scenario: 切换到 CreateGame Tab
- **WHEN** 用户切换到 CreateGame Tab（包括首次进入 MultiplayerScreen 默认选中 CreateGame 时）
- **THEN** "开始游戏"按钮（StartGameAction 绑定）被 Add，"加入游戏"按钮被 Remove

#### Scenario: 切换到 JoinGame Tab
- **WHEN** 用户从 CreateGame Tab 切换到 JoinGame Tab
- **THEN** "加入游戏"按钮（JoinGameAction 绑定）被 Add，"开始游戏"按钮被 Remove

### Requirement: 按钮回调占位实现
系统 SHALL 在"开始游戏"和"加入游戏"按钮的回调中输出 Debug 日志，实际创建/加入逻辑留待后续实现。

#### Scenario: 用户点击开始游戏按钮
- **WHEN** 用户在 CreateGame Tab 中按下开始游戏按钮
- **THEN** 系统调用 Debug::Print 输出占位日志，不执行实际创建逻辑

#### Scenario: 用户点击加入游戏按钮
- **WHEN** 用户在 JoinGame Tab 中按下加入游戏按钮
- **THEN** 系统调用 Debug::Print 输出占位日志，不执行实际加入逻辑
