## ADDED Requirements

### Requirement: GameInstance 持有并管理会话存档数据
`UFrontendGameInstance` SHALL 继承 `UAdvancedFriendsGameInstance`(AdvancedSessions 插件),以保留现有 GameInstance 蓝图的全部功能。
`UFrontendGameInstance` SHALL 负责 `UMenuSessionData` 的创建、从槽位加载、保存到槽位与进程内缓存,并将这些操作暴露为 BlueprintCallable 函数;多人界面不再直接调用 `UGameplayStatics` 的存档 API。

#### Scenario: 保留 AdvancedSessions 功能
- **WHEN** `GI_FrontentGameInstance` 蓝图重设父类为 `UFrontendGameInstance`
- **THEN** AdvancedSessions 提供的会话/好友相关 GameInstance 功能保持可用

#### Scenario: 获取或创建缓存数据
- **WHEN** 任何蓝图或 C++ 调用 `GetOrCreateMenuSessionData()`
- **THEN** 若缓存为空则调用 `CreateSaveGameObject(UMenuSessionData)` 创建并缓存,返回该对象

#### Scenario: 从槽位加载
- **WHEN** 调用 `LoadMenuSessionData()`
- **THEN** 从 `MenuSession` 槽位读取存档,成功后更新缓存并返回;槽位不存在时返回空且不缓存无效对象

#### Scenario: 保存到槽位
- **WHEN** 调用 `SaveMenuSessionData()`
- **THEN** 将缓存的会话数据保存到 `MenuSession` 槽位(槽位索引 0)

#### Scenario: 读取当前缓存
- **WHEN** 调用 `GetMenuSessionData()`
- **THEN** 返回当前缓存对象,未加载过则为空

### Requirement: 多人界面保留 UI 数据的收集与应用职责
`UW_MultiplayerScreen` SHALL 继续负责从 `UMultiplayerDataRegistry` 收集字段值写入存档、以及把存档值应用回列表项;其 `SaveSessionData()` / `LoadSessionData()` / `GetCachedSessionData()` 函数签名 SHALL 保持不变。

#### Scenario: 保存流程
- **WHEN** 蓝图调用多人界面的 `SaveSessionData()`
- **THEN** 界面收集各可编辑项的值写入 GameInstance 提供的会话数据对象,并由 GameInstance 保存到槽位

#### Scenario: 加载流程
- **WHEN** 多人界面激活(`NativeOnActivated`)
- **THEN** 界面从 GameInstance 加载会话数据,并把保存值应用回对应列表项

### Requirement: 存档向后兼容
槽位名 SHALL 保持 `MenuSession`,`UMenuSessionData` 结构与序列化格式 SHALL 不变。

#### Scenario: 读取迁移前的旧存档
- **WHEN** 存在迁移前保存的 `MenuSession` 存档
- **THEN** 迁移后能正常加载并应用其全部字段值
