## ADDED Requirements

### Requirement: Entry 支持 BindingKey
`UMenuEntryConfig` SHALL 包含可选的 `FName BindingKey` 字段。当 BindingKey 为 `NAME_None` 时，该 Entry 为纯 UI 数据，不参与持久化 getter/setter 绑定。

#### Scenario: Entry 设置 BindingKey
- **WHEN** `UMenuEntryConfig` 实例的 `BindingKey` 设置为 `"Resolution"`
- **THEN** Registry 初始化时通过该 Key 查找注册的 getter/setter 并绑定到对应运行时 DataObject

#### Scenario: Entry 无 BindingKey
- **WHEN** `UMenuEntryConfig` 实例的 `BindingKey` 为 `NAME_None`（默认）
- **THEN** Registry 初始化时不执行 binding 解析，该 Entry 作为纯 UI 数据展示

### Requirement: Registry 维护 BindingMap 注册表
Options Registry 和 Multiplayer Registry SHALL 各自在内部维护 `TMap<FName, FBindingEntry>` 类型的 BindingMap，其中 `FBindingEntry` 包含 getter 和 setter 函数指针或可调用对象。

#### Scenario: Options Registry 注册 Bindings
- **WHEN** `UOptionsDataRegistry` 初始化
- **THEN** 其内部 BindingMap 包含 Resolution/WindowMode/OverallQuality 等 Options 专用 Key 的 getter/setter 映射

#### Scenario: Multiplayer Registry 注册 Bindings
- **WHEN** `UMultiplayerDataRegistry` 初始化
- **THEN** 其内部 BindingMap 包含 GameName/LastRegion 等 Multiplayer 专用 Key 的 getter/setter 映射

### Requirement: InitFromConfig 解析 BindingKey
Registry 的初始化遍历过程 SHALL 在创建 Entry 对应的运行时 DataObject 后，检查 EntryConfig 的 BindingKey 是否有效。若有效且 BindingMap 中存在该 Key，则调用对应的 getter 填充初始值并绑定 setter。

#### Scenario: 有效 BindingKey 解析成功
- **WHEN** EntryConfig 的 `BindingKey` 为 `"WindowMode"` 且 Options Registry 的 BindingMap 中存在该 Key
- **THEN** Registry 调用 getter 获取当前 `EWindowMode::Type` 值并写入运行时 DataObject，同时绑定 setter 以便用户修改后持久化

#### Scenario: 无效 BindingKey 不崩溃
- **WHEN** EntryConfig 的 `BindingKey` 为 `"UnknownKey"` 且 Registry 的 BindingMap 中不存在该 Key
- **THEN** 系统输出 `ensureMsgf` 警告但不崩溃，Entry 作为无绑定数据展示

### Requirement: EGameUserSettingFunction 枚举移除
编译单元 SHALL 不再引用 `EGameUserSettingFunction` 枚举。所有 `CreateHelperFromEnum`、`CreateEntryFromData` 等依赖该枚举的工厂函数改为基于 `EMenuEntryValueType` + `BindingKey` 的版本。

#### Scenario: 编译不包含 EGUSF 枚举
- **WHEN** 项目完成迁移后编译
- **THEN** `OptionsDataTypes.h` 中不再包含 `EGameUserSettingFunction` 枚举定义，相关 switch-case 逻辑已移除
