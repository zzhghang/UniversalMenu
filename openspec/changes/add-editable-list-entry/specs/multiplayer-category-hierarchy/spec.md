## ADDED Requirements

### Requirement: CreateGame 标签页具有基础与高级分类
系统 SHALL 在 CreateGame 标签页下提供"基础"（Basic）和"高级"（Advanced）两个 category，通过 `UListDataObject_Collection` 作为中间层组织条目。

#### Scenario: 基础 category 包含房间名和密码相关条目
- **WHEN** 用户选中 CreateGame 标签页并展开"基础" category
- **THEN** 列表显示房间名、是否启用密码、密码三个条目

#### Scenario: 高级 category 包含其他服务器设置
- **WHEN** 用户选中 CreateGame 标签页并展开"高级" category
- **THEN** 列表显示最大玩家数、地图选择、游戏模式、服务器地区等条目

### Requirement: 密码开关控制密码输入框的编辑状态
系统 SHALL 使用 `AddEditDependencyData` 将密码条目绑定到"是否启用密码"开关。当开关为 false 时，密码条目不可编辑（灰色禁用态）。

#### Scenario: 密码开关关闭
- **WHEN** "是否启用密码"条目值为 false
- **THEN** 密码条目处于不可编辑状态，EditableText 禁用

#### Scenario: 密码开关打开
- **WHEN** "是否启用密码"条目值为 true
- **THEN** 密码条目处于可编辑状态，EditableText 启用

### Requirement: MultiplayerDataRegistry 支持数据驱动模式
系统 SHALL 通过 `UMultiplayerConfigData` DataAsset 配置所有 tab/category/entry，`InitFromConfig()` 遍历 `Tabs→Categories→Entries` 构建数据树，无 DataAsset 时回退硬编码 fallback。

#### Scenario: DataAsset 配置驱动
- **WHEN** `MultiplayerConfig` DataAsset 已赋值且包含 CreateGame tab 的 categories
- **THEN** registry 按配置构建 tab→category→entry 结构，不执行硬编码分支

#### Scenario: 无 DataAsset 回退硬编码
- **WHEN** `MultiplayerConfig` 为空或不包含任何 tab
- **THEN** registry 使用硬编码数据构建基础+高级两个 category 及其条目
