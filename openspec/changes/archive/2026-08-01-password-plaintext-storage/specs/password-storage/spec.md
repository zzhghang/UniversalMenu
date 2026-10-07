## ADDED Requirements

### Requirement: 密码以明文存储
`UListDataObject_Editable` 获取密码值时 SHALL 直接返回用户输入的文本，不进行哈希。

#### Scenario: 输入密码后保存
- **WHEN** 用户在密码输入框中输入 "123456" 并保存 Session
- **THEN** `UMenuSessionData` 中存储的密码值 SHALL 为 "123456"

#### Scenario: 空密码
- **WHEN** 用户未输入密码（文本框为空）
- **THEN** 存储的密码值 SHALL 为空字符串

### Requirement: 输入字符过滤
`UListDataObject_Editable::SetText()` SHALL 根据 `CharFilter` 模式过滤非法字符：

| 模式 | 允许字符 |
|------|----------|
| `All` | 全部字符 |
| `NumbersOnly` | 仅数字 0-9 |
| `LettersOnly` | 仅字母 A-Z/a-z |
| `Alphanumeric` | 数字 + 字母 |

#### Scenario: 仅数字模式下输入混合字符
- **WHEN** `CharFilter` 为 `NumbersOnly` 且输入 "abc123"
- **THEN** `CurrentText` SHALL 为 "123"

#### Scenario: 仅字母模式下输入数字
- **WHEN** `CharFilter` 为 `LettersOnly` 且输入 "abc123def"
- **THEN** `CurrentText` SHALL 为 "abcdef"

#### Scenario: 混合模式下输入特殊字符
- **WHEN** `CharFilter` 为 `Alphanumeric` 且输入 "pass@123!"
- **THEN** `CurrentText` SHALL 为 "pass123"

#### Scenario: 全部字符模式
- **WHEN** `CharFilter` 为 `All` 且输入任意字符
- **THEN** `CurrentText` SHALL 与输入一致（无过滤）

### Requirement: 最大字符数限制
`UListDataObject_Editable::SetText()` SHALL 将输入截断到 `MaxCharCount` 个字符。

#### Scenario: 超出限制截断
- **WHEN** `MaxCharCount` 为 6 且输入 "1234567890"
- **THEN** `CurrentText` SHALL 为 "123456"

#### Scenario: 未超出限制保留完整
- **WHEN** `MaxCharCount` 为 6 且输入 "123"
- **THEN** `CurrentText` SHALL 为 "123"

#### Scenario: 过滤后超出截断
- **WHEN** `CharFilter` 为 `NumbersOnly`、`MaxCharCount` 为 3 且输入 "1a2b3c4d"
- **THEN** `CurrentText` SHALL 为 "123"（先过滤得 "1234"，再截断至 "123"）

### Requirement: 输入校验提示
`UListDataObject_Editable::ValidateText()` SHALL 在校验失败时通过 `OutReason` 返回对应错误提示。

#### Scenario: 超长输入校验失败
- **WHEN** `ValidateText()` 传入长度超过 `MaxCharCount` 的文本
- **THEN** 返回 `false`，`OutReason` 为 `TXT_MP_VALIDATION_MAX_CHARS` 的格式化文本

#### Scenario: 非法字符校验失败
- **WHEN** `ValidateText()` 传入包含不允许字符的文本
- **THEN** 返回 `false`，`OutReason` 为对应过滤模式的错误提示
