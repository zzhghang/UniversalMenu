# OptionsDataRegistry 重构方案

## 现状问题

`OptionsDataRegistry.cpp` 约 959 行，4 个 `Init*CollectionTab` 方法合计 ~900 行 `NewObject` + setter 样板代码。每新增一个选项需写 20-30 行重复代码，结构完全一致但内容不同。

---

## 方案 A：DataAsset 分层配置（推荐，长期目标）

### 思路

用 `UDataAsset` 把选项结构暴露给编辑器，策划在蓝图中可视化编辑。Tab → Category → Entry 三层嵌套 DataAsset。

### 数据结构

```cpp
// ---- Entry 级别 ----

UENUM()
enum class EOptionValueType : uint8
{
    Bool,
    Scalar,
    String,
    Enum,
    Culture,
    Resolution,
    KeyBinding
};

UENUM()
enum class EGameUserSettingFunction : uint8
{
    None,
    // Gameplay
    GetDifficulty, SetDifficulty,
    GetShowHealthBar, SetShowHealthBar,
    GetShowDamageNumbers, SetShowDamageNumbers,
    GetHighlightInteraction, SetHighlightInteraction,
    GetShowSubtitles, SetShowSubtitles,
    GetAutoSave, SetAutoSave,
    GetCurrentCulture, SetCurrentCulture,
    // Audio
    GetOverallVolume, SetOverallVolume,
    GetMusicVolume, SetMusicVolume,
    GetSFXVolume, SetSFXVolume,
    GetVoiceVolume, SetVoiceVolume,
    GetAmbientVolume, SetAmbientVolume,
    GetUIVolume, SetUIVolume,
    GetAllowBackgroundSound, SetAllowBackgroundSound,
    GetSubtitlesEnabled, SetSubtitlesEnabled,
    GetHRTFEnabled, SetHRTFEnabled,
    GetMonoOutput, SetMonoOutput,
    GetDynamicRangeCompression, SetDynamicRangeCompression,
    GetDialogueBoost, SetDialogueBoost,
    // Video
    GetFullscreenMode, SetFullscreenMode,
    GetScreenResolution, SetScreenResolution,
    GetCurrentDisplayGamma, SetCurrentDisplayGamma,
    GetOverallScalabilityLevel, SetOverallScalabilityLevel,
    GetResolutionScaleNormalized, SetResolutionScaleNormalized,
    GetGlobalIlluminationQuality, SetGlobalIlluminationQuality,
    GetShadowQuality, SetShadowQuality,
    GetAntiAliasingQuality, SetAntiAliasingQuality,
    GetViewDistanceQuality, SetViewDistanceQuality,
    GetTextureQuality, SetTextureQuality,
    GetVisualEffectQuality, SetVisualEffectQuality,
    GetReflectionQuality, SetReflectionQuality,
    GetPostProcessingQuality, SetPostProcessingQuality,
    IsVSyncEnabled, SetVSyncEnabled,
    GetFrameRateLimit, SetFrameRateLimit,
};

UCLASS(BlueprintType)
class UOptionEntryData : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    FName DataId;

    UPROPERTY(EditAnywhere)
    FText DisplayName;

    UPROPERTY(EditAnywhere)
    FText Description;

    UPROPERTY(EditAnywhere)
    EOptionValueType ValueType;

    // ---- Getter / Setter 绑定 ----
    UPROPERTY(EditAnywhere)
    EGameUserSettingFunction GetterFunc;

    UPROPERTY(EditAnywhere)
    EGameUserSettingFunction SetterFunc;

    UPROPERTY(EditAnywhere)
    bool bApplyChangeImmediately = true;

    // ---- Scalar 专用 ----
    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
    float ScalarMin = 0.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
    float ScalarMax = 1.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
    float ScalarOutMin = 0.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
    float ScalarOutMax = 2.f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
    float StepSize = 0.01f;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::Scalar", EditConditionHides))
    ECommonNumericType NumericType = ECommonNumericType::Percentage;

    // ---- String/Enum 专用 ----
    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::String || ValueType == EOptionValueType::Enum", EditConditionHides))
    TMap<FString, FText> AvailableOptions;  // Key=存盘值, Value=显示文本

    UPROPERTY(EditAnywhere)
    FString DefaultValue;

    // ---- 质量控制专用 ----
    UPROPERTY(EditAnywhere, meta = (EditCondition = "ValueType == EOptionValueType::String && QualityOptions", EditConditionHides))
    TArray<int32> IntegerValues;  // 0=Low, 1=Medium, 2=High, 3=Epic, 4=Cinematic

    // ---- 编辑条件（只在打包构建可调等） ----
    UPROPERTY(EditAnywhere)
    bool bPackagedBuildOnly = false;

    UPROPERTY(EditAnywhere, meta = (EditCondition = "bPackagedBuildOnly", EditConditionHides))
    FText PackagedBuildWarning;

    // ---- 依赖（如 OverallQuality 变化时联动） ----
    UPROPERTY(EditAnywhere)
    TArray<FName> EditDependencyIds;  // 依赖的其他 Entry 的 DataId
};

// ---- Category 级别 ----

UCLASS(BlueprintType)
class UOptionCategoryData : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    FName CategoryId;

    UPROPERTY(EditAnywhere)
    FText CategoryName;

    UPROPERTY(EditAnywhere)
    TArray<UOptionEntryData*> Entries;
};

// ---- Tab 级别 ----

UCLASS(BlueprintType)
class UOptionTabData : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    FName TabId;

    UPROPERTY(EditAnywhere)
    FText TabName;

    UPROPERTY(EditAnywhere)
    TArray<UOptionCategoryData*> Categories;
};

// ---- 总配置 ----

UCLASS(BlueprintType)
class UOptionsConfigData : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    TArray<UOptionTabData*> Tabs;
};
```

