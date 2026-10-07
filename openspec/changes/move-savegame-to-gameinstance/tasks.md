## 1. 新增 UFrontendGameInstance 类

- [x] 1.1 `GameMainMenu.Build.cs`:`PublicDependencyModuleNames` 增加 `"AdvancedSessions"`
- [x] 1.2 `GameMainMenu.uplugin`:`Plugins` 数组增加 `AdvancedSessions`(Enabled: true)
- [x] 1.3 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Public/FrontendGameInstance.h`:继承 `UAdvancedFriendsGameInstance`,`BlueprintType`/`Blueprintable`,声明 `GetMenuSessionData`(BlueprintPure)、`GetOrCreateMenuSessionData`、`LoadMenuSessionData`、`SaveMenuSessionData`(均 BlueprintCallable),私有 `UPROPERTY(Transient) UMenuSessionData* CachedMenuSessionData`
- [x] 1.4 新建 `Plugins/GameMainMenu/Source/GameMainMenu/Private/FrontendGameInstance.cpp`:实现四个函数,槽位使用 `TEXT("MenuSession")`、索引 0,加载失败不缓存
- [x] 1.5 编译 GameMainMenu 模块通过

## 2. 改造 UW_MultiplayerScreen 委托 GameInstance

- [x] 2.1 `W_MultiplayerScreen.cpp`:`SaveSessionData()` 中 `CachedSessionData` 改为 `GetGameInstance()->GetOrCreateMenuSessionData()`,`SaveGameToSlot` 调用改为 `SaveMenuSessionData()`
- [x] 2.2 `W_MultiplayerScreen.cpp`:`LoadSessionData()` 中 `LoadGameFromSlot` 改为 `LoadMenuSessionData()`,空值提前 return
- [x] 2.3 `W_MultiplayerScreen.cpp`:`GetCachedSessionData()` 改为返回 `GetMenuSessionData()`
- [x] 2.4 `W_MultiplayerScreen.h`:删除 `CachedSessionData` UPROPERTY;清理不再使用的 include(如 `Kismet/GameplayStatics.h`,若界面无其他调用)
- [x] 2.5 添加获取 GameInstance 的空值保护(Cast 失败直接 return,不 ensure)
- [x] 2.6 编译通过,三个函数签名与返回类型不变

## 3. 蓝图迁移与编辑器配置

- [ ] 3.1 编辑器打开 `GI_FrontentGameInstance`(父类已确认为 `AdvancedFriendsGameInstance`),Class Settings 重设父类为 `FrontendGameInstance`,编译并保存
- [ ] 3.2 确认 Project Settings → Maps & Modes → Game Instance Class 指向 `GI_FrontentGameInstance`
- [ ] 3.3 确认 AdvancedSessions 原有功能(会话创建/查找/加入)不受影响

## 4. PIE 验证

- [ ] 4.1 删除已有 `MenuSession` 存档,PIE 进入多人界面,修改若干字段并触发保存
- [ ] 4.2 退出 PIE 重新进入:字段值恢复,验证加载路径
- [ ] 4.3 用迁移前的旧存档(如有)验证兼容加载
- [ ] 4.4 确认 `W_MultiplayerScreen` 蓝图原有调用点无需改动即工作
