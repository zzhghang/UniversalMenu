# GameMainMenu 插件代码检查报告

- 检查日期：2026-10-12
- 检查范围：`Plugins/GameMainMenu` 的 C++ 源码（Public / Private / Tests）、`GameMainMenu.uplugin`、`GameMainMenu.Build.cs`、插件与项目的 `DefaultGameMainMenu.ini`
- 未覆盖：Blueprint / `.uasset` 内部逻辑（WBP、DataAsset 实际配置）、`doc` 目录下既有文档（按要求未参考）
- 说明：本次为静态审查，未在本机编译或运行 UE Automation Tests。标注“需验证”的条目依赖蓝图或运行时环境，建议实机确认。

## 总体评价

插件整体架构清晰：`DataAsset → Registry → ListDataObject → ListEntry Widget` 的数据驱动链路设计合理，Options 的“草稿设置（PendingSettings）+ Back 时提交”模式、多人表单的持久化/条件系统、旧资产字段向 `ValueConfig` 的兼容迁移都考虑得比较周到，并且已有 20 个左右的自动化测试覆盖核心数据逻辑。

主要问题集中在四类：

1. **异步回调里的对象生命周期**：多处 lambda 捕获裸 `this`，在列表项复用、Widget 销毁后可能崩溃或作用到错误的对象上。
2. **改键流程的逻辑缺陷**：冲突提示后并未解除原绑定、目标槽位选择不准确、未绑定按键的槽位类型判断错误。
3. **设置项“只存不生效”**：多项音频/字幕设置只写入配置，没有任何运行时效果；音量依赖的 SoundMix 未在 C++ 中 Push。
4. **数据驱动不彻底**：仍有大量按 `DataId` / `BindingKey` 字符串硬编码的特殊分支，配置写错时静默失效。

## 优先级定义

| 级别 | 含义 |
| --- | --- |
| P0 | 可能导致崩溃、数据错写，或明显错误的用户可见行为，建议尽快修复 |
| P1 | 功能缺陷或较大隐患，建议在下一个迭代修复 |
| P2 | 性能、可维护性、健壮性改进 |
| P3 | 代码风格、命名、文档类改进 |

## 问题汇总

| # | 级别 | 模块 | 问题 |
| --- | --- | --- | --- |
| 1 | P0 | KeyRemap | 改键弹窗的延迟回调捕获裸 `this`，且同一帧内多次按键会重复回调 |
| 2 | P0 | KeyBinding | 冲突确认回调捕获列表项 Widget，列表项被复用后可能把按键绑定到错误动作 |
| 3 | P0 | KeyBinding | 冲突确认“重新绑定”后，冲突动作仍保留同一个按键 |
| 4 | P0 | LoadingScreen | `HoldLoadingScreenTime <= 0` 时 `bDisableWorldRendering` 永远不会恢复 |
| 5 | P0 | UI Subsystem / AsyncAction | 多处对 `UFrontendUISubsystem::Get()` 结果直接解引用，`Get()` 本身使用 Assert 模式 |
| 6 | P1 | KeyBinding | 槽位类型按“当前按键”判断，未绑定的手柄槽位会被当成键鼠槽位 |
| 7 | P1 | KeyBinding | 点击的槽位索引没有传给绑定逻辑；同时存在两套改键输入路径 |
| 8 | P1 | Settings | 多项音频/字幕设置只保存不生效；音量 SoundMix 未 Push |
| 9 | P1 | Settings | 每次 Apply 都无条件 `SetCurrentCulture`，首次启动会覆盖系统语言 |
| 10 | P1 | Options | `InitFromDataAsset` 缺少空指针检查，DataAsset 数组中有空元素会崩溃 |
| 11 | P1 | Options | `BindingKey` 写错时静默失效，没有警告或编辑器校验 |
| 12 | P1 | Options | VSync / 分辨率的“强制值”会写入草稿，切换窗口模式后用户原值丢失 |
| 13 | P1 | Controller | 硬件基准测试放在 `OnPossess`，客户端不会执行且会造成同步卡顿 |
| 14 | P1 | Multiplayer | 密码字段默认以明文持久化到 SaveGame |
| 15 | P1 | Build | `uplugin` 未声明 EnhancedInput / OnlineSubsystem 等插件依赖 |
| 16 | P2 | 数据对象 | `IsDataCurrentlyEditable()` 名为查询却带副作用 |
| 17 | P2 | ListEntry | 列表项回收时未解绑旧数据对象的委托 |
| 18 | P2 | 性能 | 立即生效的滑条每帧执行完整 `ApplyNonResolutionSettings` |
| 19 | P2 | LoadingScreen | 每帧广播阶段事件、同步加载映射资产 |
| 20 | P2 | UI Subsystem | PrimaryLayout 强引用且只能注册一次；推送请求无去重 |
| 21 | P2 | 配置系统 | 按字符串匹配 `BindingKey` 决定数据对象类型，硬编码 Tab / Entry ID |
| 22 | P2 | 重复代码 | OptionScreen / MultiplayerScreen、两个 Registry、两个改键入口大量重复 |
| 23 | P2 | Widgets | 运行时用 `NewObject` 创建 UMG 控件，应使用 `WidgetTree->ConstructWidget` |
| 24 | P2 | Confirm | `NativeGetDesiredFocusTarget` 在 const getter 中设置焦点且返回空 |
| 25 | P2 | Credits | 用 0.016s 定时器滚动、用空白文本行填充间距，且文案不可本地化 |
| 26 | P2 | Scalar | 数值格式固定为无小数，无法正确显示 0.5～2.0 这类配置 |
| 27 | P2 | 模块启动 | StringTable 在模块启动时同步加载，cook 是否包含需验证 |
| 28 | P2 | 日志 | 全部使用 `LogTemp`，调试辅助头被大量运行时代码包含 |
| 29 | P3 | 命名 / 风格 | 拼写错误、类名双 U、混合版权头、裸指针 UPROPERTY 等 |
| 30 | P3 | 测试 | 缺少 LoadingScreen、改键、UI 栈相关测试 |

