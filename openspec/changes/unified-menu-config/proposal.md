## Why

Options 和 Multiplayer 两套菜单系统各自定义了一套 Tab → Category → Entry 三级配置 DataAsset 类，其中 Tab 和 Category 层字段完全一致，Entry 层核心模式相同仅字段有语义差异。重复代码增加维护负担，新增选项需要在两套类中各改一遍。现在两套系统刚完成基础搭建，是合并的最佳时机。

## What Changes

- 新建统一的 `UMenuTabConfig`、`UMenuCategoryConfig`、`UMenuEntryConfig`、`UMenuConfigData` 四个 DataAsset 类，替代现有的 Options 和 Multiplayer 各自的配置类
- `UMenuEntryConfig` 统一四种 ValueType（Bool/Scalar/String/Editable），覆盖 Options 和 Multiplayer 的所有条目类型
- 引入 BindingKey 机制：Entry 不再绑死 `EGameUserSettingFunction` 枚举，改为可选的 `FName BindingKey`，各 Registry 内部注册 getter/setter 解析逻辑
- **BREAKING**: 移除 `UOptionTabData`、`UOptionCategoryData`、`UOptionEntryData`、`UOptionsConfigData`、`UMultiplayerTabConfig`、`UMultiplayerCategoryConfig`、`UMultiplayerEntryConfig`、`UMultiplayerConfigData` 八个旧 DataAsset 类
- **BREAKING**: 旧 .uasset 配置资产需要重建

## Capabilities

### New Capabilities
- `unified-menu-data-assets`: 统一的 Tab/Category/Entry/ConfigData DataAsset 类，Options 和 Multiplayer 共用同一套类型但使用各自的数据实例
- `binding-key-mechanism`: FName-based getter/setter 注册与分发机制，替代 EGameUserSettingFunction 硬编码枚举，各 Registry 独立维护自己的 binding 映射表

### Modified Capabilities
<!-- 无现有 spec 的需求变更 -->

## Impact

- 受影响的源码：`GameMainMenu` 插件中所有 Options 和 Multiplayer 的 DataAsset 头文件/实现文件、两个 Registry 的 InitFromDataAsset/InitFromConfig 方法、`LocalizationKeys.h` 中的相关引用
- 受影响的资产：Options 和 Multiplayer 的 .uasset 配置 DataAsset 需要在编辑器中重建
- UI Widget 层、运行时 DataObject 体系、联动编辑条件逻辑保持不变
