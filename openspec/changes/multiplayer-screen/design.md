## Context

当前 W_MultiplayerScreen 是一个空壳类（继承 UW_ActivatableBase）。完整参考 UW_OptionScreen 的三层架构：**Config DataAsset → DataRegistry → Screen**。

## Goals / Non-Goals

**Goals:**
- 完整的 OptionScreen 架构对齐：MultiplayerConfigData + MultiplayerDataRegistry + MultiplayerScreen
- ListView + DetailView（复用 UW_OptionDetailView）保证列表虚拟化性能
- 支持 DataAsset 配置（设计师可换配置不碰 C++），也支持硬编码回退

**Non-Goals:**
- 不实现实际的多人游戏会话创建/加入逻辑（由 Online Subsystem 负责）
- 不创建新的 DetailView 类（复用 UW_OptionDetailView）
- 条目不处理值类型、编辑条件、getter/setter（与 OptionScreen 不同，多人游戏条目是信息展示而非可配置值）

## Architecture

```
MultiplayerConfigData (DataAsset, optional)
    └── MultiplayerTabConfig[] (TabId + TabName + Entries)
            └── MultiplayerEntryConfig[] (EntryId + DisplayName + Description)

MultiplayerDataRegistry (runtime)
    ├── InitFromConfig()   — builds collections from DataAsset
    └── InitHardcodedFallback() — when no DataAsset assigned

UW_MultiplayerScreen (Widget)
    └── GetOrCreateDataRegistry() → reads tabs, populates list
```

## Decisions

### 决策 1: 三层架构对齐 OptionScreen
**选择**: Config DataAsset → DataRegistry → Screen，完整三层。

**原因**: 与 OptionScreen 架构一致，降低团队理解成本。DataAsset 可配（设计师可改 Tab 顺序/名称/条目不需改 C++），空配置自动走硬编码回退。

### 决策 2: UMultiplayerEntryData 仅用于展示
**选择**: 继承 UListDataObject_Base 不添加任何值类型成员。只设置 DisplayName + DescriptionRichText 供 ListView 和 DetailView 展示。

**原因**: 多人游戏条目不同于选项配置——不需要 getter/setter、编辑条件、值验证。当前阶段是信息展示框架。后续如需要交互（文本输入、下拉选择），再扩展子类。

### 决策 3: 复用 UW_OptionDetailView
**选择**: 直接使用 OptionScreen 的详情视图。

**原因**: UW_OptionDetailView 展示的是 UListDataObject_Base 的通用属性（DisplayName、DescriptionRichText、SoftDescriptionImage、DisableRichText），不绑定 Options 特定类型。

### 决策 4: 新文件位置
**选择**: 所有多人游戏相关类放在 `Widgets/Multiplayer/` 下。

| 文件 | 对应 OptionScreen 等价物 |
|---|---|
| `MultiplayerConfigData.h` | `OptionsConfigData.h` |
| `MultiplayerDataRegistry.h/.cpp` | `OptionsDataRegistry.h/.cpp` |
| `UMultiplayerEntryData.h/.cpp` | `ListDataObject_Base` 的具体子类 |
| `W_MultiplayerScreen.h/.cpp` | `W_OptionScreen.h/.cpp` |

## Risks / Trade-offs

- **[风险] Config DataAsset 可能留空** → InitRegistry 检测空 config 走硬编码回退，保证界面可运行
- **[风险] UMultiplayerEntryData 无法交互** → 当前仅展示，交互留待后续 change。如 Create Game 表单需要输入框，届时创建 UMultiplayerEntryData_TextInput 子类

## Blueprint 配置步骤

### 1. 创建 StringTable — `ST_Multiplayer`

在 `Content/BP/StringTable/` 下新建，添加条目（见 LocalizationKeys.h 中所有 TXT_MP_* 宏对应的 Key）。

### 2. WBP_MultiplayerScreen 的 BindWidget

| Widget 名称 | 类型 |
|---|---|
| `TabListWidget_MultiplayerTabs` | `UFrontendCommonTabListWidgetBase` |
| `CommonListView_MultiplayerList` | `UFrontendCommonListView` |
| `DetailsView_ListEntryInfo` | `UW_OptionDetailView` |

### 3. （可选）创建 DataAsset 自定义 Tab

1. 创建 `UMultiplayerTabConfig` DataAsset 若干（每个 Tab 一个）
2. 为每个 Tab 创建 `UMultiplayerEntryConfig` DataAsset（每个条目一个）
3. 创建 `UMultiplayerConfigData` DataAsset 并填入 Tabs 数组
4. 在 WBP_MultiplayerScreen 蓝图的 `MultiplayerConfig` 属性中赋值

不赋值则自动走硬编码回退（创建游戏 / 加入游戏 两个 Tab）。
