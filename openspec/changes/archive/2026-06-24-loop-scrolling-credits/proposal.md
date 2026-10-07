## Why

Credits界面目前只能单次滚动到底部后停止或返回上一界面。当制作人员名单需要持续展示（如展会、主菜单背景等场景）时，需要名单循环滚动、周而复始。同时循环滚动应覆盖自动返回逻辑——循环模式下不应自动返回上一界面。

## What Changes

- 在 `UCreditsDataAsset` 中新增 `bLoopScrolling` 配置项，控制是否启用循环滚动
- `UpdateScroll` 中增加循环判断：当滚动到末尾且 `bLoopScrolling` 为 true 时，重置滚动偏移到顶部重新开始，而非触发 `OnCreditsFinished`
- 循环模式下 `OnCreditsFinished` 不会被调用，因此 `bAutoReturnOnFinish` 的效果被自然覆盖

## Capabilities

### New Capabilities
- `credits-loop-scrolling`: 制作人员名单循环滚动功能，滚动到底部后自动回到顶部重新开始

### Modified Capabilities
- `credits-auto-return`: 循环滚动启用时，自动返回行为被覆盖（不返回上一界面）

## Impact

- `UCreditsDataAsset`（CreditsDataAsset.h）——新增属性
- `UW_CreditsScreen::UpdateScroll`（W_CreditsScreen.cpp）——滚动逻辑修改
- Blueprint中已有的 `UCreditsDataAsset` 资产——新属性默认为 false，向后兼容
