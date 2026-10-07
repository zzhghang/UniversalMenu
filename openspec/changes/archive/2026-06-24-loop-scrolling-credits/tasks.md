## 1. DataAsset 配置

- [x] 1.1 在 `UCreditsDataAsset` 中新增 `bLoopScrolling` 布尔属性（默认 false），位于 `StartDelay` 下方

## 2. 双份内容填充

- [x] 2.1 修改 `PopulateCreditsEntries`：循环模式下填充两遍条目（两份相同内容上下堆叠），为偏移回卷提供基础

## 3. 无缝回卷逻辑

- [x] 3.1 修改 `UpdateScroll`：到达 ScrollEnd 时 `CurrentScrollOffset -= ScrollEnd * 0.5f`，从第二份拷贝起始处继续，视觉上最后一帧与第一帧接续，无跳跃
