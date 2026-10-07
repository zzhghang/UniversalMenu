## Context

当前 `W_CreditsScreen` 中包含 `Button_Back` 成员及对应的 `OnBackClicked()` 处理函数。该按钮的实际功能仅是调用 `DeactivateWidget()`，而 CreditsScreen 的退出已有其他机制覆盖，不需要独立处理返回操作。

## Goals / Non-Goals

**Goals:**
- 从 `W_CreditsScreen` 中移除所有 Back 按钮相关代码
- 移除不再需要的 `UCommonButtonBase` 引用

**Non-Goals:**
- 不改变 CreditsScreen 的任何其他行为
- 不影响 UI 蓝图中的 Button_Back 控件存在（蓝图侧自行处理）

## Decisions

直接删除：`OnBackClicked` 仅调用 `DeactivateWidget()`，移除并不会影响 CreditsScreen 的退出流程，因为 `NativeOnDeactivated` 中已有定时器清理逻辑，退出由父类或外部调用 `DeactivateWidget()` 触发。

## Risks / Trade-offs

- 如果蓝图仍绑定 Button_Back 且没有其他处理逻辑，按钮点击将无响应 → 蓝图侧需确保 Button_Back 有独立的事件处理（或直接在蓝图中移除该按钮）
