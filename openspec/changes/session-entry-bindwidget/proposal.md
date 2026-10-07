## Why

`W_ListEntry_Session` 的列文本控件目前在 C++ 中通过 `NewObject` 动态创建并循环填充到 `HorizontalBox`，Blueprint 中无法调整每列位置、样式和间距。设计师需要在 BP 中直接拖拽排列控件以便与 HeaderRow 对齐。

## What Changes

- `W_ListEntry_Session` 删除 `SessionTextStyle`、`ColumnGap`、`EdgePadding`、`VerticalPadding` 四个属性
- `W_ListEntry_Session` 新增 5 个 `BindWidget` CommonTextBlock：`Tb_ServerName`、`Tb_MapName`、`Tb_Players`、`Tb_Private`、`Tb_Ping`
- `OnOwningListDataObjectSet` 简化为直接对已绑定的 TextBlock 调 `SetText`，不再动态创建控件
- `WBP_ListEntry_Session` 蓝图需重新布局：放置 HorizontalBox + 5 个命名 TextBlock，在 Details 面板中分别调整样式和间距

## Capabilities

### New Capabilities

- `session-entry-blueprint-layout`: Session 列表项的列控件在蓝图中预置绑定，设计师可自由调整每列位置、样式和间距

### Modified Capabilities

<!-- 无现有 specs 需要修改 -->

## Impact

- **修改文件**: `W_ListEntry_Session.h`、`W_ListEntry_Session.cpp`
- **Blueprint**: `WBP_ListEntry_Session` 需重建布局
- **不影响**: `ListDataObject_Session`（纯数据）、`MultiplayerDataRegistry`（数据注入）、`DA_ListEntryMapping`（映射不变）
- **BREAKING**: 现有的 `WBP_ListEntry_Session` 蓝图布局失效，需重建
