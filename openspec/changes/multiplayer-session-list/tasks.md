## 1. C++ 数据层 — FSessionListItem + UListDataObject_Session

- [x] 1.1 新建 `ListDataObject_Session.h`，定义 `FSessionListItem` BlueprintType struct 和 `UListDataObject_Session` UCLASS
- [x] 1.2 新建 `ListDataObject_Session.cpp`，实现 `SetSessionData`

## 2. C++ Entry 控件 — W_ListEntry_Session

- [x] 2.1 新建 `W_ListEntry_Session.h`，声明 `Hb_SessionColumns` BindWidget 和 `OnOwningListDataObjectSet` override
- [x] 2.2 新建 `W_ListEntry_Session.cpp`，实现列动态填充逻辑（与 HeaderRow 相同 padding 策略）

## 3. C++ Registry — MultiplayerDataRegistry Session 刷新

- [x] 3.1 在 `MultiplayerDataRegistry.h` 声明 `RebuildSessionList` 方法
- [x] 3.2 在 `MultiplayerDataRegistry.cpp` 实现：找到 JoinGame tab → 移除旧 Session → 插入新 Session

## 4. C++ Screen — PopulateSessionList BlueprintCallable

- [x] 4.1 在 `W_MultiplayerScreen.h` 声明 `PopulateSessionList` UFUNCTION(BlueprintCallable)
- [x] 4.2 在 `W_MultiplayerScreen.cpp` 实现：调 Registry → 若在 JoinGame tab 则刷新 ListView

## 5. Blueprint 资源（编辑器中操作）

- [ ] 5.1 创建 `WBP_ListEntry_Session` Widget Blueprint，继承 `W_ListEntry_Session`，放置 `Hb_SessionColumns` (UHorizontalBox)
- [ ] 5.2 在 `DA_ListEntryMapping` DataAsset 中新增映射: `UListDataObject_Session → WBP_ListEntry_Session`

## 6. WBP_MultiplayerScreen BP 逻辑（编辑器中操作）

- [ ] 6.1 在 Refresh 回调中：FindSessions → 遍历 Results → 构造 `TArray<FSessionListItem>` → 调用 `PopulateSessionList`
- [ ] 6.2 在 JoinGame 回调中：获取选中 Session → 读 `SessionIndex` → 查 `FBlueprintSessionResult` → JoinSession
