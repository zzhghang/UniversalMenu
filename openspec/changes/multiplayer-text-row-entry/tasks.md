## 1. Config 层

- [x] 1.1 `MenuConfigTypes.h`: `EMenuEntryValueType` 新增 `TextRow`
- [x] 1.2 `MenuConfigTypes.h`: 新增 `FTextRowColumnConfig` struct（ColumnKey, DefaultValue）
- [x] 1.3 `MenuEntryConfig.h`: 新增 TextRow 配置字段组（`TArray<FTextRowColumnConfig> Columns`, `float InterColumnPadding`, `float SidePadding`）

## 2. 数据对象

- [x] 2.1 新建 `ListDataObject_TextRow.h`：声明 `UListDataObject_TextRow`（`TArray<FText> ColumnValues`、`SetColumnValue`、`GetColumnValue`）
- [x] 2.2 新建 `ListDataObject_TextRow.cpp`：实现 `SetColumnValue`/`GetColumnValue`（索引边界检查）

## 3. 入口 Widget

- [x] 3.1 新建 `W_ListEntry_TextRow.h`：声明 `UW_ListEntry_TextRow`（BindWidget `RowBox` UHorizontalBox）
- [x] 3.2 新建 `W_ListEntry_TextRow.cpp`：`NativePreConstruct` 动态创建 `UTextBlock`，`OnOwningListDataObjectSet` 绑定数据，`OnOwningListDataObjectModified` 刷新文本

## 4. 工厂 + 注册

- [x] 4.1 `MenuConfigData.cpp`: `CreateEntryFromConfig` 新增 `TextRow` case
- [x] 4.2 `DA_DataListEntryMapping`：C++ 侧无需改动，运行时通过 hierarchy walk 匹配。映射条目在编辑器中添加到 DataAsset 实例。

## 5. 编译验证

- [ ] 5.1 编译项目确认无错误
