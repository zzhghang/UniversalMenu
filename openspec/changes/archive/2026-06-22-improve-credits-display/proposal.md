## Why

当前 Credits 界面的成员信息是分成两行（Name 和 Role 各占一行）展示的，使用 UTextBlock 动态生成，无法设置 TextStyle 来统一管理字体样式（大小、颜色、字距等）。同时文本没有居中，视觉效果不符合传统游戏 Credits 的呈现方式。此外，每行应该是 "Role : Name" 的单行格式而非两行分离。

## What Changes

- 每行条目格式从"Name / Role 分两行"改为单行 "Role : Name"
- `PopulateCreditsEntries` 使用 `UCommonTextBlock` 替代 `UTextBlock`，支持通过 `CommonTextStyle` DataAsset 统一设置字体样式
- 每条文本水平居中对齐
- 新增 `CreditsTextStyle` 属性，允许在编辑器中指定 CommonTextStyle DataAsset

## Capabilities

### New Capabilities

- `credits-text-styling`: Credits 界面的文本样式可通过 CommonUI 的 TextStyle DataAsset 配置，包括字体、大小、颜色等

### Modified Capabilities

<!-- 无 -->

## Impact

- `W_CreditsScreen.h` — 新增 `CreditsTextStyle` 属性 (TSoftObjectPtr<UCommonTextStyle>)
- `W_CreditsScreen.cpp` — PopulateCreditsEntries 中 UTextBlock → UCommonTextBlock，格式化文本为 "Role : Name"，设置居中对齐
- `CreditsDataTypes.h` — 无需改动，数据模型不变
- `GameMainMenu.Build.cs` — 可能需要添加 CommonUI 模块依赖（如果尚未依赖）
