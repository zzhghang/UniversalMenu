## Why

当前密码存储使用 SHA256 哈希，引入了不必要的 `PlatformCrypto` 依赖和复杂度。改为明文存储更简单，与 SaveGame 数据直接匹配。同时验证输入过滤（仅数字/仅字符/混合/最大字符数）的正确性。

## What Changes

- 移除 `GetHashedPassword()` 及 SHA256 哈希逻辑，密码以明文形式存取
- 移除 `PlatformCrypto` 模块依赖（`.Build.cs` + `.uplugin`）
- `W_MultiplayerScreen` 存储 Session 时不再对密码做特殊分支处理
- 验证 `SetText()` 和 `ValidateText()` 中 CharFilter / MaxCharCount 逻辑的边界正确性

## Capabilities

### New Capabilities
<!-- No new capabilities — this is a simplification of existing behavior -->
无

### Modified Capabilities
- `password-storage`: 密码存储方式从 SHA256 哈希改为明文，输入过滤逻辑保持不变但验证其正确性

## Impact

- `ListDataObject_Editable.h/.cpp` — 移除 `GetHashedPassword()`，移除 `#include "GenericPlatform/GenericPlatformMisc.h"`
- `W_MultiplayerScreen.cpp` — 简化密码值获取，移除 `bIsPassword` 分支
- `GameMainMenu.Build.cs` — 移除 `PlatformCrypto` 依赖
- `GameMainMenu.uplugin` — 移除 `PlatformCrypto` 插件依赖
- 影响面极小，无 API 变更，无破坏性
