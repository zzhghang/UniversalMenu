## Why

CreditsScreen 中 Back 按钮的处理逻辑是多余的——该界面本身不需要独立的返回操作（退出由其他机制处理）。删除这部分无用代码，保持代码库整洁。

## What Changes

- 移除 `W_CreditsScreen` 中的 `Button_Back` 成员
- 移除 `OnBackClicked()` 函数声明与实现
- 移除 `NativeOnInitialized` 中对按钮点击事件的绑定
- 移除不再需要的 `UCommonButtonBase` 前向声明和 include

## Capabilities

### New Capabilities

无。这是纯清理，不引入新功能。

### Modified Capabilities

无。不涉及需求变更。

## Impact

- `W_CreditsScreen.h` — 移除 `Button_Back` 属性、`OnBackClicked` 声明、`UCommonButtonBase` 前向声明
- `W_CreditsScreen.cpp` — 移除 `OnBackClicked` 实现、`NativeOnInitialized` 中的按钮绑定、`CommonButtonBase.h` include
