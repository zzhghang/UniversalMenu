## ADDED Requirements

### Requirement: Session 数据对象存储 Session 展示信息
系统 SHALL 提供 `UListDataObject_Session` 继承 `UListDataObject_Base`，持有一个 `FSessionListItem` 结构体，包含服务器名称、地图名、当前玩家数、最大玩家数、是否私有、Ping 值和 Session 索引。

#### Scenario: 设置和读取 Session 数据
- **WHEN** 调用 `SetSessionData` 传入 `FSessionListItem`
- **THEN** `GetSessionData` 返回相同的数据

#### Scenario: Session 数据对象不参与 Save/Load
- **WHEN** `SaveSessionData` 或 `LoadSessionData` 遍历列表项
- **THEN** `UListDataObject_Session` 的 Cast 失败，自动跳过

### Requirement: Session 列表项控件以多列格式展示 Session 信息
系统 SHALL 提供 `W_ListEntry_Session` 继承 `UW_ListEntry_Base`，在 `OnOwningListDataObjectSet` 时从 `UListDataObject_Session` 读取数据，在 `Hb_SessionColumns` HorizontalBox 中动态创建 UCommonTextBlock 子控件，列数、列间距、边距与 HeaderRow 保持一致。

#### Scenario: 列表项展示 Session 数据
- **WHEN** ListView 将 `UListDataObject_Session` 设置到 `W_ListEntry_Session`
- **THEN** HorizontalBox 中显示五列文本：ServerName、Map、Players（格式为 "C/M"）、Private（Yes/No）、Ping（格式为 "Nms"）

#### Scenario: Session 列表项不可编辑
- **WHEN** 系统调用 `OnToggleEditableState`
- **THEN** 控件无变化，忽略调用

### Requirement: BP 可通过 BlueprintCallable 注入 Session 数据
系统 SHALL 在 `W_MultiplayerScreen` 提供 `PopulateSessionList` BlueprintCallable 方法，接收 `TArray<FSessionListItem>` 参数，调用 `MultiplayerDataRegistry` 清空旧 Session 条目并插入新条目，若当前选中 JoinGame tab 则刷新 ListView。

#### Scenario: 首次刷新填充 Session 列表
- **WHEN** FindSessions 回调中 BP 调用 `PopulateSessionList` 传入 3 个 `FSessionListItem`
- **THEN** JoinGame tab 的列表在 HeaderRow 下方显示 3 个 Session 行

#### Scenario: 再次刷新替换 Session 列表
- **WHEN** 用户再次点击 Refresh，BP 调用 `PopulateSessionList` 传入 5 个新 Session
- **THEN** 旧的 3 个 Session 行被移除，列表显示 5 个新 Session 行

#### Scenario: 非 JoinGame tab 时不刷新
- **WHEN** 当前选中 CreateGame tab，BP 调用 `PopulateSessionList`
- **THEN** 数据已更新但 ListView 不立即刷新，切换到 JoinGame tab 时自动可见

### Requirement: Session 数据对象到列表项控件的映射
系统 SHALL 在 `DA_ListEntryMapping` DataAsset 中配置 `UListDataObject_Session` 到 `W_ListEntry_Session` 的映射，使 `UFrontendCommonListView` 在生成列表项时自动选择正确的 Entry Widget。

#### Scenario: ListView 为 Session 数据对象选择 Entry 控件
- **WHEN** `UFrontendCommonListView` 遇到 `UListDataObject_Session` 类型的列表项
- **THEN** 生成 `W_ListEntry_Session` 实例作为列表项控件
