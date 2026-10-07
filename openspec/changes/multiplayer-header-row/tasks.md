## 1. Config 层 — UMenuCategoryConfig

- [x] 1.1 在 `MenuCategoryConfig.h` 中添加 `HeaderColumns`（`TArray<FText>`）、`HeaderColumnGap`（`float`，默认 40）、`HeaderEdgePadding`（`float`，默认 20），`Gap` 和 `EdgePadding` 的 EditCondition 设为 `!HeaderColumns.IsEmpty()`

## 2. 运行时数据 — UListDataObject_HeaderRow

- [x] 2.1 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.h`
- [x] 2.2 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/DataObjects/ListDataObject_HeaderRow.cpp`
- [x] 2.3 实现 `SetHeaderConfig`、Getter 方法

## 3. Registry — MultiplayerDataRegistry

- [x] 3.1 在 `MultiplayerDataRegistry.cpp` 的 `InitFromConfig` 中，处理 category 时检查 `CategoryConfig->HeaderColumns` 是否非空，非空则创建 `UListDataObject_HeaderRow` 并作为 category 子项的第一个元素
- [x] 3.2 在 `InitHardcodedFallback` 中为 JoinGame tab 的 category 添加示例 HeaderRow（如 `[ServerName, Players, Ping, Status]`）以供测试

## 4. Widget — UW_ListEntry_HeaderRow

- [x] 4.1 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/ListEntries/W_ListEntry_HeaderRow.h`
- [x] 4.2 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/ListEntries/W_ListEntry_HeaderRow.cpp`
- [ ] 4.3 在 UMG 中创建 `WBP_ListEntry_HeaderRow` 蓝图，继承 `UW_ListEntry_HeaderRow`，添加一个 `HorizontalBox` 命名为 `Hb_HeaderColumns`
- [x] 4.4 实现 `OnOwningListDataObjectSet`：从 `UListDataObject_HeaderRow` 读取配置，`ClearChildren` 后循环创建 `UCommonTextBlock`，通过 Slot Padding 设置间距

## 5. Widget 映射 — DA_DataListEntryMapping

- [ ] 5.1 在 `DA_DataListEntryMapping` DataAsset 中添加：`UListDataObject_HeaderRow` → `WBP_ListEntry_HeaderRow`

## 6. 验证

- [ ] 6.1 编译通过，PIE 中打开 Multiplayer Screen → JoinGame tab，确认 header row 显示为多列文本，间距符合配置
