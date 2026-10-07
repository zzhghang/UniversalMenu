## Why

Credits 界面存在多个代码质量和功能缺陷：`CreditsData` 为空时 Timer 空转浪费性能、logo 居中 spacer 在极端情况下静默失败、spacer 创建代码重复、魔法数字硬编码不可配置、滚动速度帧率相关。

## What Changes

- **Bug 修复**: `CreditsData` 为空时 early return 避免 Timer 空转
- **Bug 修复**: logo 居中 spacer 计算增加 geometry 不可用时的 fallback
- **重构**: 抽出 `AddSpacerLines` 辅助函数消除重复代码
- **重构**: `ScrollTickInterval` 替换为 `GetWorld()->GetDeltaSeconds()` 实现帧率无关滚动
- **重构**: logo 上方间距统一用 Padding，去掉冗余 spacer
- **配置化**: 行高从 30.f 魔法数字改为 DataAsset 可配置属性

## Capabilities

### New Capabilities
- `credits-data-config`: DataAsset 新增 `LineHeight` 可配置属性

### Modified Capabilities
- 无

## Impact

- `W_CreditsScreen.h` — 新增 `AddSpacerLines` 方法
- `W_CreditsScreen.cpp` — 重构滚动逻辑、修复 bug、帧率无关 delta time
- `CreditsDataAsset.h` — 新增 `LineHeight` 属性
