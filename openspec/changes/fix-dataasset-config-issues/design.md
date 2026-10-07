## Context

当前 Options/Multiplayer 菜单的 DataAsset 驱动初始化路径（`InitFromDataAsset`、`InitFromConfig`）与硬编码 Fallback 路径（`InitVideoCollectionTab`、`InitHardcodedFallback`）行为不一致。核心矛盾在于：`CreateEntryFromConfig` 工厂函数只负责创建 `UListDataObject_Base` 实例，不负责 BindingKey→getter/setter 绑定，导致 DataAsset 路径创建的条目无法读写 `UFrontendGameUserSettings`。

另外，`MultiplayerDataRegistry::InitFromConfig` 拥有自己的类型 switch，但只处理 `Editable` 和 `Bool`，`String`/`Integer` 类型掉入 default 分支变成空 `Collection`。而 Options 路径使用的共享工厂 `CreateEntryFromConfig` 已经正确处理了所有类型。

## Goals / Non-Goals

**Goals:**
- DataAsset 路径创建的 Options 条目能正确读写 `UFrontendGameUserSettings`（BindingKey 绑定）
- `MultiplayerDataRegistry::InitFromConfig` 使用共享工厂 `CreateEntryFromConfig`，消除重复的类型 switch
- `ApplyTabEditConditions` 中 Resolution→WindowMode 的 `AddEditDependencyData` 关系完整
- `CreateEntryFromConfig` 正确处理 `FullscreenMode` BindingKey（创建带枚举选项的 StringEnum）

**Non-Goals:**
- 不修改 DataAsset 内容（.uasset 文件）—— 这是内容工作，不属于代码修复
- 不新增 `Integer` ValueType —— 当前 Integer 类型的条目（画质预设、最大玩家数）通过 BindingKey 判别已能正确创建
- 不移除硬编码 Fallback 路径 —— 保留作为无 DataAsset 时的兜底
- 不修改 `UW_OptionScreen` / `UW_MultiplayerScreen` UI 逻辑

## Decisions

### 1. BindingKey 绑定：在 `InitFromDataAsset` 中统一完成

**选择**: 在 `OptionsDataRegistry::InitFromDataAsset` 中，`CreateEntryFromConfig` 调用后遍历 EntryLookup，对每个非空 BindingKey 的条目调用 `ResolveBinding` 并注入 getter/setter。

**替代方案**: 将 `BindingMap` 引用传入 `CreateEntryFromConfig`。但该函数是全局工厂（在 `MenuConfigData.cpp` 中），引入 Registry 依赖不干净。保持工厂纯净，绑定逻辑留在 Registry 层。

**理由**: 最小侵入。`CreateEntryFromConfig` 保持纯创建语义，Registry 负责绑定。与硬编码路径模式一致（都是先创建、后绑定）。

### 2. MultiplayerDataRegistry::InitFromConfig 使用 CreateEntryFromConfig

**选择**: 删除 `InitFromConfig` 中的自定义 switch，改为调用 `CreateEntryFromConfig`，与 Options 路径一致。

**替代方案**: 补全 Multiplayer 的 switch 分支。但与 Options 路径重复代码，违反 DRY。

**理由**: `CreateEntryFromConfig` 已经正确处理 String（含 AvailableOptions 下拉）、Bool、Editable 等所有类型。Multiplayer 不需要 String 的特殊子类型（Culture、Resolution、Integer quality），但 String 基础类型足够覆盖 Map、GameMode、ServerRegion 等下拉选择。

### 3. FullscreenMode 特殊处理在 CreateEntryFromConfig 中

**选择**: 在 `CreateEntryFromConfig` 的 `String` case 中，检查 `BindingKey == "FullscreenMode"`，创建 `UListDataObject_StringEnum` 并添加三个枚举选项（Fullscreen=0, WindowedFullscreen=1, Windowed=2）。

**替代方案**: 在 `ApplyTabEditConditions` 中替换 WindowMode 条目。但此时条目已创建完成，替换涉及从 Category 中移除旧条目、插入新条目，复杂度远高于一处工厂分支。

**理由**: 与现有模式一致（`ScreenResolution`→`UListDataObject_StringResolution`、`CurrentCulture`→`UListDataObject_StringCulture`）。BindingKey 判别已在 String case 中使用。

### 4. ApplyTabEditConditions 补全 EditDependencyData

**选择**: 在 WindowMode 条件块中，为 Resolution 调用 `AddEditDependencyData(WindowMode)`，确保 WindowMode 变化时 Resolution 的 UI 刷新。

需要先查找 WindowMode 对象。当前代码已经有 `WindowModePtr` 引用，只需追加一行 `(*ResolutionPtr)->AddEditDependencyData(*WindowModePtr)`。

## Risks / Trade-offs

- **`CreateEntryFromConfig` 的 BindingKey 判别列表持续增长**: 当前已有 `CurrentCulture`、`ScreenResolution`、`*Quality*` 三条分支，新增 `FullscreenMode`。如果未来分支数超过 5-6 个，考虑改为策略表驱动。当前 4 条分支可接受。
  → **缓解**: 在代码中加 `ponytail:` 注释标注扩展点。

- **Multiplayer 条目不再需要 getter/setter**: Multiplayer 条目是纯 UI 数据（无持久化），`CreateEntryFromConfig` 不会为它们绑定 Setting。当前行为正确且不需要修改。
  → **无额外风险**。

- **Fallback 路径仍然存在重复代码**: 硬编码 Fallback 路径（`InitGameplayCollectionTab` 等）保留了与 `CreateEntryFromConfig` 类似但独立的创建逻辑。如果 DataAsset 成为唯一路径，这部分可移除。
  → **Non-Goal**: 本次不移除 Fallback，保持向后兼容。
