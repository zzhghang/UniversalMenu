## ADDED Requirements

### Requirement: 配置化行高
`UCreditsDataAsset` SHALL 提供 `LineHeight` 浮点属性，默认值为 `30.f`，用于 spacer 行高估算和间距计算。

#### Scenario: 使用自定义行高
- **WHEN** DataAsset 中 `LineHeight` 设为非默认值
- **THEN** spacer 行数按该值计算，间距随字体大小正确缩放

#### Scenario: 使用默认行高
- **WHEN** DataAsset 中 `LineHeight` 保持默认 30.f
- **THEN** 行为与原本完全一致