---

## P0：建议尽快修复

### 1. 改键弹窗：延迟回调捕获裸 `this`，可能重复触发

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/W_KeyRemapScreen.cpp:44`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/W_KeyRemapScreen.cpp:171`

问题：

- `RequestDeactivationWidget` 通过 `FTSTicker` 在下一帧执行回调，lambda 捕获的是裸 `this`。如果在这一帧内 Widget 被移除或回收（切关卡、栈被清空），会访问悬空指针。
- InputPreprocessor 在检测到第一个有效按键后并没有立即注销，也没有设置“已完成”标记。同一帧内的多次 KeyDown / MouseDown（如组合键、按键重复）会注册多个 ticker，导致 `OnKeyRemapScreenKeyPressed` 被调用多次、`DeactivateWidget` 被调用多次。

建议：

```cpp
void UW_KeyRemapScreen::RequestDeactivationWidget(TFunction<void()> PreDeactivationCallback)
{
    if (bDeactivationRequested) return;
    bDeactivationRequested = true;

    if (CachedKeyRemapInputPreprocessor.IsValid())
    {
        FSlateApplication::Get().UnregisterInputPreProcessor(CachedKeyRemapInputPreprocessor);
        CachedKeyRemapInputPreprocessor.Reset();
    }

    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this,
        [this, PreDeactivationCallback = MoveTemp(PreDeactivationCallback)](float)
        {
            PreDeactivationCallback();
            DeactivateWidget();
            return false;
        }));
}
```

并在 `NativeOnActivated` 中重置 `bDeactivationRequested`。

### 2. 冲突确认回调捕获列表项 Widget，可能绑定到错误动作

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_KeyBinding.cpp:236`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_KeyBinding.cpp:308`

问题：

- `ListView` 会回收并复用 Entry Widget。冲突确认框是异步加载和推送的，用户确认时，原 Entry 可能已经被复用于另一行数据。
- 回调里使用的是 `CachedOwningKeyBindingDataObject`（当前 Entry 绑定的数据），而不是发起改键时的数据对象。结果可能是“为 A 动作改键，却写到了 B 动作上”。
- `PushSoftWidgetToStackAsync` 的回调同样捕获裸 `this`，Entry 销毁后访问 `GetOwningPlayer()` 有崩溃风险。

建议：在发起请求时捕获目标数据对象和槽位，而不是捕获 Widget：

```cpp
const TWeakObjectPtr<UUListDataObject_KeyBinding> WeakData = CachedOwningKeyBindingDataObject;
const int32 TargetSlot = FoundSlotIndex;

[WeakData, TargetSlot, PressedKey](EConfirmScreenButtonType Result)
{
    if (Result == EConfirmScreenButtonType::Confirmed && WeakData.IsValid())
    {
        WeakData->SetKeyForSlot(TargetSlot, PressedKey);
    }
}
```

推送 Remap 界面时也用 `TWeakObjectPtr<UW_ListEntry_KeyBinding>` 或直接把数据对象传给 Remap 界面。

### 3. “重新绑定”后，冲突动作仍保留同一按键

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_KeyBinding.cpp:301`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/UListDataObject_KeyBinding.cpp:176`

问题：确认框文案是“已被某动作使用，是否重新绑定”，但确认后只给当前动作设置了新键，冲突动作没有被清空或交换。最终两个动作绑定同一个按键，再次打开时冲突依旧存在。

建议：

