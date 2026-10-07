# 统一菜单 DataAsset 配置指南

基于 UnifiedMenuConfig_Design.md 的统一方案，Options 和 Multiplayer 各自创建一个 `UMenuConfigData` DataAsset，使用相同的类但填入不同的数据。

---

## Entry 编辑面板与内联类型配置

`UMenuEntryConfig` 的属性按以下用途分组。类型参数现在保存在 `Value Config` 内联对象中：选择 Bool、Scalar、String、Integer、Culture、Resolution 或 Editable 后，展开对象编辑该类型的参数，无需创建额外的 DataAsset。

| 分组 | 内容 |
|------|------|
| Entry | DataId、DisplayName、Description |
| Value | Value Config：直接选择配置类型，展开编辑默认值、范围、选项或输入规则 |
| Binding | BindingKey；Apply Immediately 收入高级属性 |
| Conditions | Packaged Build Only、Edit Dependency Ids、Disable Rich Reason |
| Display | Dynamic Details，收入高级属性 |

展开对应分组的高级属性可编辑 `Apply Immediately` 和 `Dynamic Details`。描述、禁用原因和补充详情支持多行编辑。

- `DataId` 应在同一个 Tab 内唯一；当前 `EditDependencyIds` 也只查找同一个 Tab 内的条目。
- Options 的依赖 ID 用于变化通知，具体可编辑条件另行处理；Multiplayer 还要求依赖中的所有 Bool 条目为 true。
- Options 的 `Apply Immediately` 默认为 false：返回时统一应用并保存。设为 true 时，用户修改或恢复默认值会立即应用该条目，仍在返回时保存；不会顺带提交其他草稿，依赖刷新也不会触发提前提交。Multiplayer 表单不使用此开关应用游戏设置。
- Scalar 的 Min/Max 是滑块显示范围，OutMin/OutMax 是设置值范围，DefaultValue 使用设置值范围。
- Culture 和 Resolution 的 `Default Value` 分别用于语言代码和分辨率字符串；可选项由运行时代码生成。

已有资产加载时会自动从旧字段生成内联配置，不需要手动重新填写，也不会批量改写磁盘资产；正常编辑并保存后会保存内联对象。复制 Entry 时会复制它自己的配置，修改副本不会影响原资产。

原有蓝图读取字段保留，编辑和保存内联配置时会同步对应旧字段。新代码应读取 `ValueConfig`，并通过 `GetEffectiveValueType()` 获取类型。尚未使用内联配置的旧 C++ 创建方式仍可运行；设置内联配置后，以内联对象为准。

后文表格保留旧配置字段名便于对照：Bool 的 `bDefaultBool` 对应内联 `Default Value`；String/Culture/Resolution 的 `DefaultStringValue`、Integer 的 `DefaultIntValue` 都对应各自的 `Default Value`；Integer 的 `AvailableIntegerOptions` 对应 `Available Options`。Scalar 和 Editable 的参数名称保持原样。

例如帧率条目选择 `String (Choice)`，BindingKey 仍为 `FrameRateLimit`。选项 Key 可以写 `60`、`60.0` 或 `60.000000`，按数值匹配；`0` 对应无限制。

本阶段完成类型配置拆分与旧资产兼容；依赖规则仍保持原行为，后续再统一条件系统。

---

## 1. Options — DA_OptionsConfig

### Tab-Gameplay（游戏）

#### Category-HUD（抬头显示）

| EntryId | ValueType | BindingKey | 额外字段 |
|---------|-----------|------------|---------|
| Entry-HealthBar | Bool | ShowHealthBar | bDefaultBool=true |
| Entry-DamageNumbers | Bool | ShowDamageNumbers | bDefaultBool=true |
| Entry-HighlightInteraction | Bool | HighlightInteraction | bDefaultBool=true |
| Entry-Subtitles | Bool | ShowSubtitles | bDefaultBool=true |

#### Category-Game（游戏）

| EntryId | ValueType | BindingKey | 额外字段 |
|---------|-----------|------------|---------|
| Entry-Difficulty | String | Difficulty | AvailableOptions: Easy/Normal/Hard/Nightmare, DefaultStringValue="Normal" |
| Entry-AutoSave | Bool | AutoSave | bDefaultBool=true |
| Entry-Language | Culture | CurrentCulture | DefaultStringValue="en" |

