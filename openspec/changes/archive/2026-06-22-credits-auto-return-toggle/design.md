## Context

当前 `OnCreditsFinished()` 无条件调用 `DeactivateWidget()` 返回上一界面。需要增加开关控制此行为。

## Goals / Non-Goals

**Goals:**
- 新增 `bAutoReturnOnFinish` 布尔属性，默认 `true`
- 当为 `false` 时，滚动结束停留在当前页面

**Non-Goals:**
- 不改变滚动机制
- 不增加新的 UI 元素（当 `false` 时，用户通过已有的返回方式关闭）

## Decisions

**选择：** 在 `UW_CreditsScreen` 中直接添加 `UPROPERTY(EditDefaultsOnly) bool bAutoReturnOnFinish = true`

**理由：** 极简实现，蓝图子类可直接在 Details 面板中切换，无需新增任何依赖或复杂逻辑。

## Risks / Trade-offs

- `bAutoReturnOnFinish = false` 时用户需手动返回，但如果界面没有返回按钮（之前已移除 Button_Back），用户可能无法退出 → 需在蓝图层确保提供退出途径（如手柄 B 键绑定）
