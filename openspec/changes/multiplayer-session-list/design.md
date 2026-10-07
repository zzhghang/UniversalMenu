## Context

JoinGame tab 通过 `UFrontendCommonListView` 渲染扁平列表。当前列表内容来自 `MultiplayerDataRegistry`：HeaderRow（列标题）之后只有一个占位的 `ServerList` Collection。HeaderRow 记录列名和间距配置，由 `W_ListEntry_HeaderRow` 渲染为 `UHorizontalBox` 中的多列 `UCommonTextBlock`。

Session 数据来源于 AdvancedSessions 插件的 FindSessions 异步回调（BP 侧）。需要将搜索结果注入到 C++ 管理的 ListView 中，且 Session 行与 HeaderRow 列对齐。

现有 ListEntry 映射机制：`UDA_DataListEntryMapping` DataAsset 将 `UListDataObject_Base` 子类映射到对应的 `UW_ListEntry_Base` 子类 Widget。列表项的选择/悬停通过 `W_MultiplayerScreen` 的 `OnListViewItemHovered`/`OnListViewItemSelected` 更新 `UW_OptionDetailView` 详情面板。

## Goals / Non-Goals

**Goals:**
- 在 HeaderRow 下方展示 Session 行，每行列数/间距与 HeaderRow 一致
- BP 侧可调用单个 BlueprintCallable 将 FindSessions 结果注入列表
- 刷新时清除旧 Session、填入新 Session
- 复用现有 ListEntry 选择/悬停/映射机制

**Non-Goals:**
- C++ 侧不做 Session 搜索（那是 BP + AdvancedSessions 的职责）
- 不做 Session 排序/过滤（BP 侧排序后传入即可）
- 不做列宽显式对齐配置（沿用 HeaderRow 的 padding 方式）
- 不修改 CreateGame tab

## Decisions

### 1. 数据传递: `FSessionListItem` struct

BP 用 `TArray<FSessionListItem>` 把 session 数据传入 C++。struct 包含展示字段 + `SessionIndex` 用于 BP 回查 `FBlueprintSessionResult`。

**备选**: 直接用 `FBlueprintSessionResult` 数组传入 C++ → 否决，因为 AdvancedSessions 是插件，C++ 侧不应直接依赖其类型。

### 2. 列表刷新方式: Registry 层操作

`MultiplayerDataRegistry::RebuildSessionList()` 直接操作 JoinGame tab 的子节点列表，移除旧 `UListDataObject_Session`，插入新的。然后 Screen 重新获取 flat list 设置到 ListView。

**备选**: 在 Screen 层直接操作 ListView 的 ListItems → 否决，数据归属不清晰，Save/Load 遍历依赖 Registry 树。

### 3. Session Entry 列布局: 复用 HeaderRow 的 padding 策略

`W_ListEntry_Session` 用相同的 Gap/Edge/VerticalPadding 创建列。HeaderRow 和 Session 行共享同一 ListView 宽度，自然对齐。

**备选**: 用 UniformGridPanel 或显式宽度 → 过度设计，列不对齐时再加 `HeaderColumnWidths` 配置即可。

### 4. Entry 控件结构: C++ 逻辑 + BP 布局

`W_ListEntry_Session` 在 C++ 中处理 `OnOwningListDataObjectSet` 动态填充逻辑，BP 只放一个空的 `Hb_SessionColumns` (UHorizontalBox)。与 `W_ListEntry_HeaderRow` 模式一致。

## Risks / Trade-offs

- **列不对齐** → HeaderRow 和 Session 行都用内容自适应宽度的 TextBlock，长文本可能导致列偏移。当前 HeaderRow 本身也没有显式宽度，实际差距通常不可见。后续加 `HeaderColumnWidths` 配置即可修复。
- **Session 选中后 Join 需 BP 回查** → `SessionIndex` 依赖 BP 侧保留 Results 数组，如果 BP 在 PopulateSessionList 后清空了数组则 Join 失败。BP 侧需注意生命周期。
- **Registry 树修改非线程安全** → FindSessions 回调在游戏线程执行，操作 UObject 安全。

## Open Questions

- 是否需要 Session 行的 hover 高亮效果？当前选中已自动触发详情面板更新，hover 行为继承自 `UW_ListEntry_Base`。
- Private Session 的密码弹窗是否复用 CreateGame tab 的密码 Entry？纯 BP 决策，C++ 侧不受影响。
