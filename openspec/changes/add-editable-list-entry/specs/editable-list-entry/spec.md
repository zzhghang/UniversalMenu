## ADDED Requirements

### Requirement: 用户可输入房间名文本
系统 SHALL 通过 `UListDataObject_Editable` 提供房间名输入数据对象，支持中英文及任意字符，字符数上限可配置（通过 `MaxCharCount`）。

#### Scenario: 输入英文字符
- **WHEN** 用户在 EditableText 中输入英文房间名
- **THEN** 文本正确存储到 `CurrentText`，不超过 `MaxCharCount` 限制

#### Scenario: 输入中文字符
- **WHEN** 用户使用中文输入法输入中文房间名
- **THEN** 中文字符正确存储，每个中文字符计为 1 个字符

#### Scenario: 超出字符限制
- **WHEN** 输入文本超过 `MaxCharCount`
- **THEN** 系统截断到最大字符数，不接受超出部分

### Requirement: 用户可输入密码并支持位数限制
系统 SHALL 通过 `UListDataObject_Editable` 提供密码输入数据对象，`bIsPassword` 标记为密码类型时启用掩码显示，`MaxCharCount` 控制密码位数上限。

#### Scenario: 密码掩码显示
- **WHEN** `bIsPassword` 为 true 且用户在 EditableText 中输入
- **THEN** 输入字符以掩码（*）形式显示

#### Scenario: 密码位数限制
- **WHEN** 用户输入密码超过 `MaxCharCount` 位数
- **THEN** 系统截断到最大位数

### Requirement: 密码哈希存储不存明文
系统 SHALL 对密码进行哈希处理后通过 `GetHashedPassword()` 暴露，`UListDataObject_Editable` 内部不持久化明文密码字符串。

#### Scenario: 获取哈希密码
- **WHEN** 调用 `GetHashedPassword()`
- **THEN** 返回 `CurrentText` 的 SHA1 哈希值，长度固定为 40 字符的十六进制字符串

#### Scenario: 空密码
- **WHEN** `CurrentText` 为空字符串
- **THEN** `GetHashedPassword()` 返回空字符串

### Requirement: EditableText 控件绑定
系统 SHALL 通过 `UW_ListEntry_Editable` 绑定 UE 的 `EditableText` 控件，响应用户文本输入并更新数据对象。

#### Scenario: 文本变更回调
- **WHEN** 用户在 EditableText 中修改文本
- **THEN** `OnEditableTextChanged` 调用 `OwningListDataObject->SetText()`，广播 `OnListDataModified`

#### Scenario: 控件初始化
- **WHEN** `UW_ListEntry_Editable` 收到 `OnOwningListDataObjectSet`
- **THEN** EditableText 初始化为数据对象当前的 `CurrentText`，`MaxCharCount` 和密码掩码从数据对象同步到控件
