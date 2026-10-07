# GameMainMenu 插件全项目代码审查报告

- **日期**: 2026-08-24
- **范围**: `Plugins/GameMainMenu` 全部 C++ 源码、配置、uplugin、Build.cs(约 110 个文件)+ 项目级 Config
- **基线**: master @ `3ab183b`(#ref32 优化游戏实例中 joinsession 函数)
- **方法**: 按模块分 4 个方向并行静态审查:核心基础设施 / 通用 UI 组件与基础控件 / Options 系统 / 多人联机系统
- **统计**: 高 12 条 / 中 30 条 / 低 31 条(跨模块去重后)

> 说明:多人联机的 Find/Join 实际逻辑在蓝图侧(`BlueprintImplementableEvent`),C++ 无法审查,相关竞态与防重入风险见 §5。

---

## 一、高严重度(优先修复)

### 1.1 核心基础设施

**H1. 异步加载回调捕获 NewObject 裸指针,存在 GC 悬垂**
- 位置: `Private/Subsystems/FrontendUISubsystem.cpp:79,105`;关联 `Private/Widgets/W_ConfirmScreen.cpp:13,29,50`
- 类别: 生命周期
- 描述: `PushConfirmWidgetToModalStackAsync` 中 `NewObject<UConfirmScreenInfoObject>()`(无 Outer、无 UPROPERTY)被异步 lambda 按值裸指针捕获,`RequestAsyncLoad` 期间(跨帧)GC 一旦运行对象即被回收,回调解引用悬垂指针。
- 建议: 存为子系统 `UPROPERTY(Transient)` 成员(回调后清除),或回调内 `TStrongObjectPtr`/`IsValid()` 校验;异步链路统一走 WeakThis 模式。

**H2. 音量设置从不落盘,重启丢失**
- 位置: `Private/FrontendSettings/FrontendGameUserSettings.cpp:61-161`
- 类别: Bug
- 描述: 6 个音量 Setter(Overall/Music/SFX/Voice/Ambient/UI)只改内存 + 设 SoundMix 覆盖,从不调用 `SaveSettings()`,而同文件其他 Setter 均调用——用户调的音量重启后全部丢失。
- 建议: 统一补 `SaveSettings()`(同时按 M3 收敛重复代码时一并修复)。

### 1.2 通用 UI 组件与基础控件

**H3. FrontendCommonListView 条目生成处 CastChecked + 映射表无空判,打包后配置错误即崩溃**
- 位置: `Private/Components/FrontendCommonListView.cpp:20`
- 类别: Bug
- 描述: `CastChecked<UListDataObject_Base>(Item)` 假定列表所有 Item 类型正确;`DataListEntryMapping` 仅在编辑器 `ValidateCompiledDefaults` 校验,运行时可能为 nullptr,直接解引用崩溃。
- 建议: 改 `Cast` 失败回退 `Super::OnGenerateEntryWidgetInternal`;对映射表加运行时空判(ensure + 回退)。

**H4. 异步动作节点在失败路径上永不销毁,永久泄漏**
- 位置: `Private/AsyncActions/AA_PushConfirmScreen.cpp:33-43`、`Private/AsyncActions/AA_PushSoftWidget.cpp:40-42`;根因在 `FrontendUISubsystem` 内部多条 `ensure` 失败路径直接 return 不触发回调
- 类别: 生命周期
- 描述: 子系统 `PushConfirmWidgetToModalStackAsync`/`PushSoftWidgetToStackAsync` 中 ensure 失败时直接 return,回调不触发 → 节点 `SetReadyToDestroy()` 永不被调用 → 已注册到 GameInstance 的异步节点(含蓝图绑定)永久挂载;且节点内 `UFrontendUISubsystem::Get()` 未判空(Get 内部为 Assert 模式,world 已销毁时直接断言)。
- 建议: 失败路径必须触发回调(带失败状态);节点 Activate 里判空后立即 `SetReadyToDestroy`。

**H5. KeyRemap 全局 Tick 延迟器裸捕获 this,窗口期悬垂**
- 位置: `Private/Widgets/W_KeyRemapScreen.cpp:173-185`
- 类别: 生命周期
- 描述: `FTSTicker::GetCoreTicker().AddTicker` 的 lambda 按值捕获 `this`;按键事件到下一帧 tick 之间若控件被销毁(模态栈清空),ticker 调用悬垂 this 崩溃;FTSTicker 不随对象销毁自动清除。
- 建议: lambda 内 `IsValid(this)` 校验;或改用挂到所属 World 的 `FTimerManager`。

### 1.3 Options 系统

**H6. StringResolution 覆写未调 Super,初始值永不初始化**
- 位置: `Private/Widgets/Options/DataObjects/ListDataObject_StringResolution.cpp:30-36`
- 类别: Bug
- 描述: `OnDataObjectInitialized()` 覆写未调用 `Super::OnDataObjectInitialized()`,`CurrentStringValue` 保持空字符串。
- 建议: 补调 `Super::OnDataObjectInitialized()`。

**H7. StringResolution 的 Getter 返回字面量而非成员,强制分辨率逻辑静默失效**
- 位置: `Private/Widgets/Options/DataObjects/ListDataObject_StringResolution.cpp:25` + 头文件 `:21`
- 类别: Bug
- 描述: `GetMaxAllowedResolution()` 返回硬编码 `"MaxAllowedResolution"`,成员 `MaxAllowedResolution`(已在 Init 赋值)从未被读取;强制值经注册表 `sscanf_s` 解析必然失败,"无边框模式强制最大分辨率"静默失效。另 `SupportedResolutions.Last()`(:25)在空数组时越界。
- 建议: Getter 返回成员;`Last()` 前判空。

**H8. OptionsDataRegistry 的 EditCondition lambda 捕获裸 UObject 指针**
- 位置: `Private/Widgets/Options/OptionsDataRegistry.cpp:533-541、646-654、882-905`
- 类别: 生命周期
- 描述: EditCondition 的 TFunction 按值捕获 `CreatedWindowMode`/`CreatedOverallQuality` 裸指针,条目被移出树或注册表先于条目销毁时悬垂解引用。
- 建议: 捕获 `TWeakObjectPtr` 并在调用处 `IsValid()` 校验。

**H9. 滑块拖拽每帧全量 ApplySettings**
- 位置: `Private/Widgets/Options/ListEntries/W_ListEntry_Scalar.cpp:46-52` + `Private/Widgets/Options/DataObjects/ListDataObject_Base.cpp:69`
- 类别: 性能
- 描述: `bShouldApplyChangeImmediately=true` 下拖拽滑块每帧 `OnValueChanged → ApplySettings(true)`,一次拖拽触发几十次全量设置应用(含音频设备重建等)。
- 建议: 拖拽期间只标 dirty,`OnMouseCaptureEnd`/`OnControllerCaptureEnd` 时统一应用。

### 1.4 多人联机系统

**H10. 保存会话数据时跳过空字符串,清空操作无法持久化**
- 位置: `Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp:254-258`(SaveSessionData)
- 类别: Bug
- 描述: `if (!Value.IsEmpty())` 导致用户清空房间名/密码后旧值仍留在存档,下次进入菜单又被 LoadSessionData 恢复。
- 建议: 删除空值跳过逻辑,始终覆盖写入。

**H11. 用魔法字符串 "ServerList" 永久删除占位 collection**
- 位置: `Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:306-339`(RebuildSessionList)
- 类别: 可维护性
- 描述: 按 `FName("ServerList")` 匹配占位项并删除且从不重建;配置驱动的占位名不同则旧占位残留列表头部;搜索 0 条时占位被删、列表只剩表头。
- 建议: 按类型删 `UListDataObject_Session`(占位由注册表/配置负责),或从配置读取占位 ID 常量。

**H12. "CreateGame"/"JoinGame" tab id 在 4 处硬编码,配置驱动路径下全部静默失效**
- 位置: `Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp:89,137-138,324`、`Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:14-15`
- 类别: 可维护性
- 描述: 默认选中、Start/Join/Refresh 动作绑定判断均依赖散落的 tab id 字面量;DataAsset 若改用其他 TabId,全部失效且无报错。
- 建议: 在注册表暴露常量(如 `GetCreateGameTabId()`)或统一从配置读取。

---

## 二、中严重度

### 2.1 核心基础设施

- **M1** `Config/DefaultGameMainMenu.ini:2` 与 `Plugins/GameMainMenu/Config/DefaultGameMainMenu.ini:5` — 可维护性: 两份配置几乎重复且已漂移(插件侧缺 `Frontend.Widget.Multiplayer` 条目),插件脱离项目分发则 widget 解析失败。建议: 只保留项目级配置。
- **M2** `Private/GameMainMenu.cpp:13-23` — UE实践: 硬编码 5 个 StringTable 路径并同步 `LoadObject`,与 `LocalizationKeys.h:10-14` 重复定义;资源重命名后运行时静默失败。建议: 路径收敛到单一宏,LoadObject 前 FindObject 并容忍失败(打日志)。
- **M3** `Private/FrontendSettings/FrontendGameUserSettings.cpp:14-27` — Bug: `GetGameWorld()` 遍历 WorldContexts 取第一个 Game/PIE world,PIE 多 world 时命中错误 world,音量覆盖作用到错误音频环境。建议: 由调用方传 WorldContextObject,或取 `UGameInstance::GetWorld()`。
- **M4** `Private/Subsystems/FrontendLoadingScreenSubsystem.cpp:88-99` — 生命周期: `bIsCurrentlyLoadingMap` 只在 `PostLoadMapWithWorld` 复位,地图加载失败/取消则加载屏(含 `bDisableWorldRendering`)永久卡死。建议: 挂 `LoadMapFailed` 委托 + 最长显示时间兜底。
- **M5** `Private/Subsystems/FrontendLoadingScreenSubsystem.cpp:223-231`、`Private/FrontendSettings/FrontendLoadingScreenSettings.cpp:9-14` — 性能: 加载屏 widget 类与 PhaseMapping 在 Tick 路径 `LoadSynchronous()`,首次命中阻塞游戏线程整帧;`GetLoadingPhaseDisplayInfo` 每次重复解析软引用。建议: Initialize 时异步预加载并缓存。
- **M6** `Private/Subsystems/FrontendUISubsystem.cpp:18` — UE实践: `Get()` 用 `EGetWorldErrorMode::Assert`,编辑器工具/UI 构造早期/world 销毁后调用即断言,与"返回 nullptr 容错"设计意图矛盾。建议: 改 `Log`/`ReturnNull`。
- **M7** `Private/FrontendSettings/FrontendGameUserSettings.cpp:61-161` — 可维护性: 6 个音量函数逐字重复(Clamp + GetWorld + SoundMix 五连),H2 的 SaveSettings 缺失正是此重复的后果。建议: 收敛为一个 `ApplyVolume` 辅助函数 + (tag→属性) 小表驱动。
- **M8** `Private/FrontendSettings/FrontendGameUserSettings.cpp:163-170` — UE实践: `SetCurrentCulture` 不校验合法性,`FrontendDeveloperSettings::SupportedCultures` 声明未使用;非法文化码写入配置导致资源解析失败。建议: 校验支持列表后再设置。
- **M9** `GameMainMenu.Build.cs:25-42` — UE实践: PublicDependency 12 个模块过多(EnhancedInput/CommonInput/InputCore/PreLoadScreen 等仅 .cpp 使用),向所有下游泄漏头文件与链接依赖。建议: 仅 Public 头实际用到的模块留 Public,其余移 Private。

### 2.2 通用 UI 组件与基础控件

- **M10** `Private/Widgets/W_ConfirmScreen.cpp:103-108` — UE实践: `NativeGetDesiredFocusTarget` 内做副作用 `SetFocus` 且返回 Super(空),与 CommonUI 后续对根控件的 SetUserFocus 竞争,按钮焦点可能被覆盖。建议: 返回按钮控件本身,删手动 SetFocus。
- **M11** `Private/Components/FrontendCommonListView.cpp:32` — Bug: `OnIsSelectableOrNavigableInternal` 对 `FirstSelectedItem` 无空判,列表为空时崩溃。建议: 加 `!FirstSelectedItem ||` 守卫。
- **M12** `Private/Widgets/MenuConfig/MenuConfigData.cpp:73-172` — 可维护性: FullscreenMode/Quality 分支在 String 与 Integer 两个 case 整段重复(共 4 段)。建议: 提取 `CreateFullscreenModeEntry/CreateQualityEntry` 辅助函数。
- **M13** `Private/Widgets/MenuConfig/MenuConfigData.cpp:80-82,93-97,130-132,144-148` — 本地化: "Fullscreen"、"Low/Medium/High/Epic/Cinematic" 硬编码英文,而 LocalizationKeys.h 已有 `TXT_VAL_WINDOW_FULLSCREEN()` 等宏。建议: 改用现成宏。
- **M14** `Private/Widgets/Credits/W_CreditsScreen.cpp:54,77` — UE实践: `NewObject<UCommonTextBlock>` 绕过 UUserWidget 生命周期(无 Initialize、GetOwningPlayer 为空)。建议: 改 `CreateWidget<UCommonTextBlock>(this)`。
- **M15** `Private/Widgets/Credits/W_CreditsScreen.cpp:75` — 本地化: `"{0} : {1}"` 格式串与分隔符硬编码。建议: 放入 StringTable。
- **M16** `Private/Components/FrontendCommonRotator.cpp:23` — Bug: 文本兜底分支只改显示不改选中项,用户下次左右切换跳回旧索引,显示值与实际值分叉。建议: 无匹配时明确"仅显示不选中"语义或保持索引一致。
- **M17** `Private/Widgets/W_ActivatableBase.cpp:10` — 生命周期: `CashedOwningPlayerController` 缓存失效后(无缝旅行/断线重连)永不复查,GetPC 永远返回 nullptr。建议: `IsValid()` 失败即重新查询,或去掉缓存。

### 2.3 Options 系统

- **M18** `Private/Widgets/Options/OptionsDataRegistry.cpp:868-925` — 可维护性: DataAsset 驱动路径仍硬编码 "Tab-Video"/"WindowMode"/"Entry-VSync" 等字符串接线,视频编辑条件逻辑双轨重复实现,配错 ID 静默失效。建议: 条件下沉为配置字段或按 BindingKey 注册统一条件表。
- **M19** `OptionsDataRegistry.cpp:103-105` + `ListDataObject_StringResolution.cpp:38-48` — 可维护性: 分辨率格式 `"(X=%d,Y=%d)"` 在 getter/setter/序列化三处手工复制。建议: 以 `ResolutionToValueString` 为唯一序列化入口,setter 走统一解析函数。
- **M20** `Private/Widgets/Options/ListEntries/W_ListEntry_KeyBinding.cpp:242-243,278-279` — 生命周期: 条目被 ListView 池化释放时未解绑 `OnKeyRemapScreenKeyPressed/Cancel` 委托,`NativeOnEntryReleased` 未 `CancelListening()`,释放条目仍可能消费键盘输入。建议: 释放时统一解绑 + CancelListening。
- **M21** `OptionsDataRegistry.cpp:720-726` — 本地化: 用本地化文本 `CategoryText.ToString()` 作 Map 键与集合 DataId,语言切换后 ID 不稳定。建议: 用稳定键做 ID,显示文本只作展示。
- **M22** `W_ListEntry_KeyBinding.cpp:211-284` — 可维护性: OnKeyboardSlotClicked/OnGamepadSlotClicked 约 90% 重复,且循环算出的 `FoundSlotIndex` 从未使用。建议: 合并为一个 `OpenRemapScreenForSlot(bool bGamepad)`。
- **M23** `OptionsDataRegistry.cpp:34-145` — 可维护性: `InitBindingMap` 约 110 行手工 lambda,键名字符串散落,`ResolveBinding` 失败静默返回 nullptr,拼错键静默退化为只读条目。建议: 宏/静态表收敛,失败时 `ensureMsgf` 报错。
- **M24** `W_ListEntry_Base.cpp:40-45,63-80` — 生命周期: `NativeOnEntryReleased` 未解绑数据对象上的 `OnListDataModified/OnDependencyDataModified`,池化条目在数据对象委托表累积失效弱引用并继续收到广播。建议: 换绑时先 RemoveAll,释放时清理。

### 2.4 多人联机系统

- **M25** `Public/Widgets/Multiplayer/DataObjects/ListDataObject_Editable.cpp:36`(对照 :44)— Bug: `SetText` 用 `Left(MaxCharCount)` 截断,`MaxCharCount==0`(初始化前)截成空串;而 `ValidateText` 将 0 视为"不限制",同一字段语义矛盾。建议: `MaxCharCount > 0` 才截断。
- **M26** `W_ListEntry_Session.cpp:17-28` — UE实践: 5 个 BindWidget TextBlock 无空检查(Editable 版对输入框有检查),且 `Hb_SessionColumns` 声明后从未使用;BP 漏绑即崩溃。建议: 统一空检查,删未用绑定。
- **M27** `W_ListEntry_Session.cpp:12`、`W_ListEntry_HeaderRow.cpp:15`、`W_ListEntry_Editable.cpp:16` — UE实践: 三个 entry 的 `OnOwningListDataObjectSet` 均 `CastChecked`,join 标签列表含多种类型,完全依赖 `UDA_DataListEntryMapping` 配置正确,配错即硬崩溃。建议: `Cast` + 失败日志跳过。
- **M28** `W_ListEntry_Session.cpp:19-28` — 本地化: `{0}/{1}`、"Yes"/"No"、`{0}ms` 均 `FText::FromString` 硬编码。建议: 走 `ST_MULTIPLAYER` StringTable(项目已有 `TXT_UI_YES/NO`)。
- **M29** `MultiplayerDataRegistry.cpp:209-251` — 本地化: 表头列名(ServerName/Map/Players/Private/Ping)及回退选项值("Map 01"、"Deathmatch"、"Asia" 等)硬编码。建议: 移入 StringTable。
- **M30** `W_MultiplayerScreen.cpp:317-331` + `MultiplayerDataRegistry.cpp:306-339` — Bug: 刷新后全部 session 对象重建,ListView 选中项必然丢失(Join 依赖选中项);且 `PopulateSessionList` 只在当前标签为 JoinGame 时刷新,搜索完成时用户已切标签则列表持陈旧条目。建议: 刷新前记录 SessionIndex,重建后恢复选中;列表刷新不依赖 CurrentTabId 分支。

---

## 三、低严重度(摘要)

**核心基础设施**
- L1 `FrontendGameInstance.cpp:24-29` — 加载槽位损坏返回 nullptr 覆盖缓存后,SaveMenuSessionData 静默空转。建议: 失败回退 `GetOrCreateMenuSessionData()`。
- L2 `FrontendLoadingScreenSubsystem.cpp:127-141` — 加载屏从未激活也会广播 deactivated,通知不对称。建议: 记录 `bHasDisplayedLoadingScreen`。
- L3 `FrontendDebugHelper.h:1-13` — 头文件不自包含;LogTemp+Warning 在 shipping 仍打印。建议: 补 include,独立 LogCategory + 编译宏裁剪。
- L4 `FrontendPlayerController.cpp:14-24` — PIE 反复进主菜单多次触发阻塞数秒的硬件 benchmark;-1 判定无法区分"从未跑"与"跑一半"。建议: 显式完成标记 + PIE 跳过。
- L5 `FrontendGameUserSettings.cpp:215-220` — `SetCurrentDisplayGamma` 直接改 `GEngine->DisplayGamma` 绕过 UGameUserSettings 通道。建议: 复用 `SetDisplayGamma`。
- L6 `Config/DefaultEngine.ini:35-36,120` — `DefaultGraphicsRHI` 重复赋值;`SteamDevAppId=480` 是 Steam 官方测试 AppID,发布前必须替换。建议: 删重复行,AppID 列入发布清单。
- L7 两个子系统 `ShouldCreateSubsystem` 逐字相同 — 复制粘贴,当前成本可接受,第三个子系统出现时再抽。

**通用 UI 组件**
- L8 `FrontendCommonTabListWidgetBase.cpp:12-17` — RegisterTab 返回值未检查,注释"同名 No-op"与实际不符。
- L9 `LoadingScreenPhaseMapping.cpp:18` — 兜底文本硬编码 StringTable 路径+键名,已有等价宏 `TXT_GAME_LOADING()` 未用。
- L10 `W_KeyRemapScreen.cpp:84-88` — 键映射查询失败静默吞掉输入,用户需再按一次。建议: 走 Cancel 路径。
- L11 `FrontendCommonButtonBase.cpp:28,35` — Get 为 Assert 模式,极端生命周期可断言;unhover 无条件广播空文本。
- L12 拼写:`W_KeyRemapScreen.h:31,33` FOnKeyRemapScreenKeyCancledDelegate(Cancled)、`AA_PushSoftWidget.h:26` DisplayName "Push Soft Wiget"、`W_ActivatableBase.h:29` CashedOwningPlayerController。
- L13 include 卫生:`FrontendCommonListView.cpp:6` 无条件 include Editor/WidgetCompilerLog.h(应加 WITH_EDITOR 守卫);`FrontendCommonButtonBase.cpp:7`、`W_PrimaryLayout.cpp:6` include 未使用的 FrontendDebugHelper.h。

**Options 系统**
- L14 `ListDataObject_Scalar.cpp:38-51` — 只判 getter 非空就调 setter,setter 未设置时空指针调用(当前恰好两处同时赋值未触发)。
- L15 `ListDataObject_String.cpp:142-153` — 值未变化也写设置并广播;`SetValueFromString` 绕过 `CanSetToForcedStringValue` 守卫,调用链不一致。
- L16 `OptionsDataRegistry.cpp:190` — `CreateScalarEntry` 无条件 NoDecimal,`WithDecimal()` 无调用者(死代码),DisplayGamma 显示为整数百分比。
- L17 `OptionsDataRegistry.cpp:377` — 语言条目硬编码默认 "en",与 SupportedCultures 无关联。
- L18 `UListDataObject_KeyBinding.cpp:137,149` — `&Slot - KeyBindingSlots.GetData()` 指针算术求索引,难读易错。建议: `IndexOfByReference(Slot)`。
- L19 `OptionsDataRegistry.cpp:268-297` — 每次切 tab 线性 FindByPredicate + 递归重建扁平数组。建议: Init 时按 TabId 预缓存。
- L20 `W_ListEntry_Base.cpp:30`、`W_OptionScreen.cpp:184,211` — `CastChecked` ListItemObject,调试期误配置即崩溃。建议: Cast + 空判。
- L21 `OptionsDataRegistry.cpp:251-266` — `InitOptionsDataRegistry` 非幂等,重复调用重建 BindingMap。建议: 加 `bHasInitialized` 守卫。
- L22 `W_ListEntry_String.cpp:62-74` — `OnRotatorValueChanged` 仅处理 Gamepad,鼠标滚轮/点击旋转时数据对象不同步。

**多人联机系统**
- L23 `W_ListEntry_Editable.cpp:39,78` — 每次过滤/回车都向 CoreTicker 挂一次性任务,连续输入反复创建/销毁 ticker,存在一帧内验证到未过滤文本的竞态。建议: 待处理标志合并同帧任务。
- L24 `ListDataObject_Editable.cpp:20-33,55-79` — SetText 过滤 switch 与 ValidateText 校验 switch 重复两份。建议: 提炼 `IsCharAllowed` 单一实现。
- L25 `MultiplayerDataRegistry.cpp:30-39` — 注释掉的测试 session 代码残留,删除。
- L26 `W_MultiplayerScreen.cpp:221` — `ensureMsgf` 检查 `NewObject` 返回值,NewObject 必然成功,死检查。
- L27 `MenuSessionData.h:20-78` — 每个方法成对 FName/FString 两个版本(8 个),API 面翻倍。建议: 只保留 FName 版。
- L28 `W_MultiplayerScreen.cpp:89` — NativeOnActivated 每次强制 `SelectTabByID("CreateGame")`,用户从 JoinGame 离开再返回被重置。建议: 用上次 CurrentTabId 恢复。
- L29 `ListDataObject_Session.h:6`、`W_ListEntry_Session.h:6`、`W_MultiplayerScreen.h:6` — 直接 include 重型头 `FindSessionsCallbackProxy.h`,纯声明需要的头可下沉 .cpp。
- L30 `W_MultiplayerScreen.cpp:72-92` — `PopulateSessionList` 无防重入(当前幂等安全),每次全量重建 N 个 UObject。建议: 搜索侧(BP)加防重复请求标志。
- L31 `W_MultiplayerScreen.cpp:233` — 调试字符串硬编码(可接受,保留)。

---

## 四、跨模块共性问题

1. **异步回调持有裸 UObject 指针(GC 悬垂)**——H1、H5、H8 同根因:异步 lambda/TFunction 捕获的裸指针 GC 不感知,加载/等待期间一次 GC 即悬垂。建议全局统一规范:跨帧回调只捕获 `TWeakObjectPtr`/WeakThis,或对象挂 UPROPERTY。
2. **失败路径不触发回调 → 永久挂起/泄漏**——H4、M4 同根因:ensure 失败直接 return,异步节点不销毁、加载屏不复位。建议统一"失败必回调(带状态)"模式。
3. **硬编码文本与现成本地化体系不一致**——M13、M15、M28、M29、L9、L17:`LocalizationKeys.h`/StringTable 体系已建立,但 MenuConfig、Multiplayer、Credits 多处仍 `FText::FromString` 硬编码。建议补一轮"硬编码文本清零"。
4. **魔法字符串/双轨接线**——H11、H12、M18、M23:"CreateGame"/"JoinGame"/"ServerList"/"Tab-Video"/BindingKey 散落多处,配置驱动与硬编码并存,"数据驱动"名不副实。建议: ID 常量收敛到注册表,硬编码路径逐步删除。
5. **运行时数据路径 CastChecked 滥用**——H3、M27、L20:ListView/entry 绑定处用 CastChecked,配置或数据一错即 Shipping 崩溃。建议: 运行时路径一律 Cast + 容错,仅编辑器校验用 Checked。
6. **平行类层次重复代码**——M7(音量 6 函数)、M12(4 段分支)、M22(双函数 90% 重复)、L24(双 switch):复制粘贴是缺陷扩散主因(H2 即 M7 的后果)。
7. **配置双份漂移**——M1:项目级与插件级 DefaultGameMainMenu.ini 已不一致。
8. **Build.cs 依赖过重**——M9:Public 依赖 12 个模块,下游编译与链接负担。
9. **BindWidget 空检查不统一**——M26:Lxx 各处有的查有的不查,BP 漏绑即崩溃。建议: 统一在 NativeConstruct 里集中校验。

---

## 五、总体评价与整改优先级

**总体**: 架构方向正确——UI/加载屏子系统职责清晰,CommonUI 激活/停用配对基本到位,Options 数据对象树与条目控件经多播委托解耦,ListView 池复用正确,多人联机复用了 Options 的注册表架构且无 Tick 滥用。整体属于"可上线的 80 分代码",但**异步链路安全**与**数据驱动双轨**是两大系统性隐患。

**特别提醒**: 多人联机 Find/Join/Refresh 实际逻辑在蓝图侧,C++ 无法审查;建议后续将会话生命周期(搜索防重入、join 超时、结果同步)下沉到 C++ 后再做一轮专项审查。

### 建议整改顺序

| 优先级 | 条目 | 理由 |
|---|---|---|
| P0(崩溃/数据丢失) | H1 H3 H4 H5 H6 H7 H8 H10 | GC 悬垂、节点泄漏、分辨率 bug、清空无法持久化 |
| P1(功能正确性) | H2 H9 H11 H12 M3 M4 M11 M16 M25 M30 | 音量不落盘、拖拽性能、选中态丢失、加载屏卡死 |
| P2(健壮性/一致性) | M2 M6 M8 M10 M17 M20 M24 M26 M27 | 失败兜底、委托清理、空检查统一 |
| P3(可维护性) | M1 M5 M7 M9 M12 M13 M14 M15 M18 M19 M21 M22 M23 M28 M29 + 低严重度全量 | 去重复、字符串收敛、本地化、include 卫生 |

*本次为只读审查,未修改任何代码。*