---

### Tab-Audio（音频）

#### Category-Volume（音量）

所有 Scalar 共用参数：Min=0, Max=1, OutMin=0, OutMax=2, Step=0.01, Default="1.0", NumericType=Percentage

| EntryId | ValueType | BindingKey |
|---------|-----------|------------|
| OverallVolume | Scalar | OverallVolume |
| MusicVolume | Scalar | MusicVolume |
| SFXVolume | Scalar | SFXVolume |
| VoiceVolume | Scalar | VoiceVolume |
| AmbientVolume | Scalar | AmbientVolume |
| UIVolume | Scalar | UIVolume |

#### Category-Sound（音效）

| EntryId | ValueType | BindingKey | bDefaultBool |
|---------|-----------|------------|-------------|
| Background-Sound | Bool | AllowBackgroundSound | false |
| Sound-Subtitles | Bool | SubtitlesEnabled | true |
| Sound-HRTF | Bool | HRTFEnabled | true |
| Sound-Mono | Bool | MonoOutput | false |
| Sound-DRC | Bool | DynamicRangeCompression | false |
| Sound-DialogueBoost | Bool | DialogueBoost | true |

---

### Tab-Video（视频）

#### Tab-Display（显示）

| EntryId | ValueType | BindingKey | 额外字段 |
|---------|-----------|------------|---------|
| WindowMode | Integer | FullscreenMode | 由 Registry 特殊处理创建 StringEnum，DataAsset 中 ValueType 可设为任意值（推荐 Integer），选项由代码注入 |
| Resolution | Resolution | Resolution | bPackagedBuildOnly=true |

#### GraphicsCategoryCollection（画质）

Integer 质量选项统一用：0→低/1→中/2→高/3→史诗/4→电影, DefaultIntValue=3

| EntryId | ValueType | BindingKey | 额外字段 |
|---------|-----------|------------|---------|
| DisplayGamma | Scalar | DisplayGamma | Min=0, Max=1, OutMin=1.7, OutMax=2.7, Default="2.2" |
| OverallQuality | Integer | OverallScalabilityLevel | 质量选项 |
| ResolutionScale | Scalar | ResolutionScaleNormalized | Min=0, Max=1, OutMin=0.5, OutMax=1, Default="1.0", EditDependencyIds=["OverallQuality"] |
| GlobalIlluminationQuality | Integer | GlobalIlluminationQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| ShadowQuality | Integer | ShadowQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| AntiAliasingQuality | Integer | AntiAliasingQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| ViewDistanceQuality | Integer | ViewDistanceQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| TextureQuality | Integer | TextureQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| VisualEffectsQuality | Integer | VisualEffectQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| ReflectionQuality | Integer | ReflectionQuality | 质量选项, EditDependencyIds=["OverallQuality"] |
| PostProcessingQuality | Integer | PostProcessingQuality | 质量选项, EditDependencyIds=["OverallQuality"] |

#### AdvancedGraphicsCategoryCollection（高级画质）

| EntryId | ValueType | BindingKey | 额外字段 |
|---------|-----------|------------|---------|
| Entry-VSync | Bool | VSync | bDefaultBool=true |
| Entry-FrameRateLimit | String | FrameRateLimit | AvailableOptions: 0→无限制/30→30/60→60/120→120/144→144/240→240, DefaultStringValue="0.0" |

> Tab-Control（按键绑定）由 Registry 运行时动态生成，不在 DataAsset 中配置。

---

## 2. Multiplayer — DA_MultiplayerConfig

Multiplayer 条目均为纯 UI 数据，无需 BindingKey。

### CreateGame（创建游戏）

#### Cat-Basic（基本设置）

| EntryId | ValueType | 额外字段 |
|---------|-----------|---------|
| GameName | Editable | HintText=请输入房间名 |
| HasPassword | Bool | bDefaultBool=false |
| Password | Editable | bIsPassword=true, MaxCharCount=6, CharFilter=NumbersOnly, HintText=请输入密码, EditDependencyIds=["HasPassword"] |

#### Cat-Advanced（高级设置）

