# 代码审查报告：多人游戏界面功能

**日期**：2026-07-21  
**分支**：master  
**审查范围**：暂存区及未跟踪文件（多人游戏模块 + 已有代码修改）

---

## 审查结果总览

| 等级 | 数量 | 说明 |
|------|------|------|
| 🔴 严重 | 1 | 会导致运行时错误的 Bug |
| 🟡 建议 | 5 | 代码质量 / 安全隐患 / 用户体验 |
| 🔵 可选 | 2 | 改进建议，不影响功能 |
| ✅ 通过 | - | 架构一致性、已有代码修复 |

---

## 🔴 严重问题

### 1. Config/DefaultEngine.ini — 路径拼写错误

**文件**：`Config/DefaultEngine.ini:113`  
**问题**：`/Scripts/` 多了一个字母 `s`

```ini
# 错误 ❌
NetConnectionClassName="/Scripts/SteamSockets.SteamSocketsNetConnection"

# 正确 ✅
NetConnectionClassName="/Script/SteamSockets.SteamSocketsNetConnection"
```

**影响**：运行时会因找不到类而创建 NetConnection 失败，多人游戏联网功能无法工作。  
**修复**：删掉多余的 `s`，改为 `/Script/SteamSockets.SteamSocketsNetConnection`。

---

## 🟡 建议修复

### 2. ListDataObject_Editable.cpp — 使用 SHA1 做密码哈希

**文件**：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/DataObjects/ListDataObject_Editable.cpp:46`  
**问题**：`FSHA1::HashBuffer` 已是被认为不安全的哈希算法。

```cpp
// 当前 ❌
FTCHARToUTF8 Utf8Str(*PlainText);
const FSHAHash Hash = FSHA1::HashBuffer(reinterpret_cast<const uint8*>(Utf8Str.Get()), Utf8Str.Length());
return Hash.ToString();

// 建议 ✅ — 同一头文件中就有 FSHA256
const FSHA256Hash Hash = FSHA256::HashBuffer(reinterpret_cast<const uint8*>(Utf8Str.Get()), Utf8Str.Length());
return LexToString(Hash);
```

**理由**：`Misc/SecureHash.h` 已经同时提供了 `FSHA1` 和 `FSHA256`，一行改动即可。游戏房间密码场景虽然安全要求不高，但既然已有更好的替代，没必要用弱算法。

---

### 3. MultiplayerDataRegistry.cpp — 不必要的 const_cast

**文件**：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/MultiplayerDataRegistry.cpp:241`

```cpp
// 当前 ❌ — 用 const_cast 去除 const
FindChildListDataRecursively(const_cast<UListDataObject_Collection*>(Tab), FlatList);

// 根本原因：FindChildListDataRecursively 声明为非 const 指针
// void FindChildListDataRecursively(UListDataObject_Base* InParentData, ...)

// 修复 ✅ — 改为 const 指针即可
void FindChildListDataRecursively(const UListDataObject_Base* InParentData, ...) const;
```

**理由**：该函数只读取数据，不修改任何内容。将参数和函数都加上 `const`，去掉 `const_cast`，零性能开销，更安全。

---

### 4. ListDataObject_Editable.cpp — 死代码 FMath::Max

**文件**：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/DataObjects/ListDataObject_Editable.cpp:29`

```cpp
// 当前 ❌
const FString Truncated = Filtered.Left(FMath::Max(0, MaxCharCount));