### Registry 变为 ~60 行

```cpp
void UOptionsDataRegistry::InitOptionsDataRegistry(ULocalPlayer* InOwningLocalPlayer)
{
    if (!OptionsConfig)
    {
        UE_LOG(LogTemp, Error, TEXT("OptionsConfig DataAsset not set"));
        return;
    }

    for (UOptionTabData* TabData : OptionsConfig->Tabs)
    {
        UListDataObject_Collection* TabObj = CreateCollection(TabData->TabId, TabData->TabName);

        for (UOptionCategoryData* CatData : TabData->Categories)
        {
            UListDataObject_Collection* CatObj = CreateCollection(CatData->CategoryId, CatData->CategoryName);

            for (UOptionEntryData* EntryData : CatData->Entries)
            {
                UListDataObject_Base* Entry = CreateEntryFromData(EntryData);
                if (Entry)
                {
                    CatObj->AddChildListData(Entry);
                }
            }

            TabObj->AddChildListData(CatObj);
        }

        RegisteredOptionsCollectionTabs.Add(TabObj);
    }

    // 按键绑定仍需动态生成
    InitControlCollectionTab(InOwningLocalPlayer);
}

UListDataObject_Base* UOptionsDataRegistry::CreateEntryFromData(UOptionEntryData* Data)
{
    switch (Data->ValueType)
    {
    case EOptionValueType::Bool:
        return CreateBoolEntry(Data);
    case EOptionValueType::Scalar:
        return CreateScalarEntry(Data);
    case EOptionValueType::String:
    case EOptionValueType::Enum:
        return CreateStringEntry(Data);
    default:
        return nullptr;
    }
}
```

### 优缺点

| 优点 | 缺点 |
|------|------|
| 新增选项零 C++ 代码 | 需要重构现有 DataObject 体系 |
| 策划在编辑器可视化编辑 | 枚举 `EGameUserSettingFunction` 需维护 |
| DataAsset 可复用、可版本控制 | 复杂逻辑（EditCondition 闭包）需额外处理 |
| 支持继承和覆盖（不同平台不同配置） | 首次搭建工作量大 |

---

## 方案 B：DataTable 驱动

### 思路

用一张 DataTable（CSV/Excel 可导入），每行就是一个选项。适合选项数量极大且结构简单的场景。

### 数据结构

```cpp
USTRUCT(BlueprintType)
struct FOptionEntryRow : public FTableRowBase
{
    GENERATED_BODY()

    // ---- 层级 ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TabId;          // "Tab-Audio"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CategoryId;     // "Category-Volume"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EntryId;        // "Entry-OverallVolume"

    // ---- 显示 ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;    // 可用 StringTable Key 或直接文本

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    // ---- 类型 ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ValueType;    // "Bool" / "Scalar" / "String" / "Enum" / "Culture" / "Resolution"

    // ---- Getter/Setter（反射函数名） ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString GetterFuncName;  // "GetOverallVolume"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SetterFuncName;  // "SetOverallVolume"

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DefaultValue;    // "1.0" / "true" / "Normal" / "en"

    // ---- Scalar 专用 ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MinValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxValue = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float OutMinValue = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float OutMaxValue = 2.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float StepSize = 0.01f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bShowAsPercentage = true;

    // ---- String/Enum 选项列表 ----
    // 格式: "Easy|Normal|Hard" 用 | 分隔，默认第一个
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString AvailableValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DisplayLabels;  // 对应的显示文本，"Easy|Normal|Hard"，空则用 AvailableValues

    // ---- 编辑条件 ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPackagedBuildOnly = false;

    // ---- 依赖（逗号分隔的 EntryId） ----
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DependencyIds;  // "Entry-OverallQuality"
};
```

