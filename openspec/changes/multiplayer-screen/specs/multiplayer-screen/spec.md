## ADDED Requirements

### Requirement: 多人游戏界面 Tab 导航
系统 SHALL 在多人游戏界面提供两个 Tab："创建游戏"（CreateGame）和"加入游戏"（JoinGame），用户可通过 Tab 切换不同功能区域。

#### Scenario: 默认选中创建游戏 Tab
- **WHEN** 多人游戏界面首次激活
- **THEN** 默认选中"创建游戏" Tab 并显示对应内容

#### Scenario: 切换到加入游戏 Tab
- **WHEN** 用户点击"加入游戏" Tab
- **THEN** 界面切换到加入游戏相关内容，先前 Tab 内容被替换

### Requirement: 返回导航
系统 SHALL 支持通过返回操作（Back Action）关闭多人游戏界面，返回上一层界面。

#### Scenario: 按返回键关闭界面
- **WHEN** 用户触发返回操作（ESC 或手柄 B 键）
- **THEN** 多人游戏界面被 Deactivate，返回上一层界面

### Requirement: 本地化支持
系统 SHALL 通过 StringTable 对所有界面文本进行本地化，包括 Tab 名称、输入字段标签、按钮文本等。

#### Scenario: Tab 名称显示本地化文本
- **WHEN** 界面在不同语言环境下激活
- **THEN** Tab 名称显示对应语言的本地化文本

#### Scenario: 界面文本从 StringTable 读取
- **WHEN** 界面加载任何文本内容
- **THEN** 文本通过 LOCTABLE 宏从对应 StringTable 获取

### Requirement: 蓝图 Widget 绑定
系统 SHALL 通过 BindWidget 元数据将 Tab 列表控件绑定到蓝图中的对应 Widget，允许蓝图层配置具体的 UI 布局和样式。

#### Scenario: Tab 列表 Widget 自动绑定
- **WHEN** 蓝图在编辑器中编译
- **THEN** 蓝图中名为 TabListWidget_MultiplayerTabs 的控件自动绑定到 C++ 对应成员
