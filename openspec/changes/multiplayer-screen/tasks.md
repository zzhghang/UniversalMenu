## 1. 本地化

- [x] 1.1 在 `LocalizationKeys.h` 中新增 `ST_MULTIPLAYER` StringTable 路径宏和多人游戏文本宏（Tab / Label / Desc / Button 共 16 个宏）

## 2. 配置层（对应 OptionsConfigData）

- [x] 2.1 创建 `MultiplayerConfigData.h`：UMultiplayerEntryConfig / UMultiplayerTabConfig / UMultiplayerConfigData 三个 DataAsset 类

## 3. 数据注册表层（对应 OptionsDataRegistry）

- [x] 3.1 创建 `MultiplayerDataRegistry.h/.cpp`：InitRegistry() 支持 DataAsset 配置驱动或硬编码回退，GetRegisteredTabs() / GetListItemsForTab()

## 4. 数据对象

- [x] 4.1 创建 `UMultiplayerEntryData` — UListDataObject_Base 最小具体子类

## 5. C++ 头文件

- [x] 5.1 重写 `W_MultiplayerScreen.h`：BindWidget（TabList + CommonListView + DetailView）、MultiplayerConfig 属性、GetOrCreateDataRegistry()

## 6. C++ 实现文件

- [x] 6.1 `NativeOnInitialized`：回退操作绑定、Tab 选中/列表 hover/列表 selection 回调注册
- [x] 6.2 `NativeOnActivated`：从 Registry 遍历 Tab 注册、默认选中 CreateGame
- [x] 6.3 `OnMultiplayerTabSelected`：Registry 获取列表项 → SetListItems → Refresh → Navigate
- [x] 6.4 `OnListViewItemHovered/Selected`：更新 DetailView
- [x] 6.5 `GetOrCreateDataRegistry`：懒创建 Registry（SetConfig + InitRegistry）

## 7. 验证

- [x] 7.1 编译通过（UHT 7 文件, link 成功）
- [x] 7.2 蓝图配置步骤见 design.md
