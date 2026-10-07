# Multiplayer JoinGame Tab — Session 列表 (子行)

**Date:** 2026-08-05
**Status:** draft
**Parent:** [HeaderRow 设计](./2026-08-04-multiplayer-header-row-design.md)

## 目标

在 JoinGame tab 的 HeaderRow 下方，每个发现的 Session 显示为一行子项，列与 HeaderRow 对齐：

```
[ ServerName    Map      Players    Private    Ping ]   ← HeaderRow
  MyServer      DeathMatch  3/16     Yes       42ms   ← Session
  TestRoom      Sandbox     1/8      No        120ms  ← Session
```

## 方案: 平面列表 + SessionDataObject

沿用现有 ListEntry 架构，新建 `UListDataObject_Session`(数据) 和 `W_ListEntry_Session`(行控件)。BP 通过 AdvancedSessions 查到 Session 后，调 C++ 暴露的 `BlueprintCallable` 注入列表。

## 改动明细

### 1. 数据结构 — `FSessionListItem`

`ListDataObject_Session.h` 中定义，BP 可填充的 struct：

```cpp
USTRUCT(BlueprintType)
struct FSessionListItem
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    int32 SessionIndex = -1;

    UPROPERTY(BlueprintReadWrite)
    FText ServerName;

    UPROPERTY(BlueprintReadWrite)
    FText MapName;

    UPROPERTY(BlueprintReadWrite)
    int32 CurrentPlayers = 0;

    UPROPERTY(BlueprintReadWrite)
    int32 MaxPlayers = 0;

    UPROPERTY(BlueprintReadWrite)
    bool bIsPrivate = false;

    UPROPERTY(BlueprintReadWrite)
    int32 Ping = 0;
};
```

`SessionIndex` 用于 BP 回查对应的 `FBlueprintSessionResult`，在 Join 操作时使用。

### 2. 数据对象 — `ListDataObject_Session.h/.cpp`

继承 `UListDataObject_Base`。纯数据容器，无交互逻辑：

```cpp
UCLASS()
class UListDataObject_Session : public UListDataObject_Base
{
    GENERATED_BODY()
public:
    void SetSessionData(const FSessionListItem& InData);
    const FSessionListItem& GetSessionData() const { return SessionData; }

private:
    FSessionListItem SessionData;
};
```

不参与 Save/Load（`SaveSessionData` / `LoadSessionData` 中 Cast 失败即 continue，无需特殊处理）。

### 3. Entry 控件 — `W_ListEntry_Session.h/.cpp`

继承 `UW_ListEntry_Base`。UMG 中放置 `Hb_SessionColumns` (UHorizontalBox)。

`OnOwningListDataObjectSet` 时动态填充，结构与 HeaderRow 一致：
- 同样的列数、同样的 Gap/Edge/VerticalPadding
- 每列一个 `UCommonTextBlock`
- Players 显示为 `"CurrentPlayers/MaxPlayers"` 格式

```cpp
void UW_ListEntry_Session::OnOwningListDataObjectSet(UListDataObject_Base* InOwningListDataObject)
{
    Super::OnOwningListDataObjectSet(InOwningListDataObject);

    UListDataObject_Session* SessionData = CastChecked<UListDataObject_Session>(InOwningListDataObject);
    if (!Hb_SessionColumns) return;

    Hb_SessionColumns->ClearChildren();

    const FSessionListItem& Data = SessionData->GetSessionData();
    const FText PlayersText = FText::Format(
        FText::FromString(TEXT("{0}/{1}")),
        FText::AsNumber(Data.CurrentPlayers),
        FText::AsNumber(Data.MaxPlayers));

    TArray<FText> ColumnTexts = {
        Data.ServerName,
        Data.MapName,
        PlayersText,
        Data.bIsPrivate ? FText::FromString(TEXT("Yes")) : FText::FromString(TEXT("No")),
        FText::Format(FText::FromString(TEXT("{0}ms")), FText::AsNumber(Data.Ping)),
    };

    for (int32 i = 0; i < ColumnTexts.Num(); ++i)
    {
        UCommonTextBlock* TextBlock = NewObject<UCommonTextBlock>(this);
        TextBlock->SetText(ColumnTexts[i]);
        // padding logic: same as HeaderRow
    }
}
```

`OnToggleEditableState` 重写为空（Session 行不可编辑）。

### 4. Registry — `MultiplayerDataRegistry`

新增方法，清空 JoinGame tab 下的 Session 条目并重建：

```cpp
void UMultiplayerDataRegistry::RebuildSessionList(
    const TArray<FSessionListItem>& Sessions,
    float InColumnGap,
    float InEdgePadding,
    float InVerticalPadding);
```

逻辑：
1. 找到 `TabId_JoinGame` 的 Collection
2. 遍历其子节点，移除所有 `UListDataObject_Session`（保留 HeaderRow 和其他非 Session 项）
3. 为每个 `FSessionListItem` 创建 `UListDataObject_Session` 并添加为子节点

### 5. Screen — `W_MultiplayerScreen`

新增 `BlueprintCallable`，BP 在 FindSessions 回调中调用：

```cpp
UFUNCTION(BlueprintCallable, Category = "Multiplayer Screen")
void PopulateSessionList(const TArray<FSessionListItem>& Sessions);
```

实现：调 `MultiplayerDataRegistry::RebuildSessionList`，若当前在 JoinGame tab 则刷新 ListView。

### 6. Entry 映射 — `DA_ListEntryMapping`

DataAsset 中加一行: `UListDataObject_Session → WBP_ListEntry_Session`

### 7. 新增 Blueprint Widget — `WBP_ListEntry_Session`

在编辑器中创建，继承 `W_ListEntry_Session`，放置 `Hb_SessionColumns` (UHorizontalBox)。

## 数据流

```
BP: FindSessions (AdvancedSessions)
    │
    ▼
BP: 遍历 Results → 构造 TArray<FSessionListItem>
    │
    ▼
BP: 调用 PopulateSessionList(Sessions)
    │
    ▼
C++: MultiplayerDataRegistry::RebuildSessionList()
    │  清空旧 UListDataObject_Session
    │  创建新的插入 JoinGame tab 子节点
    │
    ▼
C++: ListView->SetListItems() + RequestRefresh()
    │
    ▼
UFrontendCommonListView
    │  DA_ListEntryMapping → W_ListEntry_Session
    ▼
[ServerName  Players  Ping  Status]   ← HeaderRow
[MyServer    3/16     42ms  Waiting]  ← Session
[TestRoom    1/8      120ms  Running] ← Session
```

## 待定 / 后续

- **列宽显式对齐**: 当前 HeaderRow 和 Session 行都用 padding 方式，宽列内容差异大时可能不对齐。后续可给所有列的 TextBlock 加显式 `SetMinDesiredWidth`，通过 `UMenuCategoryConfig` 新增 `HeaderColumnWidths` 字段配置
- **Session 选中→ Join**: 列表选中某 Session 后，焦点移到 JoinGameAction，BP 中通过 `GetSelectedItem` → `UListDataObject_Session::SessionIndex` 拿到索引，Join 对应的 `FBlueprintSessionResult`
- **密码 Session 弹窗**: 选中 Private Session 点 Join 时，BP 弹出密码输入框后再 Join —— 纯 BP 逻辑，不涉及 C++ 改动
