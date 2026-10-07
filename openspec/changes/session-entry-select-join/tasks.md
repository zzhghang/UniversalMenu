## 1. 模块依赖

- [x] 1.1 `GameMainMenu.Build.cs` 的 `PublicDependencyModuleNames` 增加 `"OnlineSubsystem"`

## 2. 数据层

- [x] 2.1 `ListDataObject_Session.h`:`FSessionListItem` 增加 `FBlueprintSessionResult SessionResult` 字段(BlueprintReadWrite),include `OnlineBlueprintSupport.h`
- [x] 2.2 编译验证:数据层改动无编译错误

## 3. 条目 Widget(C++)

- [x] 3.1 `W_ListEntry_Session.h`:声明 `BlueprintPure` `GetSessionResult()` 与私有缓存的 `FBlueprintSessionResult`(按钮方案已撤销,无触发事件)
- [x] 3.2 `W_ListEntry_Session.cpp`:`OnOwningListDataObjectSet` 中缓存会话结果(按钮方案已撤销)
- [x] 3.3 编译验证:完整编译通过

## 4. 蓝图接线

- [ ] 4.1 搜索流程蓝图中构建 `FSessionListItem` 时填充 `SessionResult`
- [ ] 4.2 PIE 验证:搜索后条目可取得对应会话结果(触发加入的方式待定)