- `FindConflictingAction` 返回冲突映射的完整标识（MappingName + Slot），而不只是名称。
- 确认后执行以下策略之一，并在文案中明确说明：
  - 交换：冲突动作改为当前动作的旧按键；
  - 解绑：冲突动作设为 `EKeys::Invalid`。
- 修改冲突动作后，需要通知对应的数据对象刷新 UI。由于多个数据对象共享同一个草稿 Profile，建议在 Registry 层增加“按 MappingName 查找数据对象并广播修改”的能力。

### 4. `HoldLoadingScreenTime <= 0` 时世界渲染不会恢复

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Subsystems/FrontendLoadingScreenSubsystem.cpp:112`

问题：

- 显示分支中通过 `bDisableWorldRendering = (Phase != TextureStreaming)` 控制渲染。
- 隐藏分支（`else`）没有把 `bDisableWorldRendering` 恢复为 `false`。
- 当前之所以能恢复，是因为 Hold 时间内会进入“TextureStreaming 阶段”的显示分支并把它设为 `false`。如果项目把 `HoldLoadingScreenTime` 配成 `0`，或首次计算时 `ElapsedTime < HoldTime` 不成立，会直接进入隐藏分支，世界保持黑屏。

建议：在 `TryRemoveLoadingScreen()` 中显式恢复：

```cpp
if (UGameViewportClient* ViewportClient = GetGameInstance()->GetGameViewportClient())
{
    ViewportClient->bDisableWorldRendering = false;
}
```

### 5. `UFrontendUISubsystem::Get()` 的空值与断言风险

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Subsystems/FrontendUISubsystem.cpp:14`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/AsyncActions/AA_PushSoftWidget.cpp:40`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/AsyncActions/AA_PushConfirmScreen.cpp:33`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/W_OptionScreen.cpp:113`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Components/FrontendCommonButtonBase.cpp:28`

问题：

- `Get()` 使用 `EGetWorldErrorMode::Assert`，上下文无效时直接断言。
- 在专用服务器上 `ShouldCreateSubsystem` 返回 `false`，`Get()` 会返回 `nullptr`，但多处调用方直接 `Get(this)->...`。
- `PushSoftWidgetToStackAsync` 内部任意一个 `ensure` 失败时会直接返回，不会触发回调。`UAA_PushSoftWidget` / `UAA_PushConfirmScreen` 因此永远不会调用 `SetReadyToDestroy()`，异步节点会一直挂在 GameInstance 上。
- `AddWidget` 失败时 `AfterPush` 回调会收到 `nullptr`，`AA_PushSoftWidget.cpp:56` 会直接解引用。

建议：

- `Get()` 改为 `LogAndReturnNull`，调用方统一判空。
- `PushSoftWidgetToStackAsync` 增加失败回调（例如 `EAsyncPushWidgetState::Failed`），AsyncAction 在失败时也调用 `SetReadyToDestroy()`。
- Confirm 界面如果被 Back 键或栈清理关闭，也应以 `Closed` 调用回调，避免异步节点泄漏（需结合蓝图中的 Back Handler 设置验证）。

---

## P1：功能缺陷与较大隐患

### 6. 未绑定的手柄槽位会被识别为键鼠槽位

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:309`

```cpp
const bool bIsGamepadKey = PlayerMapping->GetCurrentKey().IsGamepadKey();
Slot.bAllowKeyboardAndMouse = !bIsGamepadKey;
Slot.bAllowGamepad = bIsGamepadKey;
```

如果某个手柄映射当前为空（`EKeys::Invalid`），`IsGamepadKey()` 为 `false`，该槽位会被标记为键鼠槽位。之后手柄按钮无法再绑定到这个动作。

建议：优先使用 `GetPrimaryDeviceType()`（上方已在用）或 `GetDefaultKey()` 判断设备类型，当前按键仅作兜底。

### 7. 点击的槽位没有传给绑定逻辑；存在两套改键路径

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_KeyBinding.cpp:211`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/UListDataObject_KeyBinding.cpp:139`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_KeyBinding.cpp:73`

问题：

- `OnKeyboardSlotClicked` 计算出了 `FoundSlotIndex`，但只用于判断是否存在，最终调用的 `BindNewInputKey` 会重新选择“第一个兼容槽位”。一旦配置了主键 / 副键（Slot First / Second），永远只能修改第一个。
- 同一个 Entry 中还有一套 `StartListeningForSlot` + `NativeOnKeyDown` 的内联监听路径。这条路径不做冲突检测，排除键规则也与 Remap 界面不同。两套逻辑行为不一致，后续维护容易出错。

建议：

- 统一为一个入口 `OpenRemapForSlot(int32 SlotIndex)`，并把 `SlotIndex` 一路传到 `SetKeyForSlot`。
- 若内联监听已不再使用，删除它；若蓝图仍在使用，让它复用同一套冲突检测和过滤规则。

