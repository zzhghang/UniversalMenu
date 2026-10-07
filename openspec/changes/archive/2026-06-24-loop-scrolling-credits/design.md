## Context

当前 `UW_CreditsScreen` 滚动是单向的，滚到底触发 `OnCreditsFinished`。需要无缝循环——最后一条从顶部消失的瞬间，第一条从底部出现，像传送带一样连续不断。

## Goals / Non-Goals

**Goals:**
- 在 DataAsset 中新增 `bLoopScrolling` 配置项
- 无缝循环：条目连续滚动，无跳跃、无长时间空白
- 循环模式下 `bAutoReturnOnFinish` 自然无效

**Non-Goals:**
- 不实现淡入淡出过渡动画
- 不修改 `bAutoReturnOnFinish` 的原有行为

## Decisions

### Decision 1: 配置放在 DataAsset

**选择**: `bLoopScrolling` 放在 `UCreditsDataAsset` 中。
**理由**: 与 ScrollSpeed/StartDelay 位置一致，便于按资产切换。

### Decision 2: 双份内容 + 偏移回卷

**选择**: `PopulateCreditsEntries` 填充两遍条目（拷贝1 + 拷贝2 上下堆叠）。`UpdateScroll` 到达 `ScrollEnd` 时 `CurrentScrollOffset -= ScrollEnd * 0.5f`，回卷到两份内容的中点。
**理由**: 两份相同内容堆叠 → 总高度 ≈ 2H。滚到底部（offset ≈ 2H）时回卷到 offset ≈ H，此时拷贝2 的视觉位置恰好等于拷贝1 在 offset=0 时的位置，形成无缝接续。最后一条从顶部消失 ↔ 第一条从底部出现，两者是同一瞬间。

### Decision 3: 不引入额外 Timer 或回调

**选择**: 循环判断直接在 `UpdateScroll` tick 中处理。
**理由**: 循环是滚动的延伸，同一帧内完成偏移回卷，不引入额外复杂性。

## Risks / Trade-offs

- [向后兼容] `bLoopScrolling` 默认 false，PassCount=1，行为不变
- [内存] 两份条目 Widget → 制作人员名单通常几十行，双份可忽略
