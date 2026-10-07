## Context

当前 `UListDataObject_Editable` 通过 `GetHashedPassword()` 对密码做 SHA256 哈希后存入 `UMenuSessionData`。这引入了不必要的 `PlatformCrypto` 模块依赖和复杂性。Switch 这类主机平台的 SaveGame 数据本身就在沙箱内，明文存储无额外风险。

`SetText()` 和 `ValidateText()` 中已实现了 `EEditableCharFilter` 的三种过滤模式（NumbersOnly / LettersOnly / Alphanumeric）和 `MaxCharCount` 截断。需求是验证这些逻辑的边界正确性。

## Goals / Non-Goals

**Goals:**
- 移除 SHA256 密码哈希，改用明文存取
- 从 `GameMainMenu` 移除 `PlatformCrypto` 依赖
- 验证 `CharFilter` + `MaxCharCount` 逻辑在所有边界条件下正确

**Non-Goals:**
- 不改变密码加解密机制（暂无需求）
- 不改变 UI 输入控件的行为
- 不添加新的过滤模式

## Decisions

1. **直接使用 `GetText()` 替代 `GetHashedPassword()`**
   - `W_MultiplayerScreen::SaveSessionData()` 中移除 `bIsPassword` 分支，统一走 `GetText()`
   - `GetHashedPassword()` 方法本身移除，`#include "GenericPlatform/GenericPlatformMisc.h"` 一并移除
   - `bIsPassword` 属性保留，仍控制 UI 中密码掩码显示（`SetIsPassword`）

2. **CharFilter / MaxCharCount 验证**
   - `SetText()` 在写入时已做过滤+截断，行为正确
   - `ValidateText()` 用于 UI 预检，逻辑与 `SetText` 一致且已覆盖四种过滤模式
   - 无需改动 — 仅做回归确认

3. **移除 PlatformCrypto 依赖**
   - `.Build.cs` 中移除 `"PlatformCrypto"`
   - `.uplugin` 中移除 `PlatformCrypto` 条目

## Risks / Trade-offs

- 密码以明文存入 SaveGame 文件 — 无实际风险，SaveGame 在主机沙箱内，且当前未做网络传输