### CSV 示例

```csv
TabId,CategoryId,EntryId,DisplayName,Description,ValueType,GetterFuncName,SetterFuncName,DefaultValue,MinValue,MaxValue,OutMinValue,OutMaxValue,StepSize,AvailableValues,bPackagedBuildOnly
Tab-Audio,Category-Volume,Entry-OverallVolume,"Overall Volume","Adjust the overall volume level of the game.",Scalar,GetOverallVolume,SetOverallVolume,1.0,0.0,1.0,0.0,2.0,0.01,,false
Tab-Audio,Category-Volume,Entry-MusicVolume,"Music Volume","Adjust the music volume level.",Scalar,GetMusicVolume,SetMusicVolume,1.0,0.0,1.0,0.0,2.0,0.01,,false
Tab-Audio,Category-Sound,Entry-3DSpatialAudio,"3D Spatial Audio","Enable HRTF-based 3D spatial audio for headphones.",Bool,GetHRTFEnabled,SetHRTFEnabled,true,,,,,,,false
Tab-Gameplay,Category-Game,Entry-Difficulty,"Difficulty","Set the game difficulty level.",String,GetDifficulty,SetDifficulty,Normal,,,,,,"Easy|Normal|Hard|Nightmare",false
Tab-Video,Category-Graphics,Entry-OverallQuality,"Overall Quality","Sets the overall graphics quality preset.",String,GetOverallScalabilityLevel,SetOverallScalabilityLevel,Medium,,,,,,"Low|Medium|High|Epic|Cinematic",false
Tab-Video,Category-Display,Entry-Resolution,"Resolution","Select the screen resolution.",Resolution,GetScreenResolution,SetScreenResolution,,,,,,,true
```

### Registry 变为 ~80 行

```cpp
void UOptionsDataRegistry::InitFromDataTable()
{
    static const FString ContextString(TEXT("OptionsDataTable"));
    TArray<FOptionEntryRow*> Rows;
    OptionsTable->GetAllRows(ContextString, Rows);

    // 按 Tab → Category 层级组装
    TMap<FName, UListDataObject_Collection*> TabMap;
    TMap<FName, UListDataObject_Collection*> CategoryMap;

    for (FOptionEntryRow* Row : Rows)
    {
        // 确保 Tab 存在
        if (!TabMap.Contains(Row->TabId))
        {
            UListDataObject_Collection* Tab = CreateCollection(Row->TabId, FText::FromString(Row->TabId.ToString()));
            TabMap.Add(Row->TabId, Tab);
            RegisteredOptionsCollectionTabs.Add(Tab);
        }

        // 确保 Category 存在
        FName CatKey(Row->TabId, Row->CategoryId);
        if (!CategoryMap.Contains(CatKey))
        {
            UListDataObject_Collection* Cat = CreateCollection(Row->CategoryId, FText::FromString(Row->CategoryId.ToString()));
            CategoryMap.Add(CatKey, Cat);
            TabMap[Row->TabId]->AddChildListData(Cat);
        }

        // 创建 Entry
        UListDataObject_Base* Entry = CreateEntryFromRow(Row);
        if (Entry)
        {
            CategoryMap[CatKey]->AddChildListData(Entry);
        }
    }
}
```

### 优缺点

| 优点 | 缺点 |
|------|------|
| CSV/Excel 编辑，策划最熟悉 | 复杂依赖（EditCondition 闭包）难以在表中表达 |
| 支持批量导入导出 | 函数名是裸字符串，重命名时编译期不报错 |
| 不同平台可用不同表 | 不易表达条件显示逻辑 |
| DataTable 是 UE 标准资产类型 | KeyBinding、Culture 等特殊类型需要额外处理 |

---

## 方案 C：工厂方法提取（最小改动，立即可用）

### 思路

在现有代码基础上提取工厂辅助方法，消除样板代码。不改变数据结构，不改变架构，只减少行数。

