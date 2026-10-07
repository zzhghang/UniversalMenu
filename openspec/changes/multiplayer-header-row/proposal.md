## Why

JoinGame 列表的每一列（ServerName、Players、Ping 等）目前没有列头标注，用户无法一目了然地知道每列数据的含义。需要一个可配置的多列文本 header，列数和间距由 DataAsset 驱动，方便后续不同场景复用。

## What Changes

- `UMenuCategoryConfig` 新增 `HeaderColumns`、`HeaderColumnGap`、`HeaderEdgePadding` 字段
- 新增 `UListDataObject_HeaderRow` 数据对象，承载 header 的列文本和间距配置
- `MultiplayerDataRegistry` 构建列表时，按配置在 category 子项头部插入 HeaderRow
- 新增 `UW_ListEntry_HeaderRow` widget，用 HorizontalBox + 动态 TextBlock 渲染多列 header
- `DA_DataListEntryMapping` 添加 HeaderRow → HeaderRow Widget 的映射

## Capabilities

### New Capabilities

- `multiplayer-header-row`: Category 下支持多列文本 header row，列数、列文本、间距由 DataAsset 配置

### Modified Capabilities

<!-- 无现有 spec 受影响 -->

## Impact

| 文件 | 影响 |
|---|---|
| `MenuCategoryConfig.h` | 新增 3 个 UPROPERTY |
| 新文件 `ListDataObject_HeaderRow.h/.cpp` | 新增数据对象类 |
| `MultiplayerDataRegistry.h/.cpp` | 构建逻辑增加 header 插入 |
| 新文件 `W_ListEntry_HeaderRow.h/.cpp` | 新增 widget 类 |
| `DA_DataListEntryMapping` (.uasset) | 新增一条类型映射 |
