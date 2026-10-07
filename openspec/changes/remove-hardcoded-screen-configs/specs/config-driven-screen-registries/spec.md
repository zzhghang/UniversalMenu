## ADDED Requirements

### Requirement: 注册表仅从配置资产构建

`UOptionsDataRegistry` 与 `UMultiplayerDataRegistry` SHALL 仅从 `UMenuConfigData` 配置资产构建菜单内容,不得包含硬编码的 Tab/条目回退实现。

#### Scenario: 配置有效
- **WHEN** 注册表初始化且配置资产有效(非空且含 Tabs)
- **THEN** 菜单 Tab、分类、条目全部由配置资产驱动构建,与硬编码路径删除前的配置路径行为一致

#### Scenario: 配置缺失或为空
- **WHEN** 注册表初始化但配置资产为空、未指定或 Tabs 为空
- **THEN** 输出 `ensure` 与错误日志,注册表保持为空,屏幕显示空菜单,不构建任何硬编码内容,不崩溃

### Requirement: Multiplayer 屏幕 Tab 语义由配置声明

`UMenuTabConfig` SHALL 提供 Tab 角色字段(None/CreateGame/JoinGame),`UW_MultiplayerScreen` 的动作绑定与初始 Tab 选择 SHALL 基于配置声明的角色,不得使用代码中的 Tab ID 字面量。

#### Scenario: 选中 CreateGame 角色 Tab
- **WHEN** 用户选中配置中角色为 CreateGame 的 Tab
- **THEN** 绑定 StartGame 动作并执行创建游戏 Tab 的选中处理,移除 JoinGame/Refresh 动作绑定

#### Scenario: 选中 JoinGame 角色 Tab
- **WHEN** 用户选中配置中角色为 JoinGame 的 Tab
- **THEN** 绑定 JoinGame 与 Refresh 动作并执行加入游戏 Tab 的选中处理,移除 StartGame 动作绑定

#### Scenario: 初始 Tab 选择
- **WHEN** MultiplayerScreen 完成 Tab 注册后
- **THEN** 选中配置 Tabs 数组中的第一个 Tab

#### Scenario: 无角色声明
- **WHEN** 用户选中角色为 None 的 Tab
- **THEN** 不绑定 StartGame/JoinGame/Refresh 动作,仅切换列表内容

### Requirement: 引擎默认选项可被配置覆盖

`CreateEntryFromConfig()` 对窗口模式(FullscreenMode)与画质档位(质量/可缩放)条目 SHALL 优先使用配置中提供的 `AvailableOptions`/`AvailableIntegerOptions`;配置未提供时沿用引擎默认选项列表。

#### Scenario: 配置提供选项列表
- **WHEN** FullscreenMode 条目在配置中提供了 `AvailableOptions`
- **THEN** 该条目使用配置提供的选项,而非默认的三种窗口模式

#### Scenario: 配置未提供选项列表
- **WHEN** FullscreenMode 或画质条目在配置中未提供选项列表
- **THEN** 沿用引擎默认列表(窗口模式:Fullscreen/Windowed Fullscreen/Windowed;画质:Low 至 Cinematic 五档),现有资产行为不变
