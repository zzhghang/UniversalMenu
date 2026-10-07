## ADDED Requirements

### Requirement: Category 支持多列 Header Row

系统 SHALL 允许 `UMenuCategoryConfig` 配置一个多列文本 header。当 `HeaderColumns` 为空时，category 表现与现有行为一致（仅显示 `CategoryName`）。

#### Scenario: 配置了 HeaderColumns 的 category 显示多列 header
- **WHEN** 一个 Category 的 `HeaderColumns` 为非空数组 `[ServerName, Players, Ping, Status]`
- **THEN** 该 category 在列表中的头部渲染为一个水平排列的多列文本行，各列文本按顺序显示

#### Scenario: 未配置 HeaderColumns 的 category 保持原有行为
- **WHEN** 一个 Category 的 `HeaderColumns` 为空
- **THEN** 该 category 的显示与现有逻辑完全一致，无任何变化

### Requirement: Header 列间距可配置

系统 SHALL 支持通过 DataAsset 配置列之间的间距（`HeaderColumnGap`）和两侧边距（`HeaderEdgePadding`）。

#### Scenario: 列间距和边距生效
- **WHEN** 配置 `HeaderColumnGap = 40` 且 `HeaderEdgePadding = 20`
- **THEN** 第一列左侧有 20px 边距，最后一列右侧有 20px 边距，相邻列之间有 40px 间距

### Requirement: Header Row 不参与交互

`UListDataObject_HeaderRow` SHALL 为纯展示型数据对象，不响应焦点、点击和编辑状态变化。

#### Scenario: Header row 始终可用
- **WHEN** 列表滚动、焦点移动、编辑状态切换
- **THEN** header row 保持可见且不受 `OnToggleEditableState` 影响

### Requirement: Header Row 不参与 Session 持久化

`UListDataObject_HeaderRow` SHALL 被 `SaveSessionData` 和 `LoadSessionData` 自动跳过，不写入或读取存档。

#### Scenario: Save/Load 跳过 HeaderRow
- **WHEN** `SaveSessionData` 或 `LoadSessionData` 遍历列表项
- **THEN** `UListDataObject_HeaderRow` 类型在 Cast 检查时被跳过，不影响存档内容
