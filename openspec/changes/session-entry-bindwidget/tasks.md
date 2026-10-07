## 1. C++ — W_ListEntry_Session.h

- [x] 1.1 删除 `SessionTextStyle`、`ColumnGap`、`EdgePadding`、`VerticalPadding` 四个属性
- [x] 1.2 新增 5 个 BindWidget CommonTextBlock: `Tb_ServerName`、`Tb_MapName`、`Tb_Players`、`Tb_Private`、`Tb_Ping`
- [x] 1.3 删除 `UHorizontalBox` forward declaration（如 `Hb_SessionColumns` 保留则保留）

## 2. C++ — W_ListEntry_Session.cpp

- [x] 2.1 重写 `OnOwningListDataObjectSet`：删除 ClearChildren/NewObject/循环，改为 5 行 SetText 直接赋值
- [x] 2.2 删除不再需要的 include：`Components/HorizontalBoxSlot.h`

## 3. Blueprint — WBP_ListEntry_Session（编辑器中操作）

- [ ] 3.1 重建布局：放置 `Hb_SessionColumns` (UHorizontalBox)，内部放 5 个 UCommonTextBlock
- [ ] 3.2 分别命名 5 个 TextBlock 为 `Tb_ServerName`、`Tb_MapName`、`Tb_Players`、`Tb_Private`、`Tb_Ping`
- [ ] 3.3 调整每个 TextBlock 的 Slot Padding、Text Style 与 HeaderRow 列对齐
