UniversalMenu — 项目网页映射

启动
  在项目目录运行：python WebPreview/serve.py
  浏览器打开：http://127.0.0.1:8765/
  或：powershell -File WebPreview/start-preview.ps1
  更换端口：python WebPreview/serve.py --port 9000
  停止：在服务终端按 Ctrl+C。

使用
  左上角“预览导航”或 F1：选择全部页面、查看复刻说明、进入配置优化评审。
  Q / E 切换页签；方向键选择/修改条目；Enter 确认；Backspace 返回。
  Esc 关闭弹窗/取消重绑定，网页额外支持 Esc 返回。
  设置保存在本浏览器，不写入 Unreal 的 GameUserSettings；房间密码只存内存。
  控制绑定支持键盘/鼠标捕获，手柄通过浏览器 Gamepad API 捕获。

覆盖范围
  7 个注册菜单屏幕：MainMenu、PressAnyKey、Options、Multiplayer、Credits、Confirm、KeyRemap。
  Options：Gameplay、Audio、Video、Control；34 个设置与 14 个输入动作。
  Multiplayer：6 个创建游戏字段、加入游戏、搜索中、空结果，以及独立 SessionRow 样例。
  额外覆盖独立 LoadingScreen 和项目实际配置的 UELogoAnim 启动视频。
  确认弹窗：Yes/No、OK、OK/Cancel、按键冲突；重绑定：键鼠与手柄两种提示。
  当前项目没有注册独立 Pause、Lobby 或 LoadGame 页面，因此未凭空添加。

数据与视觉来源
  data.js / reference/content.json：从项目 .uasset 提取的 227 条 StringTable 文本、条目配置、Credits。
  assets：项目原始 OTF 字体、86 张源像素解码 PNG、原样复制的启动视频。
  asset-map.js / assets-manifest.json：实际 CData 的键位图标映射与字体样式。
  reference/WBP_*-properties.json：蓝图 WidgetTree 属性和布局依据。
  主菜单、Options、Multiplayer 的几何布局用现有打包版画面核对。
  打包版落后于当前资产：网页采用当前资产的 JoinGame 标签、Advanced/LAN 和动态操作按钮。
  项目未提供有效中文本地化译文，因此菜单保留真实英文；说明工具与评审页使用中文。

边界
  这是原菜单的网页重建，不是将 Unreal 运行时嵌入网页；不包含 3D 游戏地图。
  Steam/LAN 房间、存档和关卡加载只演示流程，不连接服务、不读写真实存档。
  JoinGame 当前引用 DA_Empty；网页初始保持空列表。SessionRow 样例必须显式选择且有标记。
  浏览器字体光栅化与 UE、加载材质动画及鼠标光晕有差异；未逐像素验证所有页面和状态。
  显示模式和渲染设置只改变预览数据，不切换操作系统分辨率。
  分辨率候选值为浏览器演示列表；整体画质更新整数质量项，未实现引擎自动分辨率缩放算法。
  启动视频默认静音，满足浏览器自动播放限制。
  后续已修复评审 01 的 UE C++ 初始化顺序，并通过 Rider 构建、UE 两项自动化测试和实际界面验证。
  蓝图与 DataAsset 未修改；其余优化项仍为建议，详见 review.html。

验证
  verify.cjs：Playwright + Edge 无界面浏览器验收；需要开发环境已有 Playwright。
  PLAYWRIGHT_MODULE 环境变量可指定 Playwright 包路径。
  运行：node WebPreview/verify.cjs
  reference/verification.json：49 项通过，包含路由、设置持久化、依赖、重置、重绑定、搜索、视频及评审交互。
  reference/web-*.png：浏览器测试截图。

评审 01 验证记录
  reference/fix-01-verification.json：UE 自动化测试结果及首次打开 Options 显示已有 Hard 难度的实际核对。
