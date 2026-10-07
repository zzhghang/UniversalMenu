## ADDED Requirements

### Requirement: Session 列表项提供 5 个 BindWidget 文本控件
系统 SHALL 在 `W_ListEntry_Session` 中声明 5 个 `BindWidget` UCommonTextBlock 成员，分别对应 ServerName、MapName、Players、Private、Ping 列，供 Blueprint 预置布局。

#### Scenario: Blueprint 预置 5 个 TextBlock
- **WHEN** 在 Blueprint 中创建继承 `W_ListEntry_Session` 的 Widget，放置 5 个 UCommonTextBlock 并分别命名为 `Tb_ServerName`、`Tb_MapName`、`Tb_Players`、`Tb_Private`、`Tb_Ping`
- **THEN** Widget 编译通过，无需 C++ 侧动态创建控件

#### Scenario: 缺少必需 BindWidget 时报错
- **WHEN** Blueprint 中缺少任意一个必需 BindWidget（如未放置 `Tb_Ping`）
- **THEN** 引擎输出错误日志提示缺失的 BindWidget 名称

### Requirement: C++ 仅赋值文本不创建布局
系统 SHALL 在 `OnOwningListDataObjectSet` 中直接对已绑定的 5 个 TextBlock 调用 `SetText`，不执行 `ClearChildren`、`NewObject`、`AddChildToHorizontalBox` 等布局操作。

#### Scenario: 数据设置后文本正确显示
- **WHEN** ListView 将 `UListDataObject_Session` 设置到 `W_ListEntry_Session`
- **THEN** `Tb_ServerName` 显示服务器名，`Tb_Players` 显示 "3/16" 格式，`Tb_Private` 显示 "Yes"/"No"，`Tb_Ping` 显示 "42ms" 格式

### Requirement: 删除动态布局属性
系统 SHALL 从 `W_ListEntry_Session` 中移除 `SessionTextStyle`、`ColumnGap`、`EdgePadding`、`VerticalPadding` 四个 `EditDefaultsOnly` 属性，列间距和文字样式改由 Blueprint 中直接设置。

#### Scenario: 旧属性不可访问
- **WHEN** Blueprint 打开 `WBP_ListEntry_Session` 的 Class Defaults
- **THEN** `SessionTextStyle`、`ColumnGap`、`EdgePadding`、`VerticalPadding` 不再出现在 Details 面板中
