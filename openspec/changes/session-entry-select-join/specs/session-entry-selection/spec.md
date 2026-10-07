## ADDED Requirements

### Requirement: Session 条目携带会话结果

`FSessionListItem` SHALL 包含一个 `FBlueprintSessionResult` 类型的 `SessionResult` 字段,蓝图可读写,并随 `PopulateSessionList` 数据链路传递到列表条目。`UW_ListEntry_Session` SHALL 通过 `GetSessionResult()`(BlueprintPure)暴露当前条目的会话结果。

#### Scenario: 搜索结果随条目数据传递

- **WHEN** 蓝图构建 `FSessionListItem`、设置其 `SessionResult` 并调用 `PopulateSessionList`
- **THEN** 对应条目的 `UListDataObject_Session` 保存该结果,条目 widget 的 `GetSessionResult()` 返回同一会话结果

(已撤销:条目内按钮触发加入操作的需求被用户移除,触发方式待定)
