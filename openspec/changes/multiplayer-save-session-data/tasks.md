## 1. UMenuSessionData 类创建

- [x] 1.1 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/Multiplayer/MenuSessionData.h`，定义 `UMenuSessionData : USaveGame`，包含 `UPROPERTY(SaveGame) TMap<FName, FString> SavedValues`
- [x] 1.2 在 `MenuSessionData.h` 中添加 `GetSavedValue(DataId)` / `SetSavedValue(DataId, Value)` helper 方法

## 2. W_MultiplayerScreen 中新增 Save/Load 逻辑

- [x] 2.1 在 `W_MultiplayerScreen.h` 中声明 `SaveSessionData()` 和 `LoadSessionData()` 方法
- [x] 2.2 实现 `SaveSessionData()`：遍历 `GetOrCreateDataRegistry()` 所有 Tab 下所有条目，对 `UListDataObject_Editable` 取文本（密码取 hash）、对 `UListDataObject_String` 及其子类取 `GetCurrentStringValue()`，按 DataId 存入 `UMenuSessionData::SavedValues`，调用 `UGameplayStatics::SaveGameToSlot`
- [x] 2.3 实现 `LoadSessionData()`：从 `"MenuSession"` slot 加载 `UMenuSessionData`，遍历条目，用 `OnSetForcedStringValue`（String 子类）或 `SetText`（Editable）回填值

## 3. 接入生命周期和操作回调

- [x] 3.1 在 `OnStartGameActionTriggered()` 中调用 `SaveSessionData()`，之后打印日志确认保存
- [x] 3.2 在 `OnJoinGameActionTriggered()` 中调用 `SaveSessionData()`，之后打印日志确认保存
- [x] 3.3 在 `NativeOnActivated()` 末尾、`TabListWidget_MultiplayerTabs->SelectTabByID` 之后调用 `LoadSessionData()`

## 4. 验证

- [x] 4.1 编译通过
- [ ] 4.2 PIE 中：打开 MultiplayerScreen，填写 ServerName / Password 等字段，触发 StartGame → 关闭 → 重新打开 → 确认字段已回填
- [ ] 4.3 验证密码字段回填的是 hash 而非明文
