## ADDED Requirements

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
