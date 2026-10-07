## Context

当前 `W_CreditsScreen::PopulateCreditsEntries` 使用 `UTextBlock` 动态生成 Credits 条目，每项分两行（Name 和 Role 各一行），未设置对齐，无法通过 CommonUI 的 TextStyle DataAsset 统一管理字体样式。

需要改为：每行 "Role : Name" 的格式，使用 `UCommonTextBlock` 居中显示，支持通过 `UCommonTextStyle` 配置字体。

## Goals / Non-Goals

**Goals:**
- 每行显示格式从两行改为单行 "Role : Name"
- 文本水平居中
- 支持 CommonUI TextStyle DataAsset 配置字体样式（大小、颜色、字距等）

**Non-Goals:**
- 不修改数据层（FCreditsEntry 结构保持不变）
- 不改变滚动机制
- 不添加 RichText 标记支持（保持纯文本）

## Decisions

### 使用 UCommonTextBlock 替代 UTextBlock

**选择：** `UCommonTextBlock`

**替代方案：**
- `URichTextBlock`：支持富文本标记，但配置复杂，无 TextStyle 概念
- 保持 `UTextBlock` + 手动设置 Font：简单但无法复用 CommonUI 的 TextStyle 资产

**理由：** 项目已使用 CommonUI，`UCommonTextBlock` 原生支持 `UCommonTextStyle` DataAsset，与项目现有 UI 体系一致。

### 格式化为 "Role : Name" 在代码中完成

**选择：** 在 `PopulateCreditsEntries` 中用 `FText::Format` 拼接

**理由：** 数据模型 `FCreditsEntry` 保持 Name/Role 分离，方便未来改为其他格式或本地化。拼接逻辑在 C++ 端完成，避免蓝图开销。

### 文本居中对齐

**选择：** 通过每个条目的 Slot 设置水平对齐为 `HAlign_Center`，同时设置 TextJustification 为 `Center`

**理由：** VerticalBox 的 AddChild 默认创建全宽 Slot，水平对齐在 Slot 层面控制；TextJustification 控制文本块内的多行对齐。

## Risks / Trade-offs

- `UCommonTextBlock` 创建失败回退：如果 TextStyle 未配置（TSoftObjectPtr 为空），使用 CommonUI 默认样式，不会崩溃
- ScrollBox 滚动性能：条目数 < 200 时无明显差异
