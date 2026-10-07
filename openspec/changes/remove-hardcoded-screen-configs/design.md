## Context

GameMainMenu 插件中,OptionsScreen 与 MultiplayerScreen 的注册表类各存在两条构建路径:

- **配置路径**:`InitFromDataAsset()`(Options)/`InitFromConfig()`(Multiplayer),通过共享工厂 `CreateEntryFromConfig`/`CreateCollectionFromConfig` 从 `UMenuConfigData` 资产构建。
- **硬编码回退**:`UOptionsDataRegistry` 的四个 `Init*CollectionTab()` 函数(约 450 行)与 `UMultiplayerDataRegistry::InitHardcodedFallback()`(约 120 行),在配置为空/未指定时构建固定菜单。

内容资产 `DA_OptionsConfig`(Gameplay/Audio/Video 三个 Tab)与 `DA_MultiplayerConfig`(CreateGame/JoinGame 两个 Tab)已存在,配置路径是当前主力。硬编码路径是重构遗留,与资产内容重复且绕过工厂,应整体删除。

另有三个残留硬编码点不随回退删除而消失:

1. `W_MultiplayerScreen` 用 `"CreateGame"`/`"JoinGame"` 字面量决定动作绑定与初始 Tab;
2. `MenuConfigData.cpp` 工厂对 FullscreenMode/画质条目硬编码选项列表(已带 `ponytail:` 注释);
3. `LocalizationKeys.h` 中 `TXT_MP_TAB_CREATE_GAME`/`TXT_MP_TAB_JOIN_GAME` 仅被硬编码回退使用。

## Goals / Non-Goals

**Goals:**

- 删除两个注册表的硬编码回退路径及仅其使用的死代码,DataAsset 成为唯一数据源。
- Multiplayer 屏幕的 Tab 语义(创建/加入)与初始 Tab 由配置声明,消除代码中的 Tab ID 字面量。
- 工厂的引擎默认选项列表允许被配置覆盖,同时保持现有资产行为不变。
- 清理过时备份文件与失效本地化键。

**Non-Goals:**

- 不改动配置路径的现有构建逻辑、工厂 API 与数据对象类。
- 不做引擎默认选项标签(如 "Fullscreen"/"Low" 英文文本)的本地化迁移。
- 不补齐 `DA_OptionsConfig` 缺失的 Control Tab 资产(如需,由设计侧在编辑器中补充,代码天然支持)。
- 不删除 `W_MultiplayerScreen` 中被注释掉的测试会话代码(非本次范围)。

## Decisions

### D1: 配置缺失时降级为空注册表(ensure + 日志),而非崩溃或保留回退

`InitRegistry()`/`InitOptionsDataRegistry()` 在配置为空时改为 `ensureMsgf` + `UE_LOG(Error)` + 提前返回,注册表为空、屏幕显示空菜单。

- 备选 A:`check` 强制崩溃——对菜单系统过于激进,一个未赋值的蓝图引用就让编辑器崩掉。
- 备选 B:保留回退——与"单一数据源"目标矛盾,是本次要删除的东西。
- 理由:`ensure` 在开发期显眼、发行版不崩,空菜单是明确的"配置忘了指定"信号。

### D2: Tab 语义用 `UMenuTabConfig` 新增 `TabRole` 字段表达

`UMenuTabConfig` 增加 `UPROPERTY` 枚举 `EMenuTabRole { None, CreateGame, JoinGame }`(默认 None)。`UMultiplayerDataRegistry` 提供 `GetTabIdForRole(EMenuTabRole)`(扫描注册 Tab 的配置角色);`W_MultiplayerScreen`:

- 初始 Tab:`SelectTabByID(注册表首个 Tab 的 DataId)`——配置顺序即默认 Tab;
- 动作绑定:`OnMultiplayerTabSelected` 中按当前 Tab 的 Role 决定 StartGame/JoinGame+Refresh 绑定。

- 备选 A:保留字面量但提取为共享常量——改动最小,但 Tab ID 仍是"代码语义",配置改名即断,未真正消灭硬编码。
- 备选 B:给配置加 `bIsDefaultTab` 布尔——只解决初始 Tab,不解决动作绑定的角色映射,信息量不如 Role。
- 理由:Role 枚举把"哪个 Tab 是创建/加入"这一屏幕行为完全交给配置,且 `None` 角色自然覆盖"配置只有自定义 Tab"的退化场景。

### D3: 工厂选项列表支持配置覆盖,默认沿用引擎值

`CreateEntryFromConfig()` 中 FullscreenMode 分支:若 `AvailableOptions` 非空则用之,否则用现有三种 `EWindowMode` 枚举选项;画质分支同理用 `AvailableIntegerOptions` 覆盖五档默认。

- 理由:选项列表机制(`AvailableOptions`/`AvailableIntegerOptions`)已存在于其他条目类型,这两个特殊分支只是绕过了它;补上判断即可让设计侧能定制(如游戏只允许 Windowed),现有资产未提供时行为不变。

### D4: 回退删除时连带删除其独占代码

- `CreateBoolEntry`/`CreateStringEntry`/`CreateScalarEntry`(仅被 Options 四个回退函数使用)一并删除。
- `MultiplayerDataRegistry.cpp` 顶部 `TabId_CreateGame`/`TabId_JoinGame` 静态量随回退删除;屏幕改用 D2 的 Role 机制后不再需要字面量。
- `TXT_MP_TAB_CREATE_GAME`/`TXT_MP_TAB_JOIN_GAME` 本地化键删除(删除前确认无其他引用)。
- `OptionsDataRegistry.cpp.bak` 删除。

## Risks / Trade-offs

- [配置资产在 WBP 中未指定] → 删除回退后对应屏幕变空。缓解:apply 阶段在编辑器中验证 `WBP_OptionScreen`/`WBP_MultiplayerScreen` 的 Config 属性已指向 `DA_OptionsConfig`/`DA_MultiplayerConfig`。
- [Options 失去 Control Tab] → 硬编码版有 Control,`DA_OptionsConfig` 资产没有;删除后该 Tab 消失。缓解:属预期行为,资产为唯一事实来源;如需保留,由设计侧在编辑器中补充 Control Tab 资产(代码无需改动)。
- [TabRole 需改资产] → `DA_CreateGame`/`DA_JoinGame` 需在编辑器中设置 Role,漏改则动作绑定全部失效(相当于两个 Tab 都是 None)。缓解:tasks 中列为显式编辑器步骤并给出验证场景。
- [编译无法在本仓库直接验证] → 需要 UE 5.7 编辑器编译。缓解:tasks 末尾单列编译+编辑器验证步骤,不把"代码写完"当作完成。

## Migration Plan

无运行时数据迁移。资产侧改动仅两处(DA_CreateGame/DA_JoinGame 设置 TabRole),在编辑器中完成,与代码删除同批提交。

## Open Questions

- 是否需要为 `DA_OptionsConfig` 补充 Control Tab 资产(内容等价于原硬编码版)?——由设计侧决定,不影响本次代码改动。
