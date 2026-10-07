## Context

当前 `MultiplayerDataRegistry` 只有 tab→entry 的扁平结构，所有条目都是无交互的 `UListDataObject_Collection` 占位符。`W_ListEntry_Editable` 和 `ListDataObject_Editable` 是空壳 stub，需要实现可编辑文本控件。`OptionsDataRegistry` 已有成熟的 tab→category→entry 三层数据驱动模式可供参考。

## Goals / Non-Goals

**Goals:**
- `UListDataObject_Editable` 支持房间名（可中文、可任意字符、限制字符数）和密码（限制位数、哈希存储）
- `UW_ListEntry_Editable` 绑定 EditableText 控件，处理文本变更回调
- `MultiplayerDataRegistry` 重构为 tab→category→entry 三层结构，CreateGame 拆分"基础"/"高级"两个 category
- 密码开关控制密码输入框的编辑依赖（开关关闭时密码禁用）
- DataAsset (`MultiplayerConfigData`) 支持 category 层级，避免硬编码

**Non-Goals:**
- 不实现联机会话创建/加入的网络逻辑
- 不修改 JoinGame tab 的结构（保持扁平，后续按需扩展）
- 不实现密码复杂度校验（纯位数限制即可）

## Decisions

### 1. `UListDataObject_Editable` 继承 `UListDataObject_Base`（非 `UListDataObject_Value`）

**理由**: `UListDataObject_Value` 的 getter/setter 模式绑定 `UFrontendGameUserSettings`，而房间名/密码是运行时临时数据，不需要持久化到游戏设置。直接用 `UListDataObject_Base` 存 `CurrentText` 即可。

**替代方案**: 继承 `UListDataObject_Value` 但需要绕过 getter/setter — 增加复杂度无收益。

### 2. 密码哈希使用 `FSHA1` (UE 内置)

**理由**: UE 自带 `FSHA1::HashBuffer`，无需引入第三方库。密码仅用于创建房间时传递给后端，不存储在本地。哈希后的值通过 `GetHashedPassword()` 暴露给网络层。

### 3. 输入验证在数据对象层做，非控件层

**理由**: `UListDataObject_Editable` 持有 `MaxCharCount` 和 `bIsPassword`，在 `SetText()` 时截断/校验。控件只负责把用户输入传给数据对象。单一职责，数据对象可被多个控件复用。

### 4. `MultiplayerConfigData` 增加 `UMultiplayerCategoryConfig`

**理由**: 对齐 `OptionsConfigData` 的 `UOptionCategoryData`。Tab → Categories[] → Entries[]。保持两个 DataRegistry 的一致性，降低维护心智负担。

**替代方案**: 在现有 `UMultiplayerTabConfig` 上直接加 `Categories` 字段 — 这就是采用方案，本质一致。

### 5. 密码编辑依赖通过 `AddEditDependencyData` 实现

**理由**: `OptionsDataRegistry` 已有成熟的编辑依赖模式（`FOptionsDataEditConditionDescriptor` + `AddEditDependencyData`）。密码条目将"是否开启密码"条目设为依赖数据；当开关为 false 时，密码条目不可编辑。

## Risks / Trade-offs

- **SHA1 非密码级哈希**: FSHA1 仅做简单哈希，不加密。若需要密码级安全，替换为 bcrypt/scrypt。当前适用于游戏内房间密码场景（非用户账号密码）。
- **中文输入依赖 UE EditableText**: UE 的 EditableText 在部分平台可能有 IME 兼容问题，需在实际设备测试。
