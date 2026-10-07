# 将 SaveGame 持久化职责迁移到 GameInstance

## Why

目前 `UMenuSessionData` 的创建、加载、保存与缓存全部写在 `UW_MultiplayerScreen` 中,会话数据生命周期与 UI 控件绑定:控件销毁后缓存即丢失,且除该界面外没有任何系统能访问会话数据。项目已有 `GI_FrontentGameInstance` 蓝图,把持久化职责迁移到 GameInstance 后,数据生命周期与游戏进程一致,任何代码/蓝图都能读取和保存,也符合 GameInstance 作为跨场景全局持有者的定位。

## What Changes

- 新增 C++ 类 `UFrontendGameInstance`(GameMainMenu 插件,继承 `UAdvancedFriendsGameInstance`,Blueprintable),负责 `UMenuSessionData` 的创建、从槽位加载、保存到槽位与进程内缓存,全部暴露为 BlueprintCallable。继承 AdvancedSessions 的 GameInstance 是为了与现有 `GI_FrontentGameInstance` 蓝图父类一致,保留其会话/好友相关功能。
- `UW_MultiplayerScreen::SaveSessionData()` / `LoadSessionData()` 保留(收集 UI 字段值、把存档值应用回 UI 是控件职责,且现有蓝图仍调用它们),但其中的 `CreateSaveGameObject` / `LoadGameFromSlot` / `SaveGameToSlot` 与缓存改为委托给 GameInstance。
- 移除 `UW_MultiplayerScreen` 的 `CachedSessionData` 成员,`GetCachedSessionData()` 改为转发 GameInstance 的缓存。
- **BREAKING**:`GI_FrontentGameInstance` 蓝图需要重新指定父类为 `UFrontendGameInstance`(编辑器内手动操作,无法由代码完成);不重设父类则新功能静默失效。因新类直接继承 `UAdvancedFriendsGameInstance`(已确认是当前蓝图的父类),重设父类不会丢失任何现有功能。

## Capabilities

### New Capabilities

- `gameinstance-session-persistence`:GameInstance 层持有并管理 `UMenuSessionData` 的创建、加载、保存与访问。

### Modified Capabilities

<!-- 现有 specs(credits-auto-return / credits-text-styling / frontend-settings-config)均不涉及多人界面存档,无需求变更 -->

## Impact

- 代码:`Plugins/GameMainMenu/Source/GameMainMenu/` 新增 `Public/FrontendGameInstance.h` + `Private/FrontendGameInstance.cpp`;修改 `Public/Widgets/Multiplayer/W_MultiplayerScreen.h`、`Private/Widgets/Multiplayer/W_MultiplayerScreen.cpp`;`GameMainMenu.Build.cs` 新增 `AdvancedSessions` 模块依赖;`GameMainMenu.uplugin` 新增 AdvancedSessions 插件依赖。
- 蓝图:`GI_FrontentGameInstance` 需手动重设父类;`W_MultiplayerScreen` 蓝图调用点无需改动(函数签名保留)。
- 存档兼容:槽位名 `MenuSession` 与 `UMenuSessionData` 结构不变,旧存档可直接读取。
