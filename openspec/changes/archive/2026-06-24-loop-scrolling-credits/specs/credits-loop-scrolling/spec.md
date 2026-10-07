## ADDED Requirements

### Requirement: DataAsset 可配置循环滚动
`UCreditsDataAsset` SHALL 提供 `bLoopScrolling` 布尔属性，默认值为 `false`，用于控制 Credits 是否循环滚动。

#### Scenario: 启用循环——追加空白间隔
- **WHEN** `bLoopScrolling` 为 `true` 且滚动即将开始
- **THEN** 系统在条目末尾追加空白 Spacer，高度约等于视口高度，确保上一轮完全滚出后有一段空白期

#### Scenario: 启用循环——到底重置
- **WHEN** `bLoopScrolling` 为 `true` 且 `CurrentScrollOffset >= ScrollEnd`
- **THEN** `CurrentScrollOffset` 重置为 0，内容从底部重新开始滚动

#### Scenario: 禁用循环
- **WHEN** `bLoopScrolling` 为 `false` 且 Credits 滚动到底部
- **THEN** 系统触发 `OnCreditsFinished`，行为与现有逻辑一致

### Requirement: 循环模式下不触发完成回调
系统 SHALL 在循环滚动启用时，永不调用 `OnCreditsFinished`。

#### Scenario: 循环到底不触发完成
- **WHEN** `bLoopScrolling` 为 `true` 且 `CurrentScrollOffset >= ScrollEnd`
- **THEN** 偏移量被重置，`OnCreditsFinished` 不被调用，滚动持续进行
