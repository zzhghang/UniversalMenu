## Context

`W_ListEntry_Session` 当前在 `OnOwningListDataObjectSet` 中动态创建 5 个 `UCommonTextBlock` 并以循环填充到 `Hb_SessionColumns` HorizontalBox。列间距和边距通过 `EditDefaultsOnly` 属性控制。设计师无法在 Blueprint 中直接调整每列的位置、样式。

`W_ListEntry_HeaderRow` 也是动态创建模式——但它不需要设计师干预，因为列配置完全由 DataAsset 驱动。

## Goals / Non-Goals

**Goals:**
- 5 个列文本控件在 BP 中预置为命名 Child，设计师自由调整 Slot Padding 和 Text Style
- C++ 只负责赋值文本，不负责创建和布局
- 列文本格式逻辑不变（Players → "C/M"、Ping → "Nms"、Private → Yes/No）

**Non-Goals:**
- 不改动 `UListDataObject_Session`（纯数据对象）
- 不改动 `MultiplayerDataRegistry::RebuildSessionList`
- 不改动 HeaderRow（保持动态创建模式）
- 不增加运行时列宽配置

## Decisions

### 1. 绑定方式: `BindWidget` 而非 `BindWidgetOptional`

5 个 TextBlock 是 Session 行的必需组件，缺一不可。用 `BindWidget` 强制 Blueprint 提供，缺失时报错，避免静默失败。

### 2. `Hb_SessionColumns` 保留

HorizontalBox 仍作为容器保留，设计师在 BP 中排列 5 个 TextBlock。如果设计师想改用其他布局（如 Grid），需重写布局——但保留 `Hb_SessionColumns` BindWidget 兼容当前结构。

### 3. 删除所有动态布局属性

`SessionTextStyle`、`ColumnGap`、`EdgePadding`、`VerticalPadding` 全部删除。列间距和文字样式改为在 BP 的 Details 面板中直接在 Slot / Style 属性上设置。

### 4. `UListDataObject_Session` 无需改动

数据对象只持有 Session 数据，不涉及 UI 布局。从动态创建改为 BindWidget 不影响它。

## Risks / Trade-offs

- **BP 布局与 HeaderRow 不对齐** → 设计师需手动调 Slot Padding 使列对齐。目前 HeaderRow 也是固定 padding 值。
- **WBP_ListEntry_Session 需重建** → 现有 BP 引用的动态布局属性（SessionTextStyle 等）消失，蓝图报错。删除旧引用、重新放入 5 个命名 TextBlock 即可。
