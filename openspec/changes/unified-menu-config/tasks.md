## 1. 新建统一 DataAsset 类

- [x] 1.1 创建 `EMenuEntryValueType` 枚举（Bool/Scalar/String/Integer/Editable），放入 `Widgets/MenuConfig/MenuConfigTypes.h`
- [x] 1.2 创建 `UMenuEntryConfig` DataAsset 类，包含统一的字段 + EditCondition 条件可见，放入 `Widgets/MenuConfig/MenuEntryConfig.h`
- [x] 1.3 创建 `UMenuCategoryConfig` DataAsset 类（CategoryId/CategoryName/Entries），放入 `Widgets/MenuConfig/MenuCategoryConfig.h`
- [x] 1.4 创建 `UMenuTabConfig` DataAsset 类（TabId/TabName/Categories），放入 `Widgets/MenuConfig/MenuTabConfig.h`
- [x] 1.5 创建 `UMenuConfigData` DataAsset 类（Tabs 数组），放入 `Widgets/MenuConfig/MenuConfigData.h`
- [x] 1.6 添加 `CreateEntryFromConfig(const UMenuEntryConfig*)` 工厂函数声明及实现

## 2. BindingKey 机制实现

- [x] 2.1 定义 `FBindingEntry` 结构体 — ponytail: 直接复用 `FOptionsDataInteractionHelper`，BindingMap 为 `TMap<FName, TSharedPtr<FOptionsDataInteractionHelper>>`，无需额外包装
- [x] 2.2 在 `UOptionsDataRegistry` 中初始化 Options BindingMap（Resolution/WindowMode/OverallQuality 等共 35 个 binding）
- [x] 2.3 Multiplayer Registry BindingMap — ponytail: Multiplayer 条目目前无持久化 getter/setter 需求，BindingMap 为空，加新 binding 时直接 Add 即可
- [x] 2.4 在 Registry 遍历中实现 `ResolveBinding(Entry, BindingKey)` 逻辑：从 BindingMap 查找 → 调用 getter 填初始值 → 绑定 setter

## 3. Options Registry 迁移

- [x] 3.1 更新 `UOptionsDataRegistry` 的 Config 属性类型：`UOptionsConfigData*` → `UMenuConfigData*`
- [x] 3.2 重写 `InitFromDataAsset` 使用 `UMenuTabConfig`/`UMenuCategoryConfig`/`UMenuEntryConfig` 遍历，集成 BindingKey 解析
- [x] 3.3 更新 `ApplyTabEditConditions` 参数签名适配新类型
- [x] 3.4 更新 CreateBoolEntry/CreateScalarEntry 等 helper：使用 `FName BindingKey` 替代 `TSharedPtr<FOptionsDataInteractionHelper>`
- [x] 3.5 更新 `W_OptionScreen` 的 `OptionsConfig` 属性类型：`UOptionsConfigData*` → `UMenuConfigData*`

## 4. Multiplayer Registry 迁移

- [x] 4.1 更新 `UMultiplayerDataRegistry` 的 Config 属性类型：`UMultiplayerConfigData*` → `UMenuConfigData*`
- [x] 4.2 重写 `InitFromConfig` 使用 `UMenuTabConfig`/`UMenuCategoryConfig`/`UMenuEntryConfig` 遍历，通过 `CreateEntryFromConfig` 创建运行时 DataObject
- [x] 4.3 `InitHardcodedFallback` 无需改动（直接创建 DataObject，不引用旧 DataAsset 类）；更新 `ListDataObject_Editable.h` include 从 `MultiplayerConfigData.h` → `MenuConfigTypes.h`

## 5. 删除旧 DataAsset 类

- [x] 5.1 删除 `OptionsConfigData.h/.cpp`（含 UOptionEntryData、UOptionCategoryData、UOptionTabData、UOptionsConfigData）
- [x] 5.2 删除 `MultiplayerConfigData.h`（含 UMultiplayerEntryConfig、UMultiplayerCategoryConfig、UMultiplayerTabConfig、UMultiplayerConfigData、EEditableCharFilter、EMultiplayerEntryType）
- [x] 5.3 删除 `OptionsDataTypes.h`（含 EOptionValueType、EGameUserSettingFunction 枚举）
- [x] 5.4 清理所有残留的旧类 `#include` 和前向声明

## 6. 编辑器资产更新

- [ ] 6.1 在编辑器中创建新的 `UMenuConfigData` DataAsset 用于 Options 配置
- [ ] 6.2 在编辑器中创建新的 `UMenuConfigData` DataAsset 用于 Multiplayer 配置
- [ ] 6.3 将 W_OptionScreen 蓝图中的 `OptionsConfig` 引用指向新 DataAsset
- [ ] 6.4 将 MultiplayerDataRegistry 的 Config 属性指向新 DataAsset

## 7. 验证

- [ ] 7.1 编译通过（无旧类引用残留）
- [ ] 7.2 Options 界面功能验证：Tab 切换、设置项显示、值修改与持久化、联动编辑条件正常
- [ ] 7.3 Multiplayer 界面功能验证：Tab 切换、Editable/Bool 条目显示、依赖联动正常
- [ ] 7.4 Fallback 路径验证：不分配 DataAsset 时 Options/Multiplayer 均回退到硬编码默认 UI
