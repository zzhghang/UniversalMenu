## ADDED Requirements

### Requirement: UMenuEntryConfig 支持 DisableRichReason 配置
`UMenuEntryConfig` DataAsset SHALL 提供 `DisableRichReason` 字段，允许在 DataAsset 中直接配置条目的禁用原因富文本。

#### Scenario: DataAsset 中配置了 DisableRichReason
- **WHEN** DataAsset 中某个条目的 `DisableRichReason` 字段不为空
- **AND** 该条目的 EditCondition 不满足
- **THEN** 详情面板显示该 DisableRichReason 文本

### Requirement: UMenuEntryConfig 支持 DynamicDetails 配置
`UMenuEntryConfig` DataAsset SHALL 提供 `DynamicDetails` 字段，允许在 DataAsset 中配置条目的动态详情富文本。

#### Scenario: DataAsset 中配置了 DynamicDetails
- **WHEN** DataAsset 中某个条目的 `DynamicDetails` 字段不为空
- **THEN** 详情面板的 DynamicDetails 区域始终显示该文本

#### Scenario: DataAsset 中未配置 DynamicDetails
- **WHEN** DataAsset 中条目的 `DynamicDetails` 字段为空
- **THEN** 详情面板的 DynamicDetails 区域为空

### Requirement: DetailView 显示 DynamicDetails
`UW_OptionDetailView` SHALL 从 DataObject 读取 DynamicDetails 文本并显示在 `Ctb_DynamicDetails` 富文本块中。

#### Scenario: 选中条目时更新 DynamicDetails
- **WHEN** 用户选中或悬停一个条目
- **THEN** `UpdateDetailsViewInfo` 从 DataObject 读取 DynamicDetailsRichText 并设置到 Ctb_DynamicDetails

#### Scenario: 清除详情时重置 DynamicDetails
- **WHEN** 详情面板清除显示
- **THEN** Ctb_DynamicDetails 文本设为空