// 建议 ✅
const FString Truncated = Filtered.Left(MaxCharCount);
```

**理由**：`OnDataObjectInitialized()`（同文件第 99-102 行）已经保证 `MaxCharCount >= 6`（密码默认 6，其他默认 16）。`FMath::Max(0, MaxCharCount)` 等价于 `MaxCharCount`，属于无效防御代码。

---

### 5. W_ListEntry_Editable.cpp — 字符过滤弹窗体验差

**文件**：`Plugins/GameMainMenu/Source/GameMainMenu/Private/Widgets/Multiplayer/ListEntries/W_ListEntry_Editable.cpp:117`

```cpp
// 当前行为：字符被过滤掉后按回车，无论是否验证通过都会弹窗
const FText& Message = Reason.IsEmpty() ? TXT_MP_VALIDATION_FILTERED() : Reason;
```

**问题**：在"仅数字"限制的密码框中输入 `abc123`，字符已经被实时过滤掉只剩 `123`，按回车时却弹窗告诉用户"输入内容已被自动过滤"。这属于多此一举——用户已经看到字符打不进去了。

**建议**：仅在 `ValidateText` 返回失败（`!Reason.IsEmpty()`）时弹窗，字符级过滤静默处理即可。或者直接删掉整个 `TryCommitValidation` 的弹窗逻辑，因为字符已经实时过滤了，保留截断/超长提示即可。

---

### 6. MultiplayerDataRegistry.cpp — static FName 跨文件重复定义

**文件**：
- `MultiplayerDataRegistry.cpp:12-13` → `TabId_CreateGame` / `TabId_JoinGame`
- `W_MultiplayerScreen.cpp:50` → 硬编码 `FName(TEXT("CreateGame"))`

**问题**：同一个 TabId 在两个地方各自硬编码，值相同但无关联。

**建议**：将 TabId 常量移到 `MultiplayerDataRegistry.h` 作为 `static const FName` 暴露出去，或者直接用 `FrontendGameplayTags` 中已有的 Tag 作为 TabId（这样 SelectTabByID 和 RequestRegisterTab 都走同一套 ID 体系）。

---

## 🔵 可选改进

### 7. W_MultiplayerScreen — 缺少 Reset 功能

`UW_OptionScreen` 有 `OnResetBoundActionTriggered`、`OnResetAllBoundActionTriggered`、`OnListViewListDataModified` 来支持"恢复到默认值"。`UW_MultiplayerScreen` 完全没有这些。

**评估**：目前影响不大——多人界面字段较少（房间名、密码等），用户没有重置单个字段的诉求。如果后续字段增多，参考 `UW_OptionScreen` 的 Reset 绑定模式补上即可。

### 8. Config/DefaultEngine.ini — 文件末尾缺少换行符

文件最后一行 `bInitServerOnClient=true` 之后没有换行。部分工具（git、diff）会警告。补一个空行即可。

---

## ✅ 做得好的地方

- **架构一致性**：`MultiplayerConfigData → MultiplayerDataRegistry → W_MultiplayerScreen → ListView + DetailView` 完全复用了 Options 系统的模式，新增模块而没有引入新概念。
- **`NotifyListDataModified` Bug 修复**：`ListDataObject_String.cpp` 中把调用移到 `if (DataDynamicSetter)` 外部——原来 setter 为 null 时 UI 不会刷新，修得很准。
- **StringTable 预加载**：`GameMainMenu.cpp` 的 `StartupModule` 中同步预加载，解决了 `LOCTABLE` 在资产异步加载前查不到 key 的问题。ponytail 注释解释了动机，赞。
- **Ticker 延迟 SetText**：`W_ListEntry_Editable.cpp:39` 中用一帧延迟避免 OnTextChanged 期间操作 Slate 状态损坏。这是 UE 中常见的正确的 workaround。
- **新文件组织清晰**：`DataObjects/`、`ListEntries/`、`MultiplayerConfigData.h`、`MultiplayerDataRegistry.h`、`W_MultiplayerScreen.h` 分层合理，一眼能看懂职责。

---

## 修复优先级

```
1. [立即] DefaultEngine.ini 拼写错误 → 改为 /Script/
2. [本次提交] SHA1 → SHA256
3. [本次提交] const_cast 去不掉就加 const
4. [本次提交] 删掉 FMath::Max(0, MaxCharCount)
5. [可延后] 字符过滤弹窗体验优化
6. [可延后] TabId 常量统一
```
