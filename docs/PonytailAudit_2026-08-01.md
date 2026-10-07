# Ponytail 审计报告 — GameMainMenu 插件

**日期**: 2026-08-01
**范围**: `Plugins/GameMainMenu/Source/` 全部源码
**类型**: 过度工程化审查（不含正确性/安全/性能问题）

---

## 审计结果

### 1. yagni: `FOptionsDataInteractionHelper` 多余包装类

**位置**: `Public/Widgets/Options/OptionsDataInteractionHelper.h`
**问题**: 一个独立类只包装两个 `TFunction` 指针（getter/setter），提供三个方法：构造、`GetValueAsString()`、`SetValueFromString()`。功能等同于直接存两个 `TFunction` 成员。
**建议**: 将 getter/setter 的 `TFunction` 直接放到 `UListDataObject_Value` 的成员中，删除此类和头文件。
**节省**: ~30 行（1 个 .h 文件）

### 2. yagni: `UConfirmScreenInfoObject` — UObject 包装简单数据

**位置**: `Public/Widgets/W_ConfirmScreen.h:33-54`
**问题**: `UConfirmScreenInfoObject` 继承 `UObject`，仅承载三个字段（Title, Message, AvailableScreenButtons）和三个静态工厂方法。传入 `UW_ConfirmScreen::InitConfirmScreen` 后立即被读取丢弃。GC 跟踪的 UObject 用于纯数据传递——用 struct 或直接传参即可。
**建议**: 删除此类，将参数直接传给 `InitConfirmScreen`，或改用已有的 `FConfirmScreenButtonInfo` struct。
**节省**: ~25 行（UObject 分配 + 工厂方法）

### 3. shrink: `CreateEntryFromConfig` 中的重复分支代码

**位置**: `Private/Widgets/MenuConfig/MenuConfigData.cpp:73-104` 和 `:123-153`
**问题**: `FullscreenMode` 绑定键的全屏模式选项、`IsQualityBindingKey` 的质量等级选项在 `String` 和 `Integer` 两个 switch case 中各写了一遍，逻辑完全相同。
**建议**: 提取为两个 static helper 函数或 lambda，在两端共用。
**节省**: ~50 行

### 4. shrink: `FrontendDebugHelper.h` 便捷包装

**位置**: `Public/FrontendDebugHelper.h`
**问题**: `PrintFloat`、`PrintInt`、`PrintBool`、`PrintObject`、`PrintIf` 都是对 `GEngine->AddOnScreenDebugMessage` + `UE_LOG` 的一行封装。`PrintInt` 和 `PrintFloat` 可以用 `Print(FString::Printf(...))` 替代，无需各自一个函数。
**建议**: 只保留 `Print()` 和 `PrintIf()` 两个核心函数，调用处用 `FString::Printf` 格式化。其他 4 个辅助函数删除。
**节省**: ~25 行

### 5. delete: `MaxAllowedResolution` 死代码

**位置**: `Public/Widgets/Options/DataObjects/ListDataObject_StringResolution.h:21,36`
**问题**: `GetMaxAllowedResolution()` 返回硬编码字符串 `"MaxAllowedResolution"`，成员变量 `MaxAllowedResolution` 从未赋值或使用。分辨率边界检查实际在 `InitResolutionValues()` 中通过 `UGameUserSettings::GetScreenResolutionList` 完成。
**建议**: 删除该函数和成员变量。
**节省**: 2 行

### 6. delete: `Frontend_Image_Test` 测试 Tag

**位置**: `Public/FrontendGameplayTags.h:34` / `Private/FrontendGameplayTags.cpp:21`
**问题**: 生产代码中声明了测试用途的 GameplayTag `Frontend.Image.Test`。如果在 DataAsset 中无引用，应移除。
**建议**: 确认无引用后删除。
**节省**: 2 行

---

## Ponytail 债务清单（已标记的捷径）

| 文件 | 注释 | 说明 |
|------|------|------|
| `GameMainMenu.cpp:11` | `ponytail: pre-load StringTables` | 启动时预加载 StringTable，首次加载慢于懒加载 |
| `MenuConfigData.cpp:79` | `ponytail: window mode options hardcoded` | 全屏/窗口/无边框 3 种模式硬编码，如需更多模式可配置化 |
| `MenuConfigData.cpp:143` | `ponytail: quality levels hardcoded` | Low/Med/High/Epic/Cinematic 5 档硬编码，如需自定义档位可配置化 |
| `W_ListEntry_Editable.cpp:38` | `ponytail: defer to next tick` | 用下帧延迟避免 Slate 状态损坏，如 Slate 修复后可直接 SetText |

---

## 未发现问题

- **类层次深度**: `ListDataObject_Base → _Value → _String → _StringEnum/Bool/Integer/Culture/Resolution` 是 UE 反射系统下合理的继承层级，每个子类需要有独立的 UCLASS 以支持 DataAsset 驱动创建。
- **DataAsset 层级**: `MenuConfigData → TabConfig → CategoryConfig → EntryConfig` 4 层组合，每层有独立的设计师可编辑字段，符合 UE DataAsset 惯用模式。
- **Component 薄包装**: `FrontendCommonListView`、`FrontendCommonRotator`、`FrontendCommonTabListWidgetBase` 各自在引擎类型上加了一个关键方法，包装合理。
- **`W_ActivatableBase`**: 仅缓存 PC 指针，但所有 UI widget 都继承它，属于有效共享。
- **`LocalizationKeys.h` 宏**: UE 本地化 `LOCTABLE` 宏是标准模式，非过度工程。

---

## 汇总

| 分类 | 数量 | 预计节省 |
|------|------|----------|
| yagni (无用抽象) | 2 | ~55 行, 1 文件 |
| shrink (可精简) | 2 | ~75 行 |
| delete (死代码) | 2 | ~4 行 |
| **净节省** | | **~134 行, 1 个 .h 文件** |

**结论**: 整体架构简洁，过度工程化程度低。主要问题集中在两个不必要的包装类和一段重复代码。按优先级：修复 #1 和 #3 收益最大。
