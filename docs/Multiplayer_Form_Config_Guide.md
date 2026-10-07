# Multiplayer 表单配置与保存

MultiplayerScreen 使用 MenuConfig → Tab → Category → Entry → Value Config。现有资产及 SaveSessionData、LoadSessionData、GetCachedSessionData 蓝图入口继续可用。普通 Multiplayer 条目无需 BindingKey。

## 保存

- 返回按钮现在调用 SaveSessionData；蓝图已有的主动保存调用仍有效。
- Persistent（默认）：值保存至 MenuSession 存档，兼容原有 DataId 键。
- SessionOnly：仅保留在当前 GameInstance，蓝图仍可从返回的 MenuSessionData 读取；不会写入磁盘。
- None：不进入 MenuSessionData，再打开界面恢复初始化值。
- SaveKey 为空时使用 DataId。显式 SaveKey 可用于稳定存档标识；未找到新键时，在旧 DataId 无歧义的情况下读取旧键，下次保存迁移到新键。
- DataId 必须在所属 Tab 内唯一；有效 SaveKey 必须在整个表单内唯一。配置冲突会记录日志并阻止保存，避免覆盖其他字段。
- 空字符串是有效保存值。删除已填写内容并保存后，下次打开仍为空。
- 文本、Bool、String、Integer、Scalar 共用 TryGetValueAsString/TrySetValueFromString。保存使用选项 Key，Scalar 保存 Out 范围的底层值。
- 恢复时已删除的选项、非法数值/文本回退到初始化值；Scalar 越界值限制到 Out 范围。
- bLastSessionSaveSucceeded 和 OnSessionSaveCompleted(bool) 提供保存结果；GameInstance 新增 TrySaveMenuSessionData。旧的 void SaveMenuSessionData 入口保留。
- 缓存首次访问时从磁盘加载；重复打开界面不会再次读盘覆盖本次运行的数据。

## 依赖

Entry → Conditions → Multiplayer：

- Edit Conditions：条件数组；非空时替换旧 EditDependencyIds 的 Bool 门控规则。
- Source Data Id：来源条目。
- Source Tab Id：留空表示同 Tab，也可指定其他 Tab。
- Comparison：Equals、NotEquals、Greater、GreaterOrEqual、Less、LessOrEqual、IsTrue、IsFalse、IsEmpty、IsNotEmpty。
- Value：选项存储 Key 或数字，不使用翻译后的显示文本。
- Condition Match：All 全部满足 / Any 任意满足。
- Disable Rich Reason：条件不满足时显示的原因。

示例：Password 的 SourceDataId=HasPassword、Comparison=IsTrue；ServerRegion 的 SourceDataId=IsLAN、Comparison=IsFalse；队伍人数以 GameMode Equals TeamDeathmatch 控制可编辑状态。当前规则控制可编辑性，不控制行的隐藏。

旧 EditDependencyIds 继续支持“所有引用的 Bool 为 true”；非 Bool 引用保留刷新用途。缺失引用被报告且不会默认为可编辑。依赖刷新不再级联广播值修改，因此相互引用不会造成刷新递归。

## 房间列表

- Category 的 Session List 可明确指定搜索结果容器；旧 ServerList 占位条目仍可识别，运行时不再生成一个可编辑、可存档的占位输入框。
- 有表头的平铺布局和普通分类布局都能刷新结果；刷新只替换指定容器的 Session 条目，保留表头和其他静态条目。
- 搜索为空时移除旧结果；界面刷新按真实 Session ID 尝试恢复选中项。
- 总配置的 Create Game Tab Id / Join Game Tab Id 默认仍为 CreateGame / JoinGame。
- 总配置的 Preview Sessions In Editor 保留当前测试房间预览，默认开启；可关闭，打包游戏始终不生成测试房间。

修改保存策略或 SaveKey 后，请同时检查业务蓝图使用的读取键。新规则不要求重新制作原有 DataAsset。
