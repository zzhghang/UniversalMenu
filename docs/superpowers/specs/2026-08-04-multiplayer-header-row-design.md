# Multiplayer JoinGame Tab — 多列 Header Row

**Date:** 2026-08-04
**Status:** approved

## 目标

在 Multiplayer 界面的 JoinGame tab 列表顶部，支持一个多列文本 header，格式如：

```
[ ServerName    Players    Ping    Status ]
```

列数、每列文本、列间距、两侧边距全部由 DataAsset 配置。

## 架构概览

```
UMenuCategoryConfig (DataAsset)
    │  HeaderColumns / HeaderColumnGap / HeaderEdgePadding
    ▼
MultiplayerDataRegistry::InitFromConfig
    │  创建 UListDataObject_HeaderRow 插入 category 子项列表头部
    ▼
UDA_DataListEntryMapping
    │  UListDataObject_HeaderRow → UW_ListEntry_HeaderRow
    ▼
UW_ListEntry_HeaderRow
    │  Hb_HeaderColumns (HorizontalBox)
    │  动态创建 UCommonTextBlock，Slot Padding 控制间距
    ▼
[ ServerName    Players    Ping    Status ]
```

## 改动明细

### 1. Config 层 — `MenuCategoryConfig.h`

`UMenuCategoryConfig` 新增三个字段：

```cpp
/** Multi-column header labels. When empty, the category behaves as before (single CategoryName). */
UPROPERTY(EditAnywhere, BlueprintReadOnly)
TArray<FText> HeaderColumns;

UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "!HeaderColumns.IsEmpty()"))
float HeaderColumnGap = 40.f;

UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "!HeaderColumns.IsEmpty()"))
float HeaderEdgePadding = 20.f;
```

### 2. 运行时数据 — 新文件 `ListDataObject_HeaderRow.h/.cpp`

继承 `UListDataObject_Base`。纯粹的数据容器，无交互逻辑：

```cpp
UCLASS()
class UListDataObject_HeaderRow : public UListDataObject_Base
{
    GENERATED_BODY()
public:
    void SetHeaderConfig(const TArray<FText>& InColumns, float InColumnGap, float InEdgePadding);

    const TArray<FText>& GetColumns() const { return Columns; }
    float GetColumnGap() const { return ColumnGap; }
    float GetEdgePadding() const { return EdgePadding; }

private:
    TArray<FText> Columns;
    float ColumnGap = 40.f;
    float EdgePadding = 20.f;
};
```

### 3. Registry — `MultiplayerDataRegistry.cpp`

`InitFromConfig` 和 `InitHardcodedFallback` 中，遍历 category 时：
- 如果 `CategoryConfig->HeaderColumns` 非空，创建一个 `UListDataObject_HeaderRow` 作为该 category 子项列表的第一个元素

### 4. Widget — 新文件 `W_ListEntry_HeaderRow.h/.cpp`

继承 `UW_ListEntry_Base`。

UMG 里只需一个空的 `HorizontalBox`（`Hb_HeaderColumns`）。`OnOwningListDataObjectSet` 时动态填充：

- `ClearChildren()`
- 遍历 `GetColumns()`，每个创建一个 `UCommonTextBlock`
- Slot Padding：第一列 `Left = EdgePadding`，最后一列 `Right = EdgePadding`，列之间 `Right = ColumnGap`

`OnToggleEditableState` 重写为空（header 永远不禁用）。

### 5. Widget 映射 — `DA_DataListEntryMapping` DataAsset

在 `.uasset` 上加一行映射：`UListDataObject_HeaderRow → UW_ListEntry_HeaderRow`

## Save/Load

`UListDataObject_HeaderRow` 不参与 session 保存/加载。`SaveSessionData` / `LoadSessionData` 遍历时会跳过 `HeaderRow` 类型（Cast 失败即 continue，无需特殊处理）。

## 向后兼容

- `HeaderColumns` 为空 → 行为和现有完全一致
- 现有 DataAsset 不受影响（新字段默认空数组）
