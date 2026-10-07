## 1. 移除 SHA256 哈希

- [x] 1.1 `ListDataObject_Editable.h`: 移除 `GetHashedPassword()` 声明
- [x] 1.2 `ListDataObject_Editable.cpp`: 移除 `GetHashedPassword()` 实现及 `#include "GenericPlatform/GenericPlatformMisc.h"`
- [x] 1.3 `W_MultiplayerScreen.cpp`: `SaveSessionData()` 中移除 `bIsPassword` 分支，统一用 `GetText().ToString()`
- [x] 1.4 `GameMainMenu.Build.cs`: 移除 `PlatformCrypto` 依赖
- [x] 1.5 `GameMainMenu.uplugin`: 移除 `PlatformCrypto` 插件依赖

## 2. 验证输入过滤正确性

- [x] 2.1 验证 `SetText()` 在 NumbersOnly / LettersOnly / Alphanumeric / All 四种模式下过滤行为正确
- [x] 2.2 验证 `SetText()` 在过滤后再截断的行为（过滤后超出 MaxCharCount 的场景）
- [x] 2.3 验证 `ValidateText()` 与 `SetText()` 校验规则一致，OutReason 正确
- [x] 2.4 验证 `OnDataObjectInitialized()` 中 MaxCharCount 默认值逻辑（密码 6，普通 16）

## 3. 编译验证

- [ ] 3.1 编译项目确认无错误，`PlatformCrypto` 依赖警告已消除
