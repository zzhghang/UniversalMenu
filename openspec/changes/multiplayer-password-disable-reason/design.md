## Context

当前项目有两套选项/条目注册路径：
1. **Hardcoded 路径**：`MultiplayerDataRegistry.cpp` 和 `OptionsDataRegistry.cpp` 中直接创建 DataObject 并设置 EditCondition
2. **DataAsset 路径**：通过 `UMenuConfigData` → `CreateEntryFromConfig` → `ApplyTabEditConditions` 构建

DisableRichReason 目前全部硬编码在 C++ lambda 附近，DataAsset 无法干预。DynamicDetails 的 UI 组件（`Ctb_DynamicDetails`）已存在但代码被注释掉。

## Goals / Non-Goals

**Goals:**
- Password 条目在 HasPassword=false 时用富文本显示禁用原因
- 多个 DisableRichReason 以换行分隔
- UMenuEntryConfig 增加 DisableRichReason 和 DynamicDetails 字段，DataAsset 可配置
- 复活 DynamicDetails 显示逻辑

**Non-Goals:**
- 不在 DataAsset 中完整表达所有 EditCondition lambda（WindowMode 等复杂枚举条件仍需代码）
- 不改变现有的 EditCondition 依赖绑定机制
- 不涉及 Options Screen 的 DisableRichReason 迁移到 DataAsset（仅提供能力，迁移可后续做）

## Decisions

### Decision 1: DisableRichReason 换行分隔位置

在 `UListDataObject_Base::IsDataCurrentlyEditable()` 中，循环遍历 EditConditionDescArray 时，每追加一个 reason 前先检查是否已有内容，有则先加 `\n`。

**替代方案**：在 UI 层（`W_OptionDetailView`）处理 → 不选，因为 DisableRichText 已在 DataObject 上拼接好，UI 层直接显示，改动点在数据层更合理。

### Decision 2: DynamicDetails 的存储位置

在 `UListDataObject_Base` 上增加 `DynamicDetailsRichText` 字段（`FText`），与 `DisableRichText` 并列。DataObject 层持有文本，UI 层读取显示。

**替代方案**：只在 `UW_OptionDetailView` 中动态构建 → 不选，因为用户要求可从 DataAsset 配置，DataAsset 路径只能写入 DataObject，不能直接操作 UI。

### Decision 3: UMenuEntryConfig 新增字段

```cpp
// 条目的禁用原因富文本（当 EditCondition 不满足时显示）
UPROPERTY(EditAnywhere, BlueprintReadOnly)
FText DisableRichReason;

// 条目的动态详情富文本（始终显示在详情面板下方）
UPROPERTY(EditAnywhere, BlueprintReadOnly)
FText DynamicDetails;
```

`CreateEntryFromConfig` 读取这两个字段并设置到 DataObject 上。
- `DisableRichReason`：通过 `SetDisableRichReason` 设置到第一个 EditCondition 上（如果有），或预留后续使用
- `DynamicDetails`：直接 set 到 DataObject

### Decision 4: Password 的 DisableRichReason

在 `MultiplayerDataRegistry.cpp` 中 Password 的 EditCondition 创建处直接调用 `SetDisableRichReason`，使用新 key `TXT_MP_WARN_PASSWORD_DISABLED`。

HasPassword 是 Bool 类型，本身不被禁用，不需要 DisableRichReason。用户说的 "haspassword增加DisableRichReason" 理解为 "Password 被禁用时显示与 HasPassword 相关的原因文本"。

## Risks / Trade-offs

- **风险**：`DisableRichReason` 在 DataAsset 中配置了但 EditCondition 未正确设置 → 表现：配置了文本但从不显示。**缓解**：这属于 DataAsset 配置错误，不会崩溃。当前阶段仅提供字段，实际使用依赖 ApplyTabEditConditions 中已有的 EditCondition。
- **风险**：换行符 `\n` 在富文本中可能需用 `<br>` 或其他标签 → 取决于 `UCommonRichTextBlock` 对换行的解析。**缓解**：先使用 `\n` 测试；若富文本块不解析纯换行符，改为 `<br>`。

## Open Questions

- `UCommonRichTextBlock` 是否直接支持 `\n` 换行？（需打包验证，若不支持改为 `<BR>` 或 `<p>` 标签）
