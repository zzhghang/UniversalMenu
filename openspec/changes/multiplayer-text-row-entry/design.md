## Context

当前 `EMenuEntryValueType` 支持 Bool/Scalar/String/Editable/Integer/Culture/Resolution 七种单值类型，均对应单个数据对象+单个入口 Widget。JoinGame 需要服务端浏览器行，一行并排展示 servername、players、ping 等多列文本。

遵循现有 DataAsset → DataObject → Widget 三层架构，新增 TextRow 类型。

## Goals / Non-Goals

**Goals:**
- 新增 `TextRow` 条目类型，一个 entry config 定义整个行（非每列独立配置）
- 列模板通过 DataAsset 配置：每列有一个 key/label，Widget 按顺序水平排列
- 运行时通过代码填充各列实际文本值
- 支持列间距（InterColumnPadding）和左右边距（SidePadding）

**Non-Goals:**
- 不实现可编辑列（此行只读）
- 不支持运行时动态增删列（列数由配置固定）
- 不包含排序、筛选等 server browser 功能

## Decisions

1. **新枚举值 + switch case，而非独立工厂**
   - 遵循 `CreateEntryFromConfig` 的现有 switch 模式
   - `EMenuEntryValueType` 新增 `TextRow`
   - `UMenuEntryConfig` 新增 `TextRowColumns` (TArray) + `TextRowInterColumnPadding` + `TextRowSidePadding`

2. **列定义使用 `FTextRowColumnConfig` struct**
   ```cpp
   USTRUCT()
   struct FTextRowColumnConfig
   {
       GENERATED_BODY()
       UPROPERTY(EditAnywhere) FString ColumnKey;
       UPROPERTY(EditAnywhere) FText ColumnLabel;   // 可选，配置时不显示，运行时占位
       UPROPERTY(EditAnywhere) FText DefaultValue;   // 运行时替换
   };
   ```

3. **数据对象 `UListDataObject_TextRow` 持有文本数组**
   - `TArray<FText> ColumnValues` — 与配置的列数对应，按索引匹配
   - 提供 `SetColumnValue(int32 Index, FText Value)` 运行时填充
   - 提供 `GetColumnValue(int32 Index)` 读取

4. **Widget 使用 `UHorizontalBox` + `UTextBlock`**
   - `NativePreConstruct` 时按配置创建 `UTextBlock` 子控件
   - 间距通过 `FMargin` / `Padding` 实现
   - 不依赖蓝图预设控件数量，全部代码动态生成

5. **Save/Load 不做特殊处理**
   - TextRow 条目本身是只读展示，不与用户交互
   - 数据由外部（网络发现）驱动，无需持久化到 MenuSession

## Risks / Trade-offs

- 列数由配置固定，运行时无法动态增减 → 服务端返回的字段数变化时需更新 DataAsset。缓解：列数通常稳定。
