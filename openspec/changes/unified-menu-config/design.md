## Context

当前 GameMainMenu 插件中，Options 和 Multiplayer 各自独立定义了一套 Tab → Category → Entry 三级配置 DataAsset 体系，共 8 个 DataAsset 类 + 2 个 Registry 类。其中 Tab 和 Category 层字段完全一致（仅 Id + DisplayName + 子级数组），Entry 层核心模式相同仅字段有语义差异。

另外，Options Entry 通过 `EGameUserSettingFunction` 枚举硬编码 getter/setter 映射，加新选项需要改枚举定义、改 switch-case、改工厂函数，是三个分散的改动点。

## Goals / Non-Goals

**Goals:**
- Tab/Category 共用同一套 DataAsset 类，消除结构重复
- Entry 合并为一个统一类，覆盖 Options（Bool/Scalar/String/Enum）和 Multiplayer（Editable/Bool）的 ValueType
- Getter/Setter 从硬编码枚举改为 BindingKey 机制，各系统自己注册解析逻辑
- 两个 Registry 保持独立但共用 InitFromConfig 遍历骨架

**Non-Goals:**
- 不合并 UOptionsDataRegistry 和 UMultiplayerDataRegistry 为一个类
- 不改变运行时 DataObject 体系（UListDataObject_Base 及子类）
- 不改变 UI Widget 层（W_OptionScreen、W_MultiplayerScreen 等）
- 不改变 FOptionsDataEditConditionDescriptor 及联动逻辑
- 不改变 LocalizationKeys 体系，仅更新引用路径

## Decisions

### 1. 统一类命名

使用 `UMenu*` 前缀替代 `UOption*` / `UMultiplayer*`:

| 新类 | 替代 |
|------|------|
| `UMenuEntryConfig` | `UOptionEntryData` + `UMultiplayerEntryConfig` |
| `UMenuCategoryConfig` | `UOptionCategoryData` + `UMultiplayerCategoryConfig` |
| `UMenuTabConfig` | `UOptionTabData` + `UMultiplayerTabConfig` |
| `UMenuConfigData` | `UOptionsConfigData` + `UMultiplayerConfigData` |

**为何不用通用名称**: `UEntryConfig` 过于宽泛容易冲突，`UMenu*` 明确了用途域。

### 2. Entry ValueType 统一为 4 种

新枚举 `EMenuEntryValueType`：

| 值 | 覆盖 |
|----|------|
| `Bool` | Options Bool + Multiplayer Bool |
| `Scalar` | Options Scalar |
| `String` | Options String/Enum/QualityPreset/Culture/Resolution + Multiplayer Editable 的下拉化使用 |
| `Editable` | Multiplayer Editable（自由文本） |

**为何合并 String + Enum**: `AvailableOptions` Map 已完全区分下拉和自由文本 — 有选项就是下拉，无选项就是自由文本。但 Multiplayer Editable 有 `MaxCharCount`、`bIsPassword`、`HintText`、`CharFilter` 等专用字段，保留独立 `Editable` 类型避免这些字段在 String 条目上出现（EditCondition 可隐藏但不优雅）。

### 3. BindingKey 替代 GetterFunc/SetterFunc 枚举

`EGameUserSettingFunction` 枚举改为可选的 `FName BindingKey`：

```cpp
UPROPERTY(EditAnywhere, BlueprintReadOnly)
FName BindingKey;  // NAME_None = 纯 UI 数据，无持久化
```

各 Registry 在初始化时内部维护 binding 注册表：

```cpp
// Options Registry
BindingMap.Add("Resolution", { &GetResolution, &SetResolution });
// Multiplayer Registry
BindingMap.Add("GameName",    { &GetSessionGameName, &SetSessionGameName });
```

**为何不统一注册表**: 同一个 BindingKey（如 "MaxPlayers"）可能在两个 Registry 中解析到不同实现，各自注册表语义清晰且无耦合。

### 4. Registry 不合并但共享遍历骨架

两个 Registry 类名和职责保持不变，但 `InitFromConfig` / `InitFromDataAsset` 的核心三层 for 循环（Tab → Category → Entry → CreateEntryByType → ResolveBinding）在各自实现中保持一致。不提取基类或模板 — 两个遍历的差异点（Entry 创建逻辑、edit condition 后处理）足以让抽象的价值小于其成本。

## Risks / Trade-offs

- **[DataAsset 迁移]** 旧 .uasset 配置资产引用了旧类，新类创建后旧资产无法直接打开 → 需要在编辑器中重新创建 DataAsset 资产，或通过重定向器（CoreRedirects）映射旧类型名到新类型名
- **[Breaking Change]** 8 个旧 DataAsset 类移除后，任何外部引用它们的代码（如果有）会编译失败 → 在同一次提交中完成所有引用替换
- **[BindingKey 字符匹配]** BindingKey 是 FName 字符串，拼写错误在编译期检测不到 → 运行时加 `ensureMsgf` 检测未解析的 BindingKey，避免静默失败
- **[Enum → FName 功能等价]** `EGameUserSettingFunction` 的部分复杂逻辑（如 WindowMode/OverallQuality 的特殊联动）是枚举名驱动的 → 迁移到 BindingKey 后仍需保留相应的特殊处理逻辑（不受影响，这些逻辑在 Registry 层而非 Entry 层）

## Migration Plan

1. 新建 4 个统一 DataAsset 类（保留旧类不删）
2. 更新 Options Registry：引用改为新类，`InitFromDataAsset` 使用新类型 + BindingKey
3. 更新 Multiplayer Registry：引用改为新类，`InitFromConfig` 使用新类型 + BindingKey
4. 在编辑器中创建新 DataAsset 资产，填充与旧资产等价的数据
5. 更新 Widget 层引用（W_OptionScreen 的 `OptionsConfig` 属性等）
6. 编译通过 + 功能验证后，删除 8 个旧 DataAsset 类
7. 可选：添加 CoreRedirects 使旧资产路径指向新类（如果已有打包版本的旧资产需要兼容）

## Open Questions

- 旧 .uasset 资产是否已在打包版本中使用？如果是，需要 CoreRedirects 方案；如果还在开发阶段，直接重建即可
- `UMenuEntryConfig` 是否放在 `GameMainMenu/Public/Widgets/` 根下，还是分到独立的 `MenuConfig/` 子目录？