### 8. 多项设置只保存、不生效；音量 SoundMix 未 Push

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/FrontendSettings/FrontendGameUserSettings.cpp:107`
- `Plugins/GameMainMenu/Source/GameMainMenu/Public/FrontendSettings/FrontendGameUserSettings.h:157`

问题：

- `AllowBackgroundSound`、`HRTFEnabled`、`MonoOutput`、`DynamicRangeCompression`、`DialogueBoost`、`SubtitlesEnabled` 在 C++ 中只有 Getter / Setter，没有任何应用逻辑。用户修改后看不到效果。
- `ShowSubtitles`（Gameplay）和 `SubtitlesEnabled`（Audio）是同一概念的两个独立设置，容易互相矛盾。
- 音量通过 `SetSoundMixClassOverride` 设置，但 C++ 中没有 `PushSoundMixModifier` / `SetBaseSoundMix`。如果蓝图也没有 Push 该 SoundMix，所有音量滑条都不起作用（需验证蓝图）。
- 只有存在 Game / PIE World 时才应用音量；启动阶段如果没有 World，保存的音量不会生效。关卡切换后 SoundMix 状态也需要实机验证。
- `SetCurrentDisplayGamma` 没有范围限制，配置文件被篡改后可写入极端值。

建议：

- 为每项设置补充应用逻辑，例如背景声音使用 `FApp::SetUnfocusedVolumeMultiplier(bAllowBackgroundSound ? 1.f : 0.f)`，字幕使用 `UGameplayStatics::SetSubtitlesEnabled`；暂时无法实现的设置应从 UI 中移除或标注为由游戏层读取。
- 合并两个字幕设置。
- 在应用音量前确保 SoundMix 已 Push，并在 `PostLoadMapWithWorld` 后重新应用一次。
- Gamma 设置合理范围，如 `[1.0, 3.0]`。

### 9. 每次 Apply 都会强制设置文化

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/FrontendSettings/FrontendGameUserSettings.cpp:111`

问题：

- 任何设置项在“立即生效”时都会调用 `ApplyNonResolutionSettings`，从而重复调用 `FInternationalization::SetCurrentCulture`，触发全量文本刷新（拖动音量滑条时尤为明显）。
- `CurrentCulture` 默认为 `"en"`，首次启动会覆盖系统语言，中文系统用户首次进入也会显示英文。

建议：

- 仅在与当前文化不一致时设置，并考虑使用 `SetCurrentLanguageAndLocale`。
- 默认值改为空字符串，表示“跟随系统”，首次启动根据 `SupportedCultures` 与系统语言匹配。

### 10. Options 初始化缺少空指针检查

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:339`

`InitFromDataAsset` 与 `ApplyTabEditConditions` 直接访问 `TabData->TabId`、`CatData->Entries`、`EntryData->BindingKey`。只要 DataAsset 数组里留有一个空元素（编辑器中很容易出现），打开设置界面就会崩溃。Multiplayer 的 Registry 已经做了判空和重复 ID 检查，Options 侧没有。

建议：与 `UMultiplayerDataRegistry::InitFromConfig` 对齐，增加判空、重复 `TabId` / `DataId` 检测和日志。

### 11. `BindingKey` 写错时静默失效

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:355`

`ResolveBinding` 找不到时不输出任何信息，设置项仍会显示，但修改后不会写入任何配置，问题很难定位。

建议：

- 找不到绑定时输出 Warning（包含 Tab / DataId / BindingKey）。
- 为 `UMenuEntryConfig` / `UMenuConfigData` 实现 `IsDataValid`，在编辑器保存时校验 `BindingKey`、重复 ID、空引用、依赖 ID 是否存在。
- 长期可考虑用 `FName` 下拉（`GetOptions` meta）替代手填字符串。

### 12. 强制值写入草稿，导致用户原值丢失

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:452`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:467`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_Base.cpp:45`

问题：

- 切到无边框窗口时，分辨率被强制为最大分辨率；切到非全屏时，VSync 被强制为 `false`。这些强制值通过 `OnSetForcedStringValue` 写入 Binding（草稿设置）。
- 用户切回其他模式后，原来的分辨率 / VSync 选择已经丢失。
- 另外，现代平台在窗口模式下 VSync 通常也有效，“仅全屏可用”的限制是否必要值得重新评估。

建议：禁用状态下只改变显示值，不覆盖底层设置；或在进入禁用状态前缓存原值，恢复可编辑时还原。

### 13. 硬件基准测试放在 `OnPossess`

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Controllers/FrontendPlayerController.cpp:7`

问题：

