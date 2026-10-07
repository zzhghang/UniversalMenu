## MODIFIED Requirements

### Requirement: 插件设置使用插件自有配置文件
`UFrontendDeveloperSettings` 和 `UFrontendLoadingScreenSettings` 的配置 SHALL 保存在 GameMainMenu 插件的 `Config/DefaultGameMainMenu.ini` 中，而非项目 `Config/DefaultGame.ini`。

#### Scenario: 编辑器保存设置
- **WHEN** 在项目设置中修改 Frontend UI Settings 或 Loading Screen Settings
- **THEN** 变更写入 `Plugins/GameMainMenu/Config/DefaultGameMainMenu.ini`

#### Scenario: 编辑器加载设置
- **WHEN** 编辑器打开项目
- **THEN** 从 `Plugins/GameMainMenu/Config/DefaultGameMainMenu.ini` 读取 Frontend UI Settings 和 Loading Screen Settings

### Requirement: GameUserSettings 不受影响
`UFrontendGameUserSettings` 的配置 SHALL 继续使用现有配置系统，不做任何修改。

#### Scenario: GameUserSettings 保持原有行为
- **WHEN** 运行时读写用户设置
- **THEN** 行为与迁移前完全一致，配置存储位置不变

## ADDED Requirements

### Requirement: DataAsset 驱动的 Options 条目可读写设置
通过 `UMenuConfigData` DataAsset 创建的 Options 条目 SHALL 根据其 `BindingKey` 自动绑定到 `UFrontendGameUserSettings` 的对应 getter/setter。

#### Scenario: Bool 条目绑定
- **WHEN** 从 DataAsset 创建一个 `BindingKey=ShowHealthBar`、`ValueType=Bool` 的条目
- **THEN** 该条目的 `DataDynamicGetter` 和 `DataDynamicSetter` 正确指向 `GetShowHealthBar`/`SetShowHealthBar`

#### Scenario: Scalar 条目绑定
- **WHEN** 从 DataAsset 创建一个 `BindingKey=OverallVolume`、`ValueType=Scalar` 的条目
- **THEN** 该条目的 `DataDynamicGetter` 和 `DataDynamicSetter` 正确指向 `GetOverallVolume`/`SetOverallVolume`

#### Scenario: 无 BindingKey 的条目不绑定
- **WHEN** 从 DataAsset 创建一个 `BindingKey=None` 的条目
- **THEN** 该条目不设置 `DataDynamicGetter`/`DataDynamicSetter`，退化为纯 UI 数据

#### Scenario: 无效 BindingKey 不崩溃
- **WHEN** 从 DataAsset 创建一个 `BindingKey=InvalidKey` 的条目
- **THEN** `ResolveBinding` 返回 nullptr，条目无 getter/setter 但不崩溃

### Requirement: FullscreenMode 条目创建为 StringEnum
`CreateEntryFromConfig` 函数 SHALL 在遇到 `BindingKey=FullscreenMode` 的条目时，创建 `UListDataObject_StringEnum` 并自动填入全屏/窗口化全屏/窗口化三个选项。

#### Scenario: FullscreenMode 工厂分发
- **WHEN** 处理 `ValueType=String`、`BindingKey=FullscreenMode` 的 DataAsset 条目
- **THEN** 创建一个 `UListDataObject_StringEnum`，包含 Fullscreen(0)、WindowedFullscreen(1)、Windowed(2) 三个枚举选项

#### Scenario: FullscreenMode 默认值
- **WHEN** FullscreenMode 条目创建完成
- **THEN** 默认值设置为 WindowedFullscreen(1)

### Requirement: Multiplayer DataAsset 使用共享工厂
`MultiplayerDataRegistry::InitFromConfig` SHALL 使用 `CreateEntryFromConfig` 创建所有条目类型，而非自定义 switch。

#### Scenario: Multiplayer String 条目创建
- **WHEN** 处理 `ValueType=String`、带 `AvailableOptions` 的 Multiplayer DataAsset 条目（如 Map、GameMode）
- **THEN** 创建 `UListDataObject_StringEnum` 并正确填入下拉选项

#### Scenario: Multiplayer Integer 条目创建
- **WHEN** 处理 `ValueType=String`、`BindingKey` 匹配质量预设的 Multiplayer DataAsset 条目（如 MaxPlayers）
- **THEN** 创建正确的 `UListDataObject_StringInteger`（或由 AvailableOptions 驱动的 StringEnum）

#### Scenario: Multiplayer Bool/Editable 条目不受影响
- **WHEN** 处理 `ValueType=Bool` 或 `ValueType=Editable` 的条目
- **THEN** 行为与改前一致

### Requirement: Resolution 依赖 WindowMode 状态
当 WindowMode 改变时，Resolution 条目的可编辑状态 SHALL 同步更新；且 Resolution 的 `EditDependencyData` 列表 SHALL 包含 WindowMode 条目引用。

#### Scenario: WindowMode 为窗口化全屏时 Resolution 禁用
- **WHEN** 用户将 WindowMode 切换为 WindowedFullscreen
- **THEN** Resolution 条目变为不可编辑，显示禁用原因，当前值强制设置为最大分辨率

#### Scenario: WindowMode 为非窗口化全屏时 Resolution 启用
- **WHEN** 用户将 WindowMode 从 WindowedFullscreen 切换为 Fullscreen 或 Windowed
- **THEN** Resolution 条目恢复可编辑状态

#### Scenario: Resolution 的 EditDependencyData 包含 WindowMode
- **WHEN** Video Tab 初始化完成
- **THEN** Resolution 条目的 EditDependencyData 列表包含 WindowMode 条目引用
