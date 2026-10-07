## ADDED Requirements

### Requirement: 多个 DisableRichReason 以换行分隔
当一个条目同时满足多个禁用条件时，系统 SHALL 用换行符分隔每个 DisableRichReason 文本。

#### Scenario: 条目有两个禁用条件均生效
- **WHEN** 条目的两个 EditCondition 均不满足（如 PackagedBuildOnly + WindowMode 条件）
- **AND** 每个 EditCondition 都设置了 DisableRichReason
- **THEN** 详情面板 DisabledReason 显示两个原因文本，中间以换行符分隔

#### Scenario: 条目只有一个禁用条件生效
- **WHEN** 条目的两个 EditCondition 中只有一个不满足
- **THEN** 详情面板 DisabledReason 仅显示该条件的 DisableRichReason，无多余换行

#### Scenario: 条目无禁用条件
- **WHEN** 条目没有 EditCondition 或所有 EditCondition 均满足
- **THEN** DisabledReason 显示为空

### Requirement: DynamicDetails 支持多行
当条目需要显示多层 DynamicDetails 时，系统 SHALL 以相同逻辑用换行符分隔。

#### Scenario: DynamicDetails 多层显示
- **WHEN** DataObject 设置了 DynamicDetails 文本且包含多个信息段
- **THEN** 详情面板的 DynamicDetails 区域以换行分隔显示
