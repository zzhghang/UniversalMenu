## Context

`UFrontendDeveloperSettings` 和 `UFrontendLoadingScreenSettings` 是 GameMainMenu 插件内的两个 `UDeveloperSettings` 子类。两者当前使用 `Config = Game`，导致编辑器将配置序列化到主项目的 `Config/DefaultGame.ini` 中，而非插件自身的 `Config/DefaultGameMainMenu.ini`。

UE 的配置层级：`Config=<ModuleName>` 会将类配置保存到对应模块的 `Default<ModuleName>.ini`。GameMainMenu 模块已有 `Config/DefaultGameMainMenu.ini`（目前仅含 CoreRedirects）。

`UFrontendGameUserSettings` 不在此次变更范围内 —— 它继承 `UGameUserSettings`，用于用户级运行时设置，存储在 `Saved/Config/GameUserSettings.ini`，与开发设置是完全不同的系统。

## Goals / Non-Goals

**Goals:**
- 将 `FrontendDeveloperSettings` 和 `FrontendLoadingScreenSettings` 的配置从项目 `DefaultGame.ini` 迁移至插件 `DefaultGameMainMenu.ini`
- 保持所有配置值和运行时行为不变

**Non-Goals:**
- 不修改 `UFrontendGameUserSettings`（用户级设置，无需改动）
- 不移动 `DefaultGame.ini` 中属于其他系统的 section
- 不改变 GameMainMenu 模块的配置文件名或结构

## Decisions

**将 `Config = Game` 改为 `Config = GameMainMenu`**

这是 UE 标准做法：`Config` 值对应模块名，插件自己的 `UDeveloperSettings` 子类应使用插件模块名。运行时配置系统会根据模块名查找到插件 `Config/` 目录下的对应 `.ini` 文件。

**不引入 `CustomConfig` 属性**

两个类都只是 `Config` 归属问题，不使用特殊配置文件名。标准模块级配置文件即可。

## Risks / Trade-offs

- **配置键名变化风险**：低。`Config` 仅改变配置文件的搜索路径，`UCLASS` 名称和属性名不变，Section 头不变。已存在的 `DefaultGame.ini` 中的旧 section 需要在迁移后移除，否则两份配置会产生优先级歧义。
- **编辑器兼容性**：修改 `UCLASS` 宏后，编辑器下次打开项目设置会自动从新配置文件读取，之前通过 UI 保存的值需手动迁移到插件配置文件。

## Migration Plan

1. 修改两个 `.h` 文件中的 `Config` 值
2. 将 `DefaultGame.ini` 中的对应 section 内容移动到 `DefaultGameMainMenu.ini`
3. 从 `DefaultGame.ini` 中删除已迁移的 section
4. 在编辑器中打开项目设置验证配置可正确加载和保存