| EntryId | ValueType | 额外字段 |
|---------|-----------|---------|
| MaxPlayers | Integer | AvailableIntegerOptions: 2→2/4→4/8→8/16→16 |
| Map | String | AvailableOptions: Map_01→Map 01/Map_02→Map 02/Map_03→Map 03 |
| GameMode | String | AvailableOptions: Deathmatch/TeamDeathmatch/CaptureTheFlag |
| ServerRegion | String | AvailableOptions: Asia/Europe/NorthAmerica |

### JoinGame（加入游戏）

#### Cat-Join

| EntryId | ValueType | 说明 |
|---------|-----------|------|
| ServerList | Editable | 占位入口，UI 层识别 DataId="ServerList" 处理服务器列表逻辑 |

---

## 3. ValueType 说明

| ValueType | 创建的运行时类型 | 适用场景 |
|-----------|-----------------|---------|
| **Bool** | UListDataObject_StringBool | 开关选项 |
| **Scalar** | UListDataObject_Scalar | 数值滑块 |
| **String** | UListDataObject_String | 字符串下拉选择 |
| **Integer** | UListDataObject_StringInteger | 整数下拉选择（画质预设、人数等） |
| **Culture** | UListDataObject_StringCulture | 语言/区域选择（自动填充选项） |
| **Resolution** | UListDataObject_StringResolution | 分辨率选择（自动填充选项） |
| **Editable** | UListDataObject_Editable | 自由文本输入 |

## 4. BindingKey 速查

| BindingKey | ValueType | 说明 |
|------------|-----------|------|
| Difficulty | String | 游戏难度 |
| ShowHealthBar | Bool | 显示血条 |
| ShowDamageNumbers | Bool | 显示伤害数字 |
| HighlightInteraction | Bool | 高亮可交互对象 |
| ShowSubtitles | Bool | 显示字幕 |
| AutoSave | Bool | 自动保存 |
| AutoPickup | String | 自动拾取：Off / MaterialsOnly / AllItems，默认 Off |
| SprintMode | String | 冲刺方式：Hold / Toggle，默认 Hold |
| ShowMinimap | Bool | 显示小地图，默认 true |
| ShowAmmoCount | Bool | 显示弹药数量，默认 true |
| CrosshairSize | Scalar | 准星大小倍率；Min/OutMin=0.5，Max/OutMax=2，Step=0.05，Default="1.0"，NumericType=Percentage，显示 50%～200% |
| CrouchMode | String | 蹲伏方式：Hold / Toggle，默认 Hold |
| CurrentCulture | Culture | 语言 |
| OverallVolume | Scalar | 总音量 |
| MusicVolume | Scalar | 音乐音量 |
| SFXVolume | Scalar | 音效音量 |
| VoiceVolume | Scalar | 语音音量 |
| AmbientVolume | Scalar | 环境音量 |
| UIVolume | Scalar | UI 音量 |
| AllowBackgroundSound | Bool | 后台播放 |
| SubtitlesEnabled | Bool | 字幕开关 |
| HRTFEnabled | Bool | 3D 空间音频 |
| MonoOutput | Bool | 单声道输出 |
| DynamicRangeCompression | Bool | 动态范围压缩 |
| DialogueBoost | Bool | 对话增强 |
| FullscreenMode | Integer | 窗口模式（Registry 特殊处理，创建 StringEnum） |
| Resolution | Resolution | 分辨率 |
| DisplayGamma | Scalar | 显示伽马 |
| OverallScalabilityLevel | Integer | 整体画质 |
| ResolutionScaleNormalized | Scalar | 分辨率缩放 |
| VSync | Bool | 垂直同步 |
| FrameRateLimit | String | 帧率限制 |
| GlobalIlluminationQuality | Integer | 全局光照质量 |
| ShadowQuality | Integer | 阴影质量 |
| AntiAliasingQuality | Integer | 抗锯齿质量 |
| ViewDistanceQuality | Integer | 视距质量 |
| TextureQuality | Integer | 纹理质量 |
| VisualEffectQuality | Integer | 特效质量 |
| ReflectionQuality | Integer | 反射质量 |
| PostProcessingQuality | Integer | 后处理质量 |

BindingKey 写错不会崩溃，运行时输出 `ensureMsgf` 警告，该条目退化为无持久化的纯 UI 数据。