### 工厂方法

```cpp
// ---- 通用集合创建 ----
UListDataObject_Collection* UOptionsDataRegistry::CreateCollection(
    const FName& Id, const FText& Name) const
{
    UListDataObject_Collection* Col = NewObject<UListDataObject_Collection>();
    Col->SetDataId(Id);
    Col->SetDataDisplayName(Name);
    return Col;
}

// ---- Bool 选项 ----
UListDataObject_StringBool* UOptionsDataRegistry::CreateBoolEntry(
    const FName& Id, const FText& Name, const FText& Desc,
    const FName& GetterFnName, const FName& SetterFnName,
    bool bDefaultValue, bool bApplyImmediately) const
{
    auto* Entry = NewObject<UListDataObject_StringBool>();
    Entry->SetDataId(Id);
    Entry->SetDataDisplayName(Name);
    Entry->SetDescriptionRichText(Desc);
    Entry->SetDataDynamicGetter(MakeShared<FOptionsDataInteractionHelper>(GetterFnName));
    Entry->SetDataDynamicSetter(MakeShared<FOptionsDataInteractionHelper>(SetterFnName));
    Entry->SetShouldApplyChangeImmediately(bApplyImmediately);
    bDefaultValue ? Entry->SetTrueAsDefaultValue() : Entry->SetFalseAsDefaultValue();
    return Entry;
}

// ---- Scalar 选项 ----
UListDataObject_Scalar* UOptionsDataRegistry::CreateScalarEntry(
    const FName& Id, const FText& Name, const FText& Desc,
    const FName& GetterFnName, const FName& SetterFnName,
    TRange<float> DisplayRange, TRange<float> OutRange,
    float StepSize, float DefaultValue,
    ECommonNumericType NumericType, bool bApplyImmediately) const
{
    auto* Entry = NewObject<UListDataObject_Scalar>();
    Entry->SetDataId(Id);
    Entry->SetDataDisplayName(Name);
    Entry->SetDescriptionRichText(Desc);
    Entry->SetDisplayValueRange(DisplayRange);
    Entry->SetOutValueRange(OutRange);
    Entry->SetSliderStepSize(StepSize);
    Entry->SetDefaultValueFromString(LexToString(DefaultValue));
    Entry->SetCommonNumericType(NumericType);
    Entry->SetCommonNumberFormattingOptions(UListDataObject_Scalar::NoDecimal());
    Entry->SetDataDynamicGetter(MakeShared<FOptionsDataInteractionHelper>(GetterFnName));
    Entry->SetDataDynamicSetter(MakeShared<FOptionsDataInteractionHelper>(SetterFnName));
    Entry->SetShouldApplyChangeImmediately(bApplyImmediately);
    return Entry;
}

// ---- String 选项 ----
UListDataObject_String* UOptionsDataRegistry::CreateStringEntry(
    const FName& Id, const FText& Name, const FText& Desc,
    const FName& GetterFnName, const FName& SetterFnName,
    const TMap<FString, FText>& Options, const FString& DefaultValue,
    bool bApplyImmediately) const
{
    auto* Entry = NewObject<UListDataObject_String>();
    Entry->SetDataId(Id);
    Entry->SetDataDisplayName(Name);
    Entry->SetDescriptionRichText(Desc);
    for (const auto& Pair : Options)
    {
        Entry->AddDynamicString(Pair.Key, Pair.Value);
    }
    Entry->SetDefaultValueFromString(DefaultValue);
    Entry->SetDataDynamicGetter(MakeShared<FOptionsDataInteractionHelper>(GetterFnName));
    Entry->SetDataDynamicSetter(MakeShared<FOptionsDataInteractionHelper>(SetterFnName));
    Entry->SetShouldApplyChangeImmediately(bApplyImmediately);
    return Entry;
}
```

### Init 方法从 138 行 → ~35 行