- `OnPossess` 只在服务器端执行，多人游戏中的客户端永远不会运行基准测试。
- `RunHardwareBenchmark()` 是同步操作，会在进入地图时造成明显卡顿。
- 与 Controller 无关的“首次启动硬件检测”被绑定到了 Pawn 控制流程上。

建议：移到 `UFrontendGameInstance::Init()` 或主菜单首次显示前执行，只对本地玩家执行一次；最好在加载界面期间完成，并把 `== -1.f` 改为 `< 0.f`。

### 14. 密码字段默认明文持久化

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Widgets/MenuConfig/MenuEntryConfig.h:84`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:226`

`Persistence` 默认为 `Persistent`，`bIsPassword` 的输入框也会以明文写入 `MenuSession` 存档。

建议：对 `bIsPassword == true` 的条目强制使用 `SessionOnly` 或 `None`，并在编辑器校验中提示。

另外，`ListDataObject_Editable.cpp:106` 中密码默认最大长度为 6 是比较意外的硬编码，建议改为显式配置项或更宽松的默认值。

### 15. `uplugin` 未声明实际依赖的插件

位置：

- `Plugins/GameMainMenu/GameMainMenu.uplugin`
- `Plugins/GameMainMenu/Source/GameMainMenu/GameMainMenu.Build.cs`

`Build.cs` 依赖 `EnhancedInput`、`OnlineSubsystem`、`OnlineSubsystemUtils`、`CommonInput` 等模块，它们在 UE5 中都属于插件，但 `.uplugin` 只声明了 `CommonUI` 和 `AdvancedSessions`。UBT 会给出“未声明插件依赖”的警告，插件单独迁移到其他项目时也可能加载失败。

建议：

- 在 `.uplugin` 中补齐 `EnhancedInput`、`OnlineSubsystem`、`OnlineSubsystemUtils`。
- `Build.cs` 中只有公共头文件需要的模块放在 Public，其余移到 Private。目前反过来：公共头大量使用 `Engine` / `CoreUObject` 类型，它们却在 Private 中；`PreLoadScreen`、`OnlineSubsystem` 等只在 cpp 中使用，却放在 Public 中。

---

## P2：性能、健壮性与可维护性

### 16. `IsDataCurrentlyEditable()` 是带副作用的“查询”

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_Base.cpp:24`

该函数会：

- 修改 `DisableRichText`；
- 写入强制值，并广播 `OnListDataModified`。

但它在 Detail 面板刷新、列表项设置、Tab 切换、Reset 状态计算等多个地方被当作纯查询调用，会引起重复广播和连锁刷新，也让第 12 条的问题更难排查。此外，`ApplyCommonConfig` 中从配置设置的 `DisableRichText` 会被条件原因覆盖。

建议拆分为：

- `bool IsDataCurrentlyEditable() const`：只计算；
- `FText GetDisabledReason() const`：只计算；
- `void ApplyEditConditions()`：在依赖项变化时显式调用，负责写入强制值。

### 17. 列表项回收时没有解绑旧数据对象

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_Base.cpp:40`

`OnOwningListDataObjectSet` 会绑定 `OnListDataModified` / `OnDependencyDataModified`，但 `NativeOnEntryReleased` 没有解绑，也没有清空 `CachedOwningListDataObject`。Entry 被复用后仍会收到旧数据对象的事件，并用新对象刷新 UI；绑定数量会随滚动和切 Tab 逐渐增加。

建议：在 `NativeOnEntryReleased` 中对旧对象执行 `RemoveAll(this)` 并清空缓存。

### 18. 立即生效的滑条每帧执行完整 Apply

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/ListEntries/W_ListEntry_Scalar.cpp:43`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:133`

拖动滑条时每次 `OnValueChanged` 都会：字符串往返转换 → 写草稿 → 广播 → `ApplyNonResolutionSettings()`。后者会同步加载 SoundMix / SoundClass、设置文化、应用画质等，开销与单个设置项无关。

建议：

- 立即生效时只应用当前 Key 对应的效果（例如音量只更新对应 SoundClass）。
- 或在 `OnMouseCaptureEnd` / 手柄松开时再执行完整 Apply。
- 缓存 SoundMix / SoundClass，避免反复 `LoadSynchronous`。

