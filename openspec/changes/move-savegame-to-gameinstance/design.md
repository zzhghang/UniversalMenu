# 设计:SaveGame 持久化职责迁移到 GameInstance

## Context

现状(`GameMainMenu` 插件):

- `UW_MultiplayerScreen::SaveSessionData()`(`W_MultiplayerScreen.cpp:236-270`)与 `LoadSessionData()`(`:272-299`)内联完成 `CreateSaveGameObject` / `LoadGameFromSlot` / `SaveGameToSlot`,并把 `UMenuSessionData*` 缓存在 widget 的 `CachedSessionData` 成员上。
- `LoadSessionData()` 由 `NativeOnActivated()` 调用;`SaveSessionData()` 为 BlueprintCallable,由 `W_MultiplayerScreen` 蓝图子类调用(C++ 内无调用点,`.uasset` 无法用代码修改)。
- 项目已有蓝图 GameInstance:`Plugins/GameMainMenu/Content/BP/UMG/GI_FrontentGameInstance.uasset`,无 C++ GameInstance 类;已从其资产导入表确认父类为 `/Script/AdvancedSessions.AdvancedFriendsGameInstance`。
- 游戏模块 `UniversalMenu` 不依赖 `GameMainMenu` 模块(见 `UniversalMenu.Build.cs`),依赖是蓝图软引用。
- `UMenuSessionData` 是 `USaveGame` 子类(`TMap<FName,FString> SavedValues`),位于插件 `Public/Widgets/Multiplayer/`。

## Goals / Non-Goals

**Goals:**

- `UMenuSessionData` 的创建/加载/保存/缓存由 GameInstance 持有,任何系统可访问。
- 多人界面保留"收集 UI 值 → 存档"与"存档 → 应用回 UI"两个 UI 侧职责。
- 界面三个函数的蓝图签名不变,`W_MultiplayerScreen` 蓝图调用点零改动。
- 旧存档(槽位 `MenuSession`)直接兼容。

**Non-Goals:**

- 不把 `UMultiplayerDataRegistry` 或收集/应用逻辑移出 widget(依赖 widget 的 `MultiplayerConfig`,且属于 UI 职责)。
- 不移动 `UMenuSessionData` 文件位置。
- 不改槽位名、不改数据结构。
- 不提供"未重设父类时的运行时回退"(见 Risks)。

## Decisions

### 1. 新增 `UFrontendGameInstance` C++ 类,放在 GameMainMenu 插件顶层

继承 `UAdvancedFriendsGameInstance`(而非 `UGameInstance`),标记 `BlueprintType`/`Blueprintable`,文件为 `Public/FrontendGameInstance.h` + `Private/FrontendGameInstance.cpp`(与插件顶层 `FrontendGameplayTags.h` 同风格,新建 GameInstance 文件夹无必要)。

依赖变更:
- `GameMainMenu.Build.cs` 的 `PublicDependencyModuleNames` 增加 `"AdvancedSessions"`(头文件 include `AdvancedFriendsGameInstance.h`,必须 public 依赖)。
- `GameMainMenu.uplugin` 的 `Plugins` 数组增加 `AdvancedSessions`(Enabled: true),声明插件级依赖,保证加载顺序。

暴露的 BlueprintCallable 函数:

| 函数 | 行为 |
|---|---|
| `GetMenuSessionData()` (BlueprintPure) | 返回缓存,无则空 |
| `GetOrCreateMenuSessionData()` | 缓存空则 `CreateSaveGameObject` 创建,返回缓存 |
| `LoadMenuSessionData()` | `LoadGameFromSlot(TEXT("MenuSession"), 0)`,成功则更新缓存并返回,失败返回空 |
| `SaveMenuSessionData()` | `SaveGameToSlot(缓存, TEXT("MenuSession"), 0)` |

缓存成员:`UPROPERTY(Transient) UMenuSessionData* CachedMenuSessionData;`

**备选方案**:继承 `UGameInstance` —— 蓝图重设父类后会丢失 AdvancedSessions 的会话/好友功能(项目多人界面正在使用),不可接受。已否决。

**备选方案**:类放在 `UniversalMenu` 游戏模块 —— 需要给游戏模块加 `GameMainMenu` 模块依赖(或把 `UMenuSessionData` 移出插件),改动面更大;且用户的 `GI_FrontentGameInstance` 资产本身就在插件 Content 下。已否决。

### 2. 界面函数保留签名,内部委托 GameInstance

- `SaveSessionData()`:收集逻辑不动,把 `CachedSessionData` 替换为 `GameInstance->GetOrCreateMenuSessionData()`,末尾改为 `GameInstance->SaveMenuSessionData()`。
- `LoadSessionData()`:改为 `GameInstance->LoadMenuSessionData()`,空则 return,应用逻辑不动。
- `GetCachedSessionData()`:改为返回 `GameInstance->GetMenuSessionData()`。
- 删除 widget 的 `CachedSessionData` UPROPERTY 与相关 include(`Kismet/GameplayStatics.h` 若不再使用也移除)。
- 获取 GameInstance 的辅助:`Cast<UFrontendGameInstance>(UGameplayStatics::GetGameInstance(this))`,空则直接 return(见 Risks)。

**备选方案**:彻底删除界面三个函数,让蓝图改调 GameInstance —— `.uasset` 调用点无法用代码改写,会立即破坏蓝图直到用户手动修复;且收集/应用逻辑本就必须留在 widget。已否决。

### 3. 槽位名保留字面量 `TEXT("MenuSession")`

`UMenuSessionData` 结构不变,序列化兼容。不引入可配置槽位名(YAGNI,无人要求)。

## Risks / Trade-offs

- **[用户未把 `GI_FrontentGameInstance` 重设父类 → 持久化静默失效]** → 迁移任务清单把"编辑器重设父类 + 确认项目 GameInstanceClass 指向该蓝图"列为显式步骤,并提供 PIE 验证清单。
- **[AdvancedSessions 模块依赖引入编译耦合]** → GameMainMenu 与 AdvancedSessions 均为项目内插件且都已启用,依赖仅编译期;AdvancedSessions 是 marketplace 插件,若未来升级需同步重编 GameMainMenu。
- **[Cast 失败返回空导致函数无操作]** → 这是期望行为(未配置好时失败可见于 PIE 验证步骤),不做隐藏回退。

## Migration Plan

1. 新增 `UFrontendGameInstance` 头/实现,编译。
2. 改 `W_MultiplayerScreen` 委托逻辑,编译。
3. 编辑器:打开 `GI_FrontentGameInstance`,Class Settings 重设父类为 `FrontendGameInstance`,保存;确认 Project Settings → Maps & Modes → Game Instance Class 指向该蓝图。
4. PIE 验证:修改多人界面字段 → 触发保存 → 退出 PIE → 重新 PIE,确认字段值恢复。
5. 回滚:代码 revert 即可;蓝图父类改回原类无数据损失(槽位与格式未变)。

## Open Questions

- 无(已确认 `GI_FrontentGameInstance` 当前父类为 `UAdvancedFriendsGameInstance`,新类直接继承它即可保留全部现有功能)。
