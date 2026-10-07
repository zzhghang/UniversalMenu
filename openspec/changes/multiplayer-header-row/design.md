## Context

当前 `UMenuCategoryConfig` 只支持单一 `CategoryName` 作为列表中的分类头。在 JoinGame 的服务器列表场景中，需要多列 header（ServerName、Players、Ping 等）来标注各列含义。

约束：必须与现有 DataAsset → Registry → ListView 架构一致，不引入新依赖，不影响现有 behavior。

## Goals / Non-Goals

**Goals:**
- Category 可选地显示多列文本 header，替代单一 `CategoryName`
- 列数、列文本、列间距、两侧边距由 DataAsset 配置

**Non-Goals:**
- 列头不参与焦点导航、不响应点击
- 不做每列独立宽度（只用 Slot Padding 控制间距）
- 不参与 session save/load

## Decisions

| 决策 | 选择 | 理由 |
|---|---|---|
| 数据对象 | 新建 `UListDataObject_HeaderRow` 继承 `UListDataObject_Base` | 不污染 `UListDataObject_Collection`；独立类型方便 Widget 映射和 save/load 跳过 |
| Config 位置 | 挂在 `UMenuCategoryConfig` 上 | Header 属于 category 级别的展示属性，不是 entry |
| 间距控制 | Slot Padding（Left/Right） | HorizontalBox 原生支持，不需要 Spacer 控件；第一列 Left=EdgePad，最后一列 Right=EdgePad，列间 Right=Gap |
| Widget 基类 | `UW_ListEntry_Base` | 复用现有 ListView 映射机制，匹配 `UDA_DataListEntryMapping` 体系 |
| 动态创建 vs 静态 UMG | 动态创建 `UCommonTextBlock` | 列数可变，静态 UMG 无法满足；运行时从 DataObject 读取列配置后 ClearChildren + 循环创建 |

## Risks / Trade-offs

- **动态创建 Widget 在 UMG 设计师不可见** → Header 列的视觉效果只能运行时验证。可以在编辑器 PIE 里预览。
- **列宽不均匀（文本长度差异大）** → 当前设计只用间距控制，列宽由文本内容自然撑开。后续如需固定列宽可加 `HeaderColumnMinWidth` 字段。
