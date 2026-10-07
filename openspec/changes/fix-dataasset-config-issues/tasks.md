## 1. CreateEntryFromConfig 修复

- [x] 1.1 在 `String` case 中新增 `FullscreenMode` BindingKey 分支，创建 `UListDataObject_StringEnum` 并填入 Fullscreen/WindowedFullscreen/Windowed 三个选项
- [x] 1.2 设置 FullscreenMode 条目的默认值为 WindowedFullscreen（枚举值 1）

## 2. InitFromDataAsset 绑定 BindingKey

- [x] 2.1 在 `InitFromDataAsset` 的条目创建循环中，对每个 EntryData 的非空 BindingKey 调用 `ResolveBinding`，将返回的 Helper 注入 `DataDynamicGetter`/`DataDynamicSetter`
- [x] 2.2 确保 `ResolveBinding` 返回 nullptr 时不崩溃（无效 BindingKey 退化为纯 UI 数据）

## 3. ApplyTabEditConditions 补全 EditDependencyData

- [x] 3.1 在 `ApplyTabEditConditions` 的 WindowMode 处理块中，WindowMode 查找成功后，追加 `Resolution->AddEditDependencyData(WindowMode)`

## 4. MultiplayerDataRegistry 使用共享工厂

- [x] 4.1 将 `InitFromConfig` 中的自定义 switch 替换为调用 `CreateEntryFromConfig`
- [x] 4.2 确保 Multiplayer 无需 BindingKey 绑定的逻辑正确（跳过 getter/setter 注入）
- [x] 4.3 验证 String 类型条目（Map、GameMode、ServerRegion）正确创建为有下拉选项的 StringEnum

## 5. 编译验证

- [x] 5.1 编译 GameMainMenu 模块，确保无编译错误
- [x] 5.2 检查相关代码无新增警告
