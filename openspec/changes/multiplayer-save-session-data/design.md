## Context

MultiplayerScreen 当前架构：`UMenuConfigData` (DataAsset) → `UMultiplayerDataRegistry` (运行时) → `W_MultiplayerScreen` (UI)。用户的输入值存在 `UListDataObject_Editable` 等运行时数据对象中，界面 Deactivate 后即被 GC 回收。

数据按 DataId 标识（从 DataAsset 配置），类型包含 Editable（文本）、Integer（整数下拉）等。DataAsset 可动态增删字段，所以持久化方案需支持动态键，不能硬编码字段。

## Goals / Non-Goals

**Goals:**
- 通过 `USaveGame` 将 MultiplayerScreen 所有可编辑字段的值持久化到磁盘
- 字段以 DataId 为 key 存入通用 `TMap<FName, FString>`，适配 DataAsset 动态配置
- 界面激活时自动从 SaveGame 回填已有数据
- StartGame / JoinGame 触发时保存当前数据

**Non-Goals:**
- 不提供多槽位选择（只用一个固定 slot）
- 不同步到云端
- 不提供数据版本迁移（session 数据丢失可接受）

## Decisions

### 1. 用 USaveGame 而不用 UFrontendGameUserSettings

**选 USaveGame**：因为字段是动态的（DataAsset 驱动），`UFrontendGameUserSettings` 需要为每个字段写 `UPROPERTY(Config)`，跟动态配置矛盾。而 `TMap<FName, FString>` 在 SaveGame 中天然支持序列化，加字段不需要改 C++。

### 2. 统一存为 FString

Bool、Int、Editable 全部 `ToString()` / `FromString()` 转换。保持一致性的同时降低了类型判断的复杂度。只有 `UListDataObject_Editable` 和整数/布尔值类型的条目需要处理，转换逻辑足够简单。

### 3. 保存时机

- **Load**: `NativeOnActivated` 时，Registry 构建完成后，遍历条目回填
- **Save**: `OnStartGameActionTriggered` / `OnJoinGameActionTriggered` 时，收集当前所有值写入 SaveGame slot

不在每次字段修改时保存——没必要频繁写磁盘，操作触发时保存即可。

### 4. 固定 slot 名称

使用 `"MenuSession"` 作为 slot 名称。不需要多槽位管理，保持简单。

## Risks / Trade-offs

- **风险**: 所有字段值转 `FString` 存储，密码 hash 和原文本都存同一个 Map → 密码字段的 `bIsPassword` 属性不影响存值，存的值取决于调用 `GetHashedPassword()` 还是 `GetText()`
- **缓解**: 保存时，如果 `bIsPassword`，调用 `GetHashedPassword()` 存 hash；其余字段存 `GetText().ToString()`
- **风险**: DataAsset 删除了某个字段后，SaveGame 里仍有旧 key → 只是无用数据，不报错也不影响功能
- **风险**: Slot 被覆盖 → 单 slot 足够，不需要多槽位
