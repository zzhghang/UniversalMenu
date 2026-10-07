## Context

- `UW_ListEntry_Session` 展示 `UListDataObject_Session`(承载 `FSessionListItem`),条目数据由蓝图通过 `PopulateSessionList(TArray<FSessionListItem>)` 传入,经 `UMultiplayerDataRegistry::RebuildSessionList` 构建列表项。
- 基类 `UW_ListEntry_Base::NativeOnItemSelectionChanged` 目前只切换高亮状态,没有可供蓝图处理的选中回调。
- `FSessionListItem` 目前只有展示字段,没有携带搜索得到的 `FBlueprintSessionResult`,蓝图无法把选中条目关联到 Join Session 节点所需的会话结果。
- `GameMainMenu` 模块当前未依赖 `OnlineSubsystem`。

## Goals / Non-Goals

**Goals:**

- 条目被选中时调用蓝图实现事件,让蓝图侧完成"选中后加入"的流程。
- 条目携带并暴露 `FBlueprintSessionResult`,供蓝图的 Join Session 节点使用。

**Non-Goals:**

- 不在 C++ 侧实现实际的 Join Session 调用(由蓝图完成)。
- 不实现 Find Sessions 搜索逻辑(搜索在蓝图侧,结果经现有 `PopulateSessionList` 传入)。
- 不处理密码输入、连接失败等加入流程细节(蓝图侧职责)。

## Decisions

1. **会话结果存放在 `FSessionListItem` 新增的 `FBlueprintSessionResult` 字段**
   复用现有数据链路(`PopulateSessionList` → `RebuildSessionList` → `UListDataObject_Session`),蓝图构建条目时直接填充,无需新的映射表或注册表改动。
   备选:在 screen 上单独维护 SelectedItem → SessionResult 映射 —— 多余,数据本就该随条目走。

2. **条目 widget 在 `OnOwningListDataObjectSet` 中缓存结果,`GetSessionResult()` BlueprintPure 暴露**
   蓝图实现 `BP_OnSessionEntrySelected` 时 `self` 即条目 widget,可直接取变量喂给 Join Session 节点。
   备选:把结果作为事件参数传入 —— 同样可行,但用户明确要求"用于加入游戏的 session 变量",getter + 变量更直观,且与模块现有 BP 事件模式一致。

3. **触发方式:暂无(已撤销)**
   条目内按钮(`Cbb_Join`)方案被用户撤销,不绑定任何触发入口;本次改动仅保留会话结果的数据传递与暴露(`GetSessionResult`)。加入操作的触发方式待用户决定后另行实现。

4. **`GameMainMenu.Build.cs` 增加 `OnlineSubsystem` + `OnlineSubsystemUtils` 依赖**
   UE 5.7 中 `FBlueprintSessionResult` 定义在 `OnlineSubsystemUtils` 插件的 `FindSessionsCallbackProxy.h`(旧版 Engine 的 `OnlineBlueprintSupport.h` 已移除)。`FOnlineSessionSearchResult` 内部的 `FVariantData` 等符号来自 `OnlineSubsystem` DLL,需显式链接 `OnlineSubsystem`(引擎侧样例同样同时列出两个模块)。仅头文件/链接使用,无运行时逻辑耦合。

## Risks / Trade-offs

- [增加 `OnlineSubsystem` 模块依赖] → 仅用于头文件包含,加入逻辑仍在蓝图侧,不引入 C++ 侧运行时耦合。
- [`InitRegistry` 中硬编码的测试数据没有真实 `SessionResult`] → 此时条目仍正常展示与触发事件,蓝图侧自行判断结果有效性(测试数据属临时方案,后续由真实搜索结果替换)。
- [蓝图未实现 `BP_OnSessionEntrySelected` 时选中无额外行为] → 与现有 BlueprintImplementableEvent 模式一致,无运行时风险。
