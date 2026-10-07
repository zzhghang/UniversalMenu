## 1. 删除 Multiplayer 硬编码回退

- [ ] 1.1 删除 `MultiplayerDataRegistry.cpp` 中 `InitHardcodedFallback()` 函数体与顶部 `TabId_CreateGame`/`TabId_JoinGame` 静态量,并删除头文件中的函数声明
- [ ] 1.2 修改 `InitRegistry()`:删除 else 回退分支,配置为空时 `ensureMsgf` + `UE_LOG(Error)` 并提前返回(空注册表)

## 2. 删除 Options 硬编码回退

- [ ] 2.1 删除 `OptionsDataRegistry.cpp` 中 `InitGameplayCollectionTab`/`InitAudioCollectionTab`/`InitVideoCollectionTab`/`InitControlCollectionTab` 四个函数及头文件声明
- [ ] 2.2 修改 `InitOptionsDataRegistry()`:删除 else 回退分支,配置为空时 `ensureMsgf` + `UE_LOG(Error)` 并提前返回
- [ ] 2.3 删除仅被回退使用的 `CreateBoolEntry`/`CreateStringEntry`/`CreateScalarEntry` 辅助函数及头文件声明
- [ ] 2.4 删除过时备份文件 `Private/Widgets/Options/OptionsDataRegistry.cpp.bak`

## 3. 清理失效本地化键

- [ ] 3.1 确认 `TXT_MP_TAB_CREATE_GAME`/`TXT_MP_TAB_JOIN_GAME` 无其他引用后,从 `LocalizationKeys.h` 删除

## 4. Tab 角色配置驱动

- [ ] 4.1 `MenuTabConfig.h` 新增枚举 `EMenuTabRole { None, CreateGame, JoinGame }` 与 `UPROPERTY TabRole` 字段(默认 None)
- [ ] 4.2 `UMultiplayerDataRegistry` 新增 `GetTabIdForRole(EMenuTabRole)`(扫描已注册 Tab 的配置角色),`InitFromConfig()` 中登记 Tab 角色映射
- [ ] 4.3 `W_MultiplayerScreen::OnMultiplayerTabSelected`:按当前 Tab 的 Role 决定 StartGame/JoinGame+Refresh 动作绑定,替换 `CreateGameTabId`/`JoinGameTabId` 字面量比较
- [ ] 4.4 `W_MultiplayerScreen` 初始 Tab 选择:改为选中注册表首个 Tab 的 DataId,替换 `SelectTabByID(FName(TEXT("CreateGame")))`

## 5. 工厂选项列表支持配置覆盖

- [ ] 5.1 `MenuConfigData.cpp` `CreateEntryFromConfig()` 的 FullscreenMode 分支:String 与 Integer 两条路径在 `AvailableOptions` 非空时使用配置选项,否则沿用三种引擎默认窗口模式
- [ ] 5.2 画质分支:String 与 Integer 两条路径在 `AvailableIntegerOptions` 非空时使用配置选项,否则沿用五档引擎默认画质

## 6. 资产与编辑器验证

- [ ] 6.1 在编辑器中确认 `WBP_OptionScreen` 已指定 `DA_OptionsConfig`,`WBP_MultiplayerScreen` 已指定 `DA_MultiplayerConfig`
- [ ] 6.2 在编辑器中为 `DA_CreateGame` 设置 TabRole=CreateGame,`DA_JoinGame` 设置 TabRole=JoinGame
- [ ] 6.3 编译插件,确认无残留引用报错
- [ ] 6.4 PIE 验证:OptionsScreen 显示 Gameplay/Audio/Video 三个 Tab,MultiplayerScreen 默认选中 CreateGame Tab,动作绑定随 Tab 切换正确
- [ ] 6.5 PIE 验证:临时清空某屏幕的 Config 引用,确认空菜单 + 日志报错而非崩溃