### 19. LoadingScreen 子系统的小问题

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Subsystems/FrontendLoadingScreenSubsystem.cpp`

- `:124`：加载期间每帧广播 `OnLoadingPhaseUpdated`，蓝图监听者每帧执行。建议只在阶段变化时广播。
- `:329`：`GetLoadingPhaseDisplayInfo` 每次调用都 `LoadSynchronous`，蓝图很可能在 Tick 中调用。建议缓存。
- `:180`：`CheckIsNeedShowLoadingScreen` 名为检查，但会修改 `CurrentLoadingPhase`。
- `:202` 与 `:262`：显示判断用 `GetFirstPlayerController()`，隐藏判断用“所有本地玩家都有 PC”，两套口径不一致。
- `:208`：“TextureStreaming”阶段实际只是固定时长等待，没有检测纹理流送。可考虑结合 `IStreamingManager::Get().GetNumWantingResources()`。
- 加载 Widget 类为空时，每帧都会尝试创建并触发 `ensure`。
- `GetTickableGameObjectWorld` 缺少 `override`，空析构函数可以删除。

### 20. UI Subsystem 的布局引用与重复推送

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Public/Subsystems/FrontendUISubsystem.h:44`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Subsystems/FrontendUISubsystem.cpp:39`

- `CreatedPrimaryLayout` 是强引用 UPROPERTY。关卡切换后旧 Layout 会被一直持有，新 Layout 注册时直接覆盖，旧 Widget 和其中的栈都无法释放。建议使用 `TWeakObjectPtr`，并提供 Unregister。
- 子系统属于 GameInstance 且只保存一个 Layout，无法支持分屏 / 多本地玩家。如果不打算支持，建议在注释中写明。
- 推送是异步的，没有去重。快速连续点击“设置”会推入两个相同界面。建议按 `WidgetStackTag + Class` 记录进行中的请求。
- `CastChecked<UW_ConfirmScreen>` 在配置错误时会直接崩溃，建议改为 `Cast` + `ensureMsgf`。

### 21. 数据驱动不彻底：字符串特判与硬编码 ID

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuEntryValueConfig.cpp:100`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuConfigData.cpp:11`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:432`

问题：

- 是否创建 `StringEnum` / `StringInteger` 取决于 `BindingKey` 是否等于 `"FullscreenMode"`，或是否包含 `"Quality"` / `"Scalability"`。
- `FrameRateLimit` 依赖名称开启数值匹配。
- 依赖规则硬编码了 `"Tab-Video"`、`"WindowMode"`、`"Resolution"`、`"Entry-VSync"`、`"OverallQuality"`，并且 ID 命名风格也不统一。

这意味着策划改一个 ID 或新增一个画质项，就可能导致行为静默改变。

建议：

- 在 `UMenuStringValueConfig` 中增加显式字段，例如 `bNumericMatching`、`bShowCustomWhenUnmatched`、`EnumType`。
- 将窗口模式相关规则改为基于 `BindingKey`（而不是 `DataId`）判断，或像 Multiplayer 一样复用 `FMenuEntryCondition` 实现配置化条件。
- `OverallQuality` 的双向依赖可以配置为“HubDataId”，避免写死。

### 22. 重复代码较多

- `W_OptionScreen` 与 `W_MultiplayerScreen`：Tab 注册、悬停 / 选中更新详情、`NativeGetDesiredFocusTarget`、`TryGetEntryWidgetClassName`、Back 绑定几乎一致。建议抽取 `UW_TabbedListScreenBase`。
- `UOptionsDataRegistry` 与 `UMultiplayerDataRegistry`：`CreateCollection`、递归展开子项完全重复；`MenuConfigData.cpp:51` 中的 `CreateCollectionFromConfig` 是第三份实现，且没有任何调用。建议抽取公共基类或工具函数，并删除未使用的版本。
- `W_OptionScreen.cpp:144`、`:263`、`:293` 有三段几乎相同的“刷新 Reset 按钮绑定”逻辑，可合并为 `RefreshResetActionBindings()`。
- `W_ListEntry_KeyBinding.cpp:211` 与 `:250` 的键盘 / 手柄点击处理只差一个枚举值。
- `OptionsDataRegistry` 同时维护 `SettingCommitters` 数组和 `SettingCommittersByKey` 映射，保留映射即可。

### 23. 运行时用 `NewObject` 创建 UMG 控件

位置：

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Credits/W_CreditsScreen.cpp:54`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Credits/W_CreditsScreen.cpp:77`
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/ListEntries/W_ListEntry_HeaderRow.cpp:28`

UMG 子控件应通过 `WidgetTree->ConstructWidget<UCommonTextBlock>()` 创建，才能正确设置 Outer 和 WidgetTree 关系，避免 GC 与命名问题。

### 24. ConfirmScreen 焦点逻辑

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/W_ConfirmScreen.cpp:103`

`NativeGetDesiredFocusTarget()` 是 const 查询函数，却在内部调用 `SetFocus()`，最后返回的仍是 `Super`（通常为空）。CommonUI 恢复焦点时拿不到正确目标。

建议直接返回 `DynamicEntryBox_Buttons->GetAllEntries().Last()`。

