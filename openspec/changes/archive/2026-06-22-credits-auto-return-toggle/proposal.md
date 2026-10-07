## Why

当前 Credits 滚动动画播放完毕后无条件自动返回上一界面。在实际使用中，可能需要用户手动关闭（如停留在片尾查看），需要将此行为改为可配置。

## What Changes

- 在 `UW_CreditsScreen` 中新增 `bAutoReturnOnFinish` 布尔属性，默认为 `true`（保持现有行为）
- `OnCreditsFinished` 中判断该属性，仅当为 `true` 时调用 `DeactivateWidget()`
- 当为 `false` 时，滚动到底部停留，用户需手动返回

## Capabilities

### New Capabilities

- `credits-auto-return`: Credits 滚动结束后的自动返回行为可通过布尔属性控制

### Modified Capabilities

<!-- 无 -->

## Impact

- `W_CreditsScreen.h` — 新增 `bAutoReturnOnFinish` 属性 (UPROPERTY EditDefaultsOnly)
- `W_CreditsScreen.cpp` — OnCreditsFinished 中增加条件判断
