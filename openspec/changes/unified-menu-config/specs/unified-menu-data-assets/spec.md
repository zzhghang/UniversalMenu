## ADDED Requirements

### Requirement: 统一 Tab 配置 DataAsset
系统 SHALL 提供 `UMenuTabConfig` DataAsset 类，包含 `TabId`（FName）、`TabName`（FText）和 `Categories`（TArray<UMenuCategoryConfig*>）三个属性，替代 `UOptionTabData` 和 `UMultiplayerTabConfig`。

#### Scenario: Options 系统使用 UMenuTabConfig
- **WHEN** Options 配置 DataAsset 引用 Tabs 数组
- **THEN** 每个元素为 `UMenuTabConfig` 实例，包含 Options 相关的 Tab 定义

#### Scenario: Multiplayer 系统使用 UMenuTabConfig
- **WHEN** Multiplayer 配置 DataAsset 引用 Tabs 数组
- **THEN** 每个元素为 `UMenuTabConfig` 实例，包含 Multiplayer 相关的 Tab 定义

### Requirement: 统一 Category 配置 DataAsset
系统 SHALL 提供 `UMenuCategoryConfig` DataAsset 类，包含 `CategoryId`（FName）、`CategoryName`（FText）和 `Entries`（TArray<UMenuEntryConfig*>）三个属性，替代 `UOptionCategoryData` 和 `UMultiplayerCategoryConfig`。

#### Scenario: Category 包含多个 Entry
- **WHEN** `UMenuCategoryConfig` 的 `Entries` 数组包含多个 `UMenuEntryConfig` 实例
- **THEN** 这些 Entry 在 UI 中作为同一分类下的设置项展示

### Requirement: 统一 Entry 配置 DataAsset
系统 SHALL 提供 `UMenuEntryConfig` DataAsset 类，使用 `EMenuEntryValueType` 枚举（Bool/Scalar/String/Editable）标记值类型，并通过 `EditCondition` 元数据使不同 ValueType 相关的字段条件可见。

#### Scenario: Bool 类型 Entry
- **WHEN** Entry 的 `ValueType` 设置为 `Bool`
- **THEN** 仅显示通用字段（DataId/DisplayName/Description/ValueType/BindingKey/bApplyImmediately/bPackagedBuildOnly/EditDependencyIds）和 `bDefaultBool` 字段

#### Scenario: Scalar 类型 Entry
- **WHEN** Entry 的 `ValueType` 设置为 `Scalar`
- **THEN** 额外显示 ScalarMin/ScalarMax/ScalarOutMin/ScalarOutMax/StepSize/NumericType 字段

#### Scenario: String 类型 Entry
- **WHEN** Entry 的 `ValueType` 设置为 `String`
- **THEN** 额外显示 `AvailableOptions` Map 字段用于配置下拉选项

#### Scenario: Editable 类型 Entry
- **WHEN** Entry 的 `ValueType` 设置为 `Editable`
- **THEN** 额外显示 MaxCharCount/bIsPassword/HintText/CharFilter 字段

#### Scenario: 通用字段在所有类型可见
- **WHEN** Entry 的 `ValueType` 为任意值
- **THEN** DataId/DisplayName/Description/ValueType/BindingKey/bApplyImmediately/bPackagedBuildOnly/EditDependencyIds 字段始终可见且可编辑

### Requirement: 统一顶层 ConfigData DataAsset
系统 SHALL 提供 `UMenuConfigData` DataAsset 类，包含 `Tabs`（TArray<UMenuTabConfig*>）属性，替代 `UOptionsConfigData` 和 `UMultiplayerConfigData`。Options 和 Multiplayer 各自创建独立的 DataAsset 实例，引用相同的类但填入不同的配置数据。

#### Scenario: Options 使用 UMenuConfigData
- **WHEN** `W_OptionScreen` 的 `OptionsConfig` 属性引用一个 `UMenuConfigData` 实例
- **THEN** 该实例包含 Options 相关的 Tabs/Categories/Entries 数据，Registry 遍历时创建对应的运行时 DataObject 层级

#### Scenario: Multiplayer 使用 UMenuConfigData
- **WHEN** `UMultiplayerDataRegistry` 的 `Config` 属性引用一个 `UMenuConfigData` 实例
- **THEN** 该实例包含 Multiplayer 相关的 Tabs/Categories/Entries 数据，Registry 遍历时创建对应的运行时 DataObject 层级

### Requirement: 旧 DataAsset 类移除
编译单元 SHALL 不再包含 `UOptionTabData`、`UOptionCategoryData`、`UOptionEntryData`、`UOptionsConfigData`、`UMultiplayerTabConfig`、`UMultiplayerCategoryConfig`、`UMultiplayerEntryConfig`、`UMultiplayerConfigData` 八个类的定义。

#### Scenario: 编译不包含旧类头文件
- **WHEN** 项目完成迁移后编译
- **THEN** 删除的旧类文件不再存在，所有引用已替换为新类名
