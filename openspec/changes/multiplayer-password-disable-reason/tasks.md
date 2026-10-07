## 1. 基础设施 — 本地化 Key 与 DataAsset 字段

- [x] 1.1 `LocalizationKeys.h` 新增 `TXT_MP_WARN_PASSWORD_DISABLED()` 宏，指向 `ST_Multiplayer` 的 `Warning_PasswordDisabled`
- [x] 1.2 `MenuEntryConfig.h` 新增 `FText DisableRichReason` 和 `FText DynamicDetails` 字段
- [ ] 1.3 `ST_Multiplayer` 字符串表中新增 `Warning_PasswordDisabled` 条目（英文文本："Password is disabled when Has Password is turned off"）
  - **需手动操作**: ST_Multiplayer.uasset 是二进制资源文件，需在 UE 编辑器中打开 `Content/BP/StringTable/ST_Multiplayer` 添加 key `Warning_PasswordDisabled`，值为 `"Password is disabled when Has Password is turned off"`

## 2. 核心逻辑 — 多 Reason 换行分隔

- [x] 2.1 `ListDataObject_Base.cpp` 的 `IsDataCurrentlyEditable()` 中，`CachedDisableRichReason.Append` 前增加换行判断：已有内容时先追加 `\n`

## 3. 核心逻辑 — DynamicDetails 数据层

- [x] 3.1 `ListDataObject_Base.h` 新增 `DynamicDetailsRichText` 字段（`FText`）及 `GET/SET` 宏访问器
- [x] 3.2 `ListDataObject_Base.cpp` 在 `OnDataObjectInitialized` 中初始化 DynamicDetails 为空（默认行为已覆盖，无需额外操作）

## 4. MultiplayerScreen — Password 禁用原因

- [x] 4.1 `MultiplayerDataRegistry.cpp` 中 Password 的 EditCondition 处增加 `SetDisableRichReason(TXT_MP_WARN_PASSWORD_DISABLED().ToString())`

## 5. DetailView — DynamicDetails 显示复活

- [x] 5.1 `W_OptionDetailView.cpp` 的 `UpdateDetailsViewInfo()` 中取消 DynamicDetails 注释，改为从 `InDataObject` 读取 DynamicDetails 文本并 set 到 `Ctb_DynamicDetails`

## 6. DataAsset 路径 — 配置下发

- [x] 6.1 `MenuConfigData.cpp` 的 `CreateEntryFromConfig` 中，读取 `UMenuEntryConfig::DisableRichReason` 和 `DynamicDetails`，设置到创建的 DataObject 上
