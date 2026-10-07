## ADDED Requirements

### Requirement: 每行显示 "Role : Name" 格式
系统 SHALL 将每个 FCreditsEntry 渲染为单行文本，格式为 "{Role} : {Name}"（例如 "Executive Producer : John Carpenter"）。

#### Scenario: 单行格式展示
- **WHEN** Credits 界面被激活
- **THEN** 每条成员信息以 "Role : Name" 的单行格式显示

### Requirement: 文本居中
Credits 的所有条目文本 SHALL 在水平方向上居中显示。

#### Scenario: 条目水平居中
- **WHEN** Credits 界面展示任意条目
- **THEN** 该条目文本相对于 ScrollBox 内容区域水平居中

### Requirement: CommonTextStyle 支持
Credits 界面 SHALL 支持通过 `TSoftObjectPtr<UCommonTextStyle>` 属性配置文本样式。当该属性有效时，所有动态生成的条目 SHALL 使用该 TextStyle；当该属性为空时，SHALL 使用 CommonUI 默认样式。

#### Scenario: 配置了 TextStyle
- **WHEN** 蓝图子类设置了 CreditsTextStyle 指向有效的 CommonTextStyle DataAsset
- **THEN** 所有 Credits 条目使用该 TextStyle 渲染（字体、大小、颜色等）

#### Scenario: 未配置 TextStyle
- **WHEN** CreditsTextStyle 为空
- **THEN** 所有 Credits 条目使用 CommonUI 默认文本样式，不会崩溃
