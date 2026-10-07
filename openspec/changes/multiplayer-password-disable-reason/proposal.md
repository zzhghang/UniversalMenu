## Why

MultiplayerScreen 的 Password 条目当 HasPassword 为 false 时被禁用，但没有显示禁用原因给玩家，造成困惑。同时，当一个条目存在多个禁用条件时（如 PackagedBuildOnly + WindowMode 限制），DisableRichReason 直接拼接没有换行分隔，阅读体验差。此外，目前 EditCondition 的 DisableRichReason 和 DynamicDetails 都是硬编码在 C++ 中，无法通过 DataAsset 配置。

## What Changes

- 给 MultiplayerScreen 的 Password 和 HasPassword 条目增加 DisableRichReason，当 HasPassword=false 时显示禁用原因文本
- 新增本地化 key `Warning_PasswordDisabled` 用于密码禁用时的提示文本
- 修复 `UListDataObject_Base::IsDataCurrentlyEditable()`：多个 DisableRichReason 之间用换行符分隔，同时为 DynamicDetails 预留同样逻辑
- 在 `UMenuEntryConfig` 中增加 `DisableRichReason` 字段，让 DataAsset 可配置条目的禁用原因文本
- 在 `UMenuEntryConfig` 中增加 `DynamicDetails` 字段，让 DataAsset 可配置条目的动态详情文本
- 复活 `UW_OptionDetailView` 中被注释掉的 DynamicDetails 显示逻辑，改为从 DataObject 读取

## Capabilities

### New Capabilities
- `multiplayer-password-disable-reason`: 当 HasPassword=false 时，Password 条目显示禁用原因
- `edit-condition-multi-reason-format`: 多个 DisableRichReason 以换行分隔显示，DynamicDetails 同理
- `entry-config-disable-reason`: DataAsset 中可配置条目的 DisableRichReason 和 DynamicDetails

### Modified Capabilities
- （无现有 spec 需要修改）

## Impact

- `MultiplayerDataRegistry.cpp` — 给 Password/HasPassword 的 EditCondition 增加 DisableRichReason
- `LocalizationKeys.h` — 新增 `TXT_MP_WARN_PASSWORD_DISABLED` 宏
- `ListDataObject_Base.cpp` — `IsDataCurrentlyEditable()` 中多个 reason 之间加换行
- `ListDataObject_Base.h` — 增加 DynamicDetails 字段及 getter/setter
- `MenuEntryConfig.h` — 增加 `DisableRichReason` 和 `DynamicDetails` 配置字段
- `MenuConfigData.cpp` — `CreateEntryFromConfig` 中从 DataAsset 读取并设置 DisableRichReason 和 DynamicDetails
- `W_OptionDetailView.cpp` — 复活 DynamicDetails 显示逻辑，从 DataObject 读取文本
- `ST_Multiplayer` 字符串表 — 新增 `Warning_PasswordDisabled` 条目