```cpp
void UOptionsDataRegistry::InitGameplayCollectionTab()
{
    auto* Tab = CreateCollection("Tab-Gameplay", TXT_OPT_TAB_GAMEPLAY());

    // HUD
    {
        auto* Cat = CreateCollection("Category-HUD", TXT_OPT_CAT_HUD());
        Cat->AddChildListData(CreateBoolEntry("Entry-HealthBar",
            TXT_OPT_HEALTH_BAR(), TXT_OPT_HEALTH_BAR_DESC(),
            GET_FN(GetShowHealthBar), GET_FN(SetShowHealthBar), true));
        Cat->AddChildListData(CreateBoolEntry("Entry-DamageNumbers",
            TXT_OPT_DAMAGE_NUMBERS(), TXT_OPT_DAMAGE_NUMBERS_DESC(),
            GET_FN(GetShowDamageNumbers), GET_FN(SetShowDamageNumbers), true));
        Cat->AddChildListData(CreateBoolEntry("Entry-HighlightInteraction",
            TXT_OPT_HIGHLIGHT_INTERACTION(), TXT_OPT_HIGHLIGHT_INTERACTION_DESC(),
            GET_FN(GetHighlightInteraction), GET_FN(SetHighlightInteraction), true));
        Cat->AddChildListData(CreateBoolEntry("Entry-Subtitles",
            TXT_OPT_SUBTITLES(), TXT_OPT_SUBTITLES_DESC(),
            GET_FN(GetShowSubtitles), GET_FN(SetShowSubtitles), true));
        Tab->AddChildListData(Cat);
    }

    // Game
    {
        auto* Cat = CreateCollection("Category-Game", TXT_OPT_CAT_GAME());
        Cat->AddChildListData(CreateStringEntry("Entry-Difficulty",
            TXT_OPT_DIFFICULTY(), TXT_OPT_DIFFICULTY_DESC(),
            GET_FN(GetDifficulty), GET_FN(SetDifficulty),
            {{TEXT("Easy"), TXT_VAL_DIFFICULTY_EASY()},
             {TEXT("Normal"), TXT_VAL_DIFFICULTY_NORMAL()},
             {TEXT("Hard"), TXT_VAL_DIFFICULTY_HARD()},
             {TEXT("Nightmare"), TXT_VAL_DIFFICULTY_NIGHTMARE()}},
            TEXT("Normal")));
        Cat->AddChildListData(CreateBoolEntry("Entry-AutoSave",
            TXT_OPT_AUTO_SAVE(), TXT_OPT_AUTO_SAVE_DESC(),
            GET_FN(GetAutoSave), GET_FN(SetAutoSave), true));
        // ...
        Tab->AddChildListData(Cat);
    }

    RegisteredOptionsCollectionTabs.Add(Tab);
}
```

### 宏辅助

```cpp
#define GET_FN(FuncName) GET_FUNCTION_NAME_STRING_CHECKED(UFrontendGameUserSettings, FuncName)
```

### 优缺点

| 优点 | 缺点 |
|------|------|
| 立即实施，无需重构架构 | 仍然需要 C++ 代码新增选项 |
| 工厂方法复用到极致 | 策划无法直接编辑 |
| 类型安全，编译期检查 | 复杂逻辑（EditCondition 闭包）仍需单独处理 |
| 改动最小，风险最低 | 没有从根本上解决问题 |

---

## 三种方案对比

| 维度 | 方案 A (DataAsset) | 方案 B (DataTable) | 方案 C (工厂方法) |
|------|-------------------|--------------------|--------------------|
| **重构成本** | 高（需新建 DataAsset 类型体系） | 中（需新建 Row 结构体） | 低（仅提取工厂方法） |
| **新增选项** | 编辑器拖拽，零 C++ | 改 CSV/Excel，零 C++ | 加一行 C++ 调用 |
| **策划友好度** | ★★★★ 蓝图编辑 | ★★★★★ CSV/Excel | ★★ C++ 编辑 |
| **编译期安全** | ★★★★ 枚举绑定 | ★★ 裸字符串函数名 | ★★★★★ 宏+反射 |
| **复杂逻辑** | ★★★★ 可扩展 | ★★ 难以表达 | ★★★★★ 任意 C++ |
| **C++ 代码量** | ~200 行 | ~150 行 | ~400 行 |
| **迁移风险** | 高 | 中 | 低 |
| **适用场景** | 长期维护的大项目 | 100+ 简单选项 | 中小型项目 |
| **当前项目适配** | 长期最佳 | 选项极多时考虑 | 立即可做 |

---

## 建议路径

```
当前：959 行单体类
  │
  ├─→ 第一步：实施方案 C（工厂方法）── 降到 ~400 行
  │    收益：立即可做，不改架构，行数减半
  │
  └─→ 第二步：项目稳定后，逐步迁移到方案 A（DataAsset）
       收益：策划自助，零代码新增选项
```

如果项目未来选项数量超过 50+ 且结构相对简单（无复杂 EditCondition），可以考虑方案 B 作为中间状态。
