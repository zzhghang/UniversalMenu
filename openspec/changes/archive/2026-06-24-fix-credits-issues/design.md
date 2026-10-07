## Context

Credits 界面经上轮迭代实现了 logo 居中停止，但代码审查发现多个待修复问题。本次一次性修复全部（跳过功能除外）。

## Goals / Non-Goals

**Goals:**
- 修复 CreditsData 为空时 Timer 空转
- 修复 logo spacer 无 geometry fallback
- 消除重复 spacer 代码
- 行高从魔法数字改为可配置
- 滚动速度帧率无关

**Non-Goals:**
- 不添加跳过/加速功能
- 不换用 Slate USpacer（保持 CommonTextBlock spacer）
- 不改变 logo 居中算法本身

## Decisions

### Decision 1: 帧率无关用 GetDeltaSeconds

在 `UpdateScroll` 中用 `GetWorld()->GetDeltaSeconds()` 代替固定的 `ScrollTickInterval`。Timer 保持 ~60Hz 触发（提供足够精度），但滚动增量与帧率解耦。

### Decision 2: LineHeight 放 DataAsset

`LineHeight` 作为 `UCreditsDataAsset` 的 `UPROPERTY`，默认 30.f。与 ScrollSpeed、StartDelay 同级配置。向后兼容（已有 asset 自动获得默认值 30.f）。

### Decision 3: spacer 代码抽取为 AddSpacerLines

消除 PopulateCreditsEntries 和 StartScrolling 中的重复。

### Decision 4: geometry fallback 为半视口高度

当 `LogoH <= 0 || ViewportH <= 0` 时，`SpacerPx = ViewportH * 0.5f`。若 ViewportH 也为 0，跳过添加 spacer。

### Decision 5: logo 上方间距统一用 SetPadding

去掉 PopulateCreditsEntries 中 logo 前的 3 行 spacer，只保留 `SetPadding(FMargin(0, PaddingTop, 0, 0))`。

## Risks / Trade-offs

- [向后兼容] LineHeight 默认值 30.f 匹配原有行为 → 已有 asset 无损
- [GetDeltaSeconds] Timer 回调中 delta 可能为 0（首帧）→ 加最小值保护
