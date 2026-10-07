## 1. 数据层 — ListDataObject_Editable

- [x] 1.1 扩展 `UListDataObject_Editable`：添加 `MaxCharCount`、`bIsPassword`、`CurrentText` 属性，`SetText()`/`GetText()` 方法（含字符截断逻辑）
- [x] 1.2 实现 `GetHashedPassword()`：使用 `FSHA1::HashBuffer` 对 `CurrentText` 做哈希，空文本返回空字符串
- [x] 1.3 添加 `OnDataObjectInitialized` 逻辑：默认 `MaxCharCount=16`（房间名），密码类型默认 `MaxCharCount=6`

## 2. 控件层 — W_ListEntry_Editable

- [x] 2.1 在 `UW_ListEntry_Editable` 中绑定 `UEditableText` 控件（`meta = (BindWidget)`），添加 `OnEditableTextChanged` 回调
- [x] 2.2 实现 `OnOwningListDataObjectSet`：从 `UListDataObject_Editable` 同步 `CurrentText`、`MaxCharCount`、`bIsPassword` 到控件
- [x] 2.3 实现 `OnToggleEditableState`：根据编辑依赖状态启用/禁用 EditableText

## 3. DataAsset — MultiplayerConfigData 扩展

- [x] 3.1 新增 `UMultiplayerCategoryConfig` UCLASS：包含 `CategoryId`、`CategoryName`、`Entries` 数组
- [x] 3.2 修改 `UMultiplayerTabConfig`：将原本的 `Entries` 数组改为 `Categories` 数组（**BREAKING** — 旧 DataAsset 需更新）
- [x] 3.3 扩展 `UMultiplayerEntryConfig`：增加 `EntryType` 字段区分普通/可编辑/布尔条目类型，增加 `EditDependencyIds` 数组

## 4. Registry — MultiplayerDataRegistry 重构

- [x] 4.1 重构 `InitFromConfig()`：遍历 `Tabs→Categories→Entries` 三层，按条目类型创建对应数据对象（Editable/Bool/Collection）
- [x] 4.2 重构 `InitHardcodedFallback()`：构建 CreateGame 基础 category（房间名、是否密码、密码）和高级 category（最大玩家数、地图、游戏模式、服务器地区）
- [x] 4.3 实现密码编辑依赖：密码条目的 `AddEditDependencyData` 绑定"是否密码"开关，开关为 false 时密码条目禁用
- [x] 4.4 更新 `GetListItemsForTab()`：兼容三层结构，`Tab→GetAllChildListData()` 返回 category 列表（ListView 展示第一层 children）

## 5. 字符串表与蓝图

- [x] 5.1 添加新本地化键：`ST_Multiplayer` 增加房间名、密码、最大玩家数、地图、游戏模式、服务器地区等条目的文本
- [ ] 5.2 更新 `WBP_MultiplayerScreen` 蓝图或对应 ListView 的条目 Widget 类映射，使 Editable 类型条目使用 `W_ListEntry_Editable`
- [ ] 5.3 创建 `WBP_ListEntry_Editable` 蓝图（继承 `W_ListEntry_Editable` C++ 类），绑定 EditableText 控件到 C++ 变量
