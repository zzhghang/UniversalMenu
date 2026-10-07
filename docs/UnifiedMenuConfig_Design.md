# 统一菜单配置设计

## 问题

Options 和 Multiplayer 各自独立定义了一套 Tab → Category → Entry 三级配置 DataAsset 体系：

| | Options | Multiplayer |
|---|---|---|
| 顶层 Config | `UOptionsConfigData` | `UMultiplayerConfigData` |
| Tab | `UOptionTabData` | `UMultiplayerTabConfig` |
| Category | `UOptionCategoryData` | `UMultiplayerCategoryConfig` |
| Entry | `UOptionEntryData` | `UMultiplayerEntryConfig` |
| Registry | `UOptionsDataRegistry` | `UMultiplayerDataRegistry` |

Tab 和 Category 在两套系统中**字段完全一致**（仅 Id + DisplayName + 子级数组），纯粹重复。Entry 字段虽有语义差异，但核心模式相同（Id、DisplayName、Description、类型标记、依赖联动）。

## 目标

1. Tab / Category 共用同一套 DataAsset 类，消除结构重复
2. Entry 合并为一个统一类，覆盖 Options（Bool/Scalar/String/Enum）和 Multiplayer（Editable/Bool）的 ValueType
3. Getter/Setter 从硬编码枚举改为 BindingKey 机制，各系统自己注册解析逻辑
4. 两个 Registry 保持独立，但共用 InitFromConfig 遍历骨架

## 统一后的类结构

### UMenuTabConfig（替代 UOptionTabData + UMultiplayerTabConfig）

```
TabId : FName
TabName : FText
Categories : Array[UMenuCategoryConfig]
```

### UMenuCategoryConfig（替代 UOptionCategoryData + UMultiplayerCategoryConfig）

```
CategoryId : FName
CategoryName : FText
Entries : Array[UMenuEntryConfig]
```

### UMenuEntryConfig（替代 UOptionEntryData + UMultiplayerEntryConfig）

统一 ValueType：

| ValueType | 说明 | 原有来源 |
|---|---|---|
| Bool | 开关 | Options Bool + Multiplayer Bool |
| Scalar | 滑块数值 | Options Scalar |
| String | 下拉选择 | Options String/Enum |
| Editable | 自由文本输入 | Multiplayer Editable |

字段按 ValueType 条件可见（EditCondition）：

| 字段 | ValueType | 说明 |
|---|---|---|
| DataId | 全部 | 唯一标识 |
| DisplayName | 全部 | 显示名 |
| Description | 全部 | 描述文本 |
| ValueType | 全部 | 类型标记 |
| BindingKey | 全部 | 可选，getter/setter 绑定 key |
| bApplyImmediately | 全部 | 是否立即应用 |
| bPackagedBuildOnly | 全部 | 仅打包版本可用 |
| EditDependencyIds | 全部 | 依赖联动的 Entry Id |
| AvailableOptions | String | 下拉选项 Map |
| ScalarMin / ScalarMax | Scalar | 滑块范围 |
| ScalarOutMin / ScalarOutMax | Scalar | 输出映射范围 |
| StepSize | Scalar | 步长 |
| NumericType | Scalar | 数值显示类型（百分比/整数等） |
| bDefaultBool | Bool | 默认值 |
| MaxCharCount | Editable | 最大字符数 |
| bIsPassword | Editable | 密码掩码 |
| HintText | Editable | 占位提示文本 |
| CharFilter | Editable | 字符过滤 |

### UMenuConfigData（替代 UOptionsConfigData + UMultiplayerConfigData）

```
Tabs : Array[UMenuTabConfig]
```

> Options 和 Multiplayer 各自创建自己的 DataAsset 实例，引用相同的类但填入不同的数据。

## BindingKey 机制

Entry 不再绑死 `EGameUserSettingFunction` 枚举，改为可选的 `FName BindingKey`：

```
Entry.BindingKey = "Resolution"   →  Registry 查找绑定的 getter/setter
Entry.BindingKey = "GameName"     →  无绑定时为纯 UI 数据
Entry.BindingKey = NAME_None      →  无持久化
```

Registry 在初始化时内部维护 binding 注册表：

```cpp
// Options Registry 注册
BindingMap.Add("Resolution", { &GetResolution, &SetResolution });
BindingMap.Add("WindowMode",  { &GetWindowMode,  &SetWindowMode });

// Multiplayer Registry 注册
BindingMap.Add("GameName",    { &GetSessionGameName, &SetSessionGameName });  // session
BindingMap.Add("LastRegion",  { &GetLastServerRegion, &SetLastServerRegion }); // 偏好
```

好处：
- Entry 配置不需要知道值从哪来
- 加新选项不需要改枚举
- 同一个 BindingKey 在不同 Registry 里可以解析到不同实现
- 没绑的 Entry 就是纯 UI 数据（如 Multiplayer 密码输入框）

## 两个 Registry 保持独立

`UOptionsDataRegistry` 和 `UMultiplayerDataRegistry` 不合并为一个类。原因：
- 初始化后处理逻辑不同（Options 有 WindowMode/OverallQuality 等复杂联动，Multiplayer 相对简单）
- Fallback 路径不同（Options 硬编码了完整设置列表，Multiplayer 硬编码了创建/加入游戏 UI）
- 各自持有不同来源的 binding 注册表

但 `InitFromConfig` 的核心遍历骨架可以统一：

```
for each TabConfig in Config.Tabs:
    Tab = CreateCollection(TabConfig.TabId, TabConfig.TabName)
    for each CatConfig in TabConfig.Categories:
        Cat = CreateCollection(CatConfig.CategoryId, CatConfig.CategoryName)
        for each EntryConfig in CatConfig.Entries:
            Entry = CreateEntryByType(EntryConfig)
            if EntryConfig.BindingKey is valid:
                ResolveBinding(Entry, EntryConfig.BindingKey)
            Cat.AddChild(Entry)
        Tab.AddChild(Cat)
```

## 迁移步骤

1. **新建统一 DataAsset 类** — `UMenuTabConfig`、`UMenuCategoryConfig`、`UMenuEntryConfig`、`UMenuConfigData`
2. **迁移 Options** — 替换 `UOptionTabData` 等引用，重写 `InitFromDataAsset` 使用新类型
3. **迁移 Multiplayer** — 替换 `UMultiplayerTabConfig` 等引用，重写 `InitFromConfig`
4. **BindingKey 改造** — Options 的 `EGameUserSettingFunction` 枚举映射改为 BindingKey 注册表
5. **删除旧类** — 移除所有 `UOption*` 和 `UMultiplayer*` 配置 DataAsset 类
6. **更新 DataAsset 资产** — 在编辑器里重建或迁移 .uasset 配置数据

## 不改的部分

- 运行时 DataObject 体系（`UListDataObject_Collection`、`UListDataObject_StringBool` 等）保持不变
- UI Widget 层（`W_OptionScreen`、`W_MultiplayerScreen` 等）保持不变
- `FOptionsDataEditConditionDescriptor` 及联动逻辑保持不变
- 两个 Registry 类名和职责保持不变
