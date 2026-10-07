## ADDED Requirements

### Requirement: DataAsset 配置 TextRow 条目
系统 SHALL 支持在 `UMenuEntryConfig` 中配置 `TextRow` 类型条目，包含列定义数组、列间距、左右边距。

#### Scenario: 配置三列服务端信息行
- **WHEN** 设计器在 DataAsset 中创建一个 `TextRow` 条目，定义三列（ServerName、Players、Ping），列间距 20px，左右边距 10px
- **THEN** `CreateEntryFromConfig` SHALL 生成一个包含三列的 `UListDataObject_TextRow`

### Requirement: 运行时填充列值
`UListDataObject_TextRow` SHALL 提供 `SetColumnValue(int32, FText)` 方法在运行时按索引设置每列文本。

#### Scenario: 填充服务端数据
- **WHEN** 网络发现返回一台服务器信息（名称 "MyServer"、玩家数 3/16、Ping 42ms）
- **THEN** 调用 `SetColumnValue(0, "MyServer")`、`SetColumnValue(1, "3/16")`、`SetColumnValue(2, "42ms")` 后，对应索引的列文本 SHALL 被更新

#### Scenario: 获取列值
- **WHEN** 调用 `GetColumnValue(1)` 且第二列已设置为 "3/16"
- **THEN** SHALL 返回 `FText("3/16")`

### Requirement: 水平并排显示
`UW_ListEntry_TextRow` Widget SHALL 将各列文本水平并排显示，按配置的列间距和左右边距排列。

#### Scenario: 三列并排渲染
- **WHEN** TextRow 有三列文本 "ServerA"、"5/16"、"32ms"，列间距 20，左右边距 10
- **THEN** Widget SHALL 从左到右渲染：10px 边距 → "ServerA" → 20px 间距 → "5/16" → 20px 间距 → "32ms" → 10px 边距

#### Scenario: 空列占位
- **WHEN** 某列未设置文本（空字符串或默认值）
- **THEN** 该列 SHALL 仍然占据布局空间，只是不显示可见文本

### Requirement: Widget 注册到映射表
`UDA_DataListEntryMapping` SHALL 包含 `UListDataObject_TextRow` → `UW_ListEntry_TextRow` 的映射。

#### Scenario: 列表正确实例化 TextRow Widget
- **WHEN** `UFrontendCommonListView` 收到一个 `UListDataObject_TextRow` 类型的数据对象
- **THEN** SHALL 使用 `UW_ListEntry_TextRow` 作为其入口 Widget
