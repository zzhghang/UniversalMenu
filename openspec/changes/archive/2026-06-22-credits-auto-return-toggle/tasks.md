## 1. 头文件修改

- [x] 1.1 新增 `bAutoReturnOnFinish` 属性（`bool`，EditDefaultsOnly，默认 `true`）

## 2. 实现修改

- [x] 2.1 `OnCreditsFinished` 中添加 `bAutoReturnOnFinish` 条件判断，仅当为 `true` 时调用 `DeactivateWidget()`

## 3. 验证

- [x] 3.1 编译通过
- [ ] 3.2 编辑器中测试：开关为 true 时自动返回，为 false 时停留