### 25. Credits 滚动实现

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Credits/W_CreditsScreen.cpp`

- `:127`：使用 0.016 秒循环定时器模拟 Tick，依赖 World 时间，世界暂停时停止。建议使用 `NativeTick` 或 Widget 动画。
- `:120`：用若干空格文本行填充视口高度，精度取决于 `LineHeight` 配置。建议使用一个高度精确的 Spacer。
- `:75`：`"{0} : {1}"` 通过 `FText::FromString` 创建，无法本地化。
- 如果 `StartDelay` 为 0，布局可能尚未完成，`GetScrollOffsetOfEnd()` 为 0，滚动会立即结束。

### 26. Scalar 数值格式固定为无小数

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/MenuConfig/MenuEntryValueConfig.cpp:71`

所有 Scalar 一律使用 `NoDecimal()`。例如准星大小（0.5～2.0，`Number` 类型）会显示为 `1` / `2`，无法区分 0.5 与 1。

建议在 `UMenuScalarValueConfig` 中增加 `MaxFractionalDigits` 字段。

### 27. StringTable 在模块启动时同步加载

位置：`Plugins/GameMainMenu/Source/GameMainMenu/Private/GameMainMenu.cpp:13`

- 模块 `LoadingPhase = Default`，此时同步加载资产偏早，可能产生启动警告。
- StringTable 仅通过 `LOCTABLE` 路径字符串引用，不构成硬引用；当前 `DirectoriesToAlwaysCook` 只包含 `/NNEDenoiser`。如果没有其他资产引用这些 StringTable，打包后可能缺失（需打包验证）。

建议：将 `/GameMainMenu/BP/StringTable` 加入 `DirectoriesToAlwaysCook`，或改用 `LOCTABLE_FROMFILE_GAME` 从 CSV 注册；加载时机改为 `PostEngineInit`。

### 28. 日志与调试辅助

- 全项目使用 `LogTemp`。建议声明 `DECLARE_LOG_CATEGORY_EXTERN(LogGameMainMenu, Log, All)`，便于过滤与按级别关闭。
- `Public/FrontendDebugHelper.h` 被 `OptionsDataRegistry.cpp`、`W_OptionScreen.cpp`、`W_MultiplayerScreen.cpp` 等多个运行时文件包含，但均未使用。它会在屏幕打印随机颜色的信息，而且 Shipping 中也会编译。建议移到 Private，并用 `#if !UE_BUILD_SHIPPING` 包裹，删除无用 include。

### 其他较小的改进点

- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_StringCulture.cpp:27`：语言名称使用 `GetDisplayName()`，它会按当前语言显示。建议使用 `GetNativeName()`，让用户在看不懂当前语言时也能找到母语。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/DataObjects/ListDataObject_String.cpp:96`：Rotator 通过显示文本反查选项，存在同名文本时会选错。建议直接使用索引。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Components/FrontendCommonRotator.cpp:23`：直接使用 `MyText`，未判空。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataInteractionHelper.cpp:25`：`bool` 解析总是返回成功，非法字符串会被当成 `false`。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:146`：提交改键时对所有映射调用 `MapPlayerKey`，忽略 `FailureReason`，并且每次 Back 都 `SaveSettings`。建议只提交修改过的映射，失败时记录日志。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/W_OptionScreen.cpp:164`：Back 时无论是否修改都 `ApplySettings(true)`，可能导致窗口模式刷新闪烁；修改分辨率后没有“保留 / 还原”倒计时确认，用户无法撤销不支持的分辨率。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Options/OptionsDataRegistry.cpp:263`：Control Tab 的分类顺序依赖 `TMap` 插入顺序，且以本地化文本作为分类 ID。建议按稳定 Key 排序。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:130`：`FindEntry` 线性查找，并在编辑条件 lambda 中反复调用，建议使用 `TMap<TPair<FName, FName>, ...>`。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/ListEntries/W_ListEntry_Editable.cpp:74`：通过预览 Enter 键 + 下一帧 ticker 做提交校验，失焦提交不会触发。建议改用 `OnTextCommitted`。
- `Plugins/GameMainMenu/Source/GameMainMenu/Private/FrontendGameInstance.cpp:14`：`GetOrCreate` / `Load` / `Get` / `Save` / `TrySave` 五个接口语义重叠，可精简为 `Get`、`GetOrLoad`、`Save(bool& bSuccess)`；同步 `SaveGameToSlot` 可改为 `AsyncSaveGameToSlot`。
- 配置中支持描述图片（`SoftDescriptionImage`、`OptionsScreenSoftImageMap`、`Frontend.Image.Test`），但 `UMenuEntryConfig` 没有图片字段，详情面板的图片功能实际不可用。建议补字段或删除死代码。
- `UFrontendGameUserSettings` 中 Difficulty / SprintMode / CrouchMode / AutoPickup 使用 `FString` 存储枚举值，且没有在 `ValidateSettings` 中校验。建议改为 `UENUM` 或 `FName` + 白名单，同时考虑增加设置版本号，方便以后迁移。
- `UFrontendGameUserSettings` 部分 UFUNCTION 带 `BlueprintPure` / `Category`，部分没有，暴露给蓝图的规则不一致。

---

## P3：命名、风格与文档

| 位置 | 问题 | 建议 |
| --- | --- | --- |
| `UUListDataObject_KeyBinding` / `UListDataObject_KeyBinding.h` | 类名多了一个 `U`，文件名与类名不对应 | 重命名为 `UListDataObject_KeyBinding`，并添加 CoreRedirect |
| `W_ActivatableBase.h:29` | `CashedOwningPlayerController` | 改为 `Cached...` |
| `AA_PushSoftWidget.h:26` | DisplayName 为 `Push Soft Wiget` | 改为 `Widget` |
| `W_KeyRemapScreen.h:31` 等 | `Cancled` | 改为 `Canceled` |
| `W_CreditsScreen.cpp:93` | `LogoSlog` | 改为 `LogoSlot` |
| `FrontendEnumTypes.h` | `UnKnown` | 改为 `Unknown`（需注意蓝图序列化兼容） |
| `GI_FrontentGameInstance` | 资产名拼写错误 | 改名时使用资产重定向 |
| `GameMainMenu.cpp` / `Build.cs` / `W_PrimaryLayout.cpp` | 版权头混用 Epic Games 模板与 `Reserverd` 拼写 | 统一版权头 |
| `GameMainMenu.cpp:11`、`W_ListEntry_Editable.cpp:38` | `ponytail:` 前缀的注释 | 删除前缀或改为说明性注释 |
| `LIST_DATA_ACCESSOR` | 生成 `GetbIsPassword` / `SetbIsPassword` | 布尔字段手写访问器，或宏中处理前缀 |
| 多数 Widget / Registry | 成员使用裸指针 `UPROPERTY() UType*` | UE5 建议使用 `TObjectPtr<UType>` |
| 全局 | 单行 `if` 无大括号与带大括号写法混用 | 按 UE 编码规范统一 |
| `UListDataObject_KeyBinding.cpp:86`、`:105` | 未使用的 `bFound` | 删除或用于回退图标 |
| `UListDataObject_KeyBinding.h:85` | `bHasDataBeenInitialized` 只写不读 | 删除 |
| `UListDataObject_KeyBinding.cpp:116` | 指针相减求索引 | 改为索引循环 |
| `LoadingScreenPhaseMapping` | 放在 `Widgets/Options/DataObjects` 下，但与 Options 无关 | 移到 `LoadingScreen/` 或 `Settings/` 目录 |
| `Widgets/Options/DataObjects` | 被 Multiplayer 和 MenuConfig 共同依赖 | 公共数据对象移到 `Widgets/ListData/` 一类的中性目录 |
| `W_ListEntry_KeyBinding.cpp:303` | 冲突提示使用 `LOCTEXT`，其他地方使用 StringTable | 统一本地化方式 |
| `W_OptionScreen.cpp:115` | `FText::FromString(TEXT(""))` 作为标题 | 使用 `FText::GetEmpty()` 或提供标题文案 |

---

## 测试建议

现有测试主要覆盖 Options 绑定、延迟 / 立即生效、分辨率修复、多人表单持久化和配置迁移，质量不错。建议补充以下用例，并优先为本报告中的 P0 / P1 问题编写回归测试：

1. 未绑定手柄映射的槽位类型识别（第 6 条）。
2. 冲突重新绑定后，冲突动作不再持有相同按键（第 3 条）。
3. 多个键盘槽位时，修改第二槽位不会影响第一槽位（第 7 条）。
4. `HoldLoadingScreenTime = 0` 时加载结束后 `bDisableWorldRendering == false`（第 4 条）。
5. DataAsset 中存在空 Tab / Category / Entry 时，Options 初始化不崩溃（第 10 条）。
6. 切换窗口模式再切回后，分辨率与 VSync 保持用户原值（第 12 条）。
7. 密码字段不会写入持久化存档（第 14 条）。

## 建议修复顺序

1. **第一批（稳定性）**：第 1、2、4、5、10 条。改动小，主要是生命周期与空值处理。
2. **第二批（改键正确性）**：第 3、6、7 条，并统一改键入口。
3. **第三批（设置生效）**：第 8、9、12、13、14 条，需要结合蓝图和实机验证。
4. **第四批（工程化）**：第 11、15、21、27、28 条，补齐编辑器校验、插件依赖和日志分类。
5. **第五批（重构）**：第 16、17、22 条及 P3 项，在有测试保护后逐步进行。
