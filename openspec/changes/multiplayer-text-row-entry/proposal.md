## Why

JoinGame tab 需要一个可展示服务器信息的横向多文本条目（类似 server browser row），以 DataAsset 方式配置列模板，运行时填充实际数据。当前系统仅支持单文本编辑、下拉选择、布尔开关等单值条目，无法在一行中并排显示 servername / players / ping 等多列信息。

## What Changes

- 新增 `UListDataObject_TextRow` 数据对象，持有一组列文本值
- 新增 `UW_ListEntry_TextRow` 入口 Widget，水平排列多个 `UTextBlock`，支持自定义列间距和左右边距
- `MenuEntryConfig` 增加 `TextRow` 类型及列模板配置（列标签、默认值）
- `CreateEntryFromConfig` 增加 TextRow 分支
- `DA_DataListEntryMapping` 注册新映射
- `W_MultiplayerScreen` 的 Save/Load 会话逻辑兼容 TextRow 类型

## Capabilities

### New Capabilities
- `text-row-entry`: 横向多文本并排条目，DataAsset 驱动列模板定义，运行时填充列值，支持列间距/边距配置

### Modified Capabilities
无

## Impact

- `MenuConfigTypes.h` — `EMenuEntryValueType` 新增 `TextRow`
- `MenuEntryConfig.h` — 新增 TextRow 配置字段组（列定义数组、间距、边距）
- `MenuConfigData.cpp` — `CreateEntryFromConfig` 新增 TextRow case
- `ListDataObject_TextRow.h/.cpp` — 新文件
- `W_ListEntry_TextRow.h/.cpp` — 新文件
- `DA_DataListEntryMapping` — 新增映射条目
- `W_MultiplayerScreen.cpp` — Save/Load 兼容 TextRow
