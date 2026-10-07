## ADDED Requirements

### Requirement: 保存会话数据
系统 SHALL 在用户触发 StartGame 或 JoinGame 操作时，将 MultiplayerScreen 中所有可编辑字段的值保存到本地 SaveGame slot。

#### Scenario: 保存 Create Game 数据
- **WHEN** 用户在 Create Game 标签页填写了服务器名、密码、最大玩家数等字段，并触发 StartGame 操作
- **THEN** 系统遍历当前 Registry 中所有 Tab 下的所有条目，将每个可赋值条目的 DataId 与对应字符串值存入 `TMap<FName, FString>`，并通过 `UGameplayStatics::SaveGameToSlot` 写入 `"MenuSession"` slot

#### Scenario: 密码字段保存 hash
- **WHEN** 保存的条目是密码类型（`bIsPassword == true`）
- **THEN** 系统调用 `GetHashedPassword()` 获取 hash 值存储，而非明文

#### Scenario: 非密码文本字段保存原文
- **WHEN** 保存的条目是普通 Editable 类型（`bIsPassword == false`）
- **THEN** 系统存储 `GetText().ToString()` 原文

### Requirement: 加载会话数据
系统 SHALL 在 MultiplayerScreen 激活时自动从 SaveGame slot 加载上次保存的数据，并回填到对应 DataId 的字段中。

#### Scenario: 自动回填文本字段
- **WHEN** MultiplayerScreen 激活，且存在 `"MenuSession"` SaveGame slot 中有匹配的 DataId
- **THEN** 对应的 `UListDataObject_Editable` 条目的 `CurrentText` 被恢复为保存的值

#### Scenario: 无已保存数据时不报错
- **WHEN** SaveGame slot 不存在或为空（首次启动）
- **THEN** 系统静默跳过，不打印错误，字段保持默认空值

#### Scenario: 已保存的 DataId 在当前配置中不存在
- **WHEN** SaveGame 中有某 DataId 对应的值，但当前 DataAsset 配置中已无该字段
- **THEN** 系统忽略该 key，不报错，不影响其他字段的回填

### Requirement: SessionData 类结构
系统 SHALL 提供 `UMenuSessionData : USaveGame` 类，包含通用键值存储能力。

#### Scenario: 类定义
- **WHEN** `UMenuSessionData` 被定义
- **THEN** 它继承 `USaveGame`，包含 `UPROPERTY(SaveGame) TMap<FName, FString> SavedValues` 字段，用于按 DataId 存储所有可编辑字段的值
