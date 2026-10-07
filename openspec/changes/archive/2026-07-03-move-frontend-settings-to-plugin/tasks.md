## 1. 修改 UCLASS 宏

- [x] 1.1 修改 `FrontendDeveloperSettings.h`：将 `Config = Game` 改为 `Config = GameMainMenu`
- [x] 1.2 修改 `FrontendLoadingScreenSettings.h`：将 `Config = Game` 改为 `Config = GameMainMenu`

## 2. 迁移配置文件

- [x] 2.1 将 `DefaultGame.ini` 中 `[/Script/GameMainMenu.FrontendDeveloperSettings]` section 移至 `DefaultGameMainMenu.ini`
- [x] 2.2 将 `DefaultGame.ini` 中 `[/Script/GameMainMenu.FrontendLoadingScreenSettings]` section 移至 `DefaultGameMainMenu.ini`
- [x] 2.3 从 `DefaultGame.ini` 中删除已迁移的两个 section

## 3. 验证

- [ ] 3.1 编译项目确保无编译错误（需在 Visual Studio 或通过 UBT 命令行编译）
- [ ] 3.2 在编辑器中打开项目设置，验证 Frontend UI Settings 和 Loading Screen Settings 可正常加载和保存
