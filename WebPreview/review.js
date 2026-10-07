'use strict';

const descriptions = {
  screen: ['Screen Config：保留界面组合入口', '当前 Options 与 Multiplayer 都通过 UMenuConfigData 的 Tabs 数组引用 Tab 资产。这个入口清晰，可以保留。'],
  tab: ['Tab：天然的编辑与复用单元', 'UMenuTabConfig 包含 TabId、TabName、Categories。当前 Options 有 Gameplay、Audio、Video 三个配置 Tab；Control 由 Registry 动态追加。Multiplayer 配置 CreateGame 与 JoinGame。'],
  category: ['Category：目前仍是独立资产', 'UMenuCategoryConfig 包含类别 ID、标题、Entry 引用和表头布局。修改一个页面需要跨多个资产跳转。若类别没有独立复用需求，可以内嵌到 Tab 中。'],
  entry: ['Entry：统一类型，但需要明确生效字段', 'UMenuEntryConfig 以 ValueType 切换 Bool、Scalar、String、Integer、Editable 等字段。EditConditionHides 能改善编辑器可读性，但不会自动清除旧类型数据，也不会验证工厂是否真正使用这些字段。'],
  'proposed-screen': ['Screen Config：只管组合与默认入口', '继续保存有序 Tabs 引用，并显式配置默认 Tab。资产应表达页面组合；Options 的设置绑定和 Multiplayer 的会话来源继续由各自运行时提供。'],
  'proposed-tab': ['Tab Asset：按页签拆分，保留独立协作空间', '一个 Tab 资产内包含完整类别和条目。先用简单 USTRUCT 数组迁移；并非要求把整个项目合并成一个巨大 DataAsset。独立 Tab 仍便于复用和多人编辑。'],
  'proposed-category': ['内嵌 Category：布局不改变数据树', '类别保存标题、稳定 ID、布局与 Entries 数组。SessionList 使用明确内容类型或容器 ID；显示表头时仍保留容器，避免布局选项改变动态数据插入位置。'],
  'proposed-entry': ['内嵌 Entry：值、顺序、规则都显式', 'Entries 使用数组排序；每项保存稳定 DataId、绑定、类型化配置和显式条件。跨 Tab 复用的选项集可以单独成资产。只有行为差异扩大时，才考虑 Instanced UObject / FInstancedStruct。']
};

function selectNode(key) {
  const description = descriptions[key];
  if (!description) return;
  document.querySelectorAll('[data-node]').forEach(button => {
    const selected = button.dataset.node === key;
    button.classList.toggle('selected', selected);
    button.setAttribute('aria-pressed', String(selected));
  });
  document.getElementById('node-title').textContent = description[0];
  document.getElementById('node-description').textContent = description[1];
}

document.querySelectorAll('[data-node]').forEach(button => {
  button.addEventListener('click', () => selectNode(button.dataset.node));
});

document.querySelectorAll('[data-view]').forEach(button => {
  button.addEventListener('click', () => {
    const proposed = button.dataset.view === 'proposed';
    document.getElementById('current-diagram').hidden = proposed;
    document.getElementById('proposed-diagram').hidden = !proposed;
    document.querySelectorAll('[data-view]').forEach(item => {
      item.setAttribute('aria-pressed', String(item === button));
    });
    selectNode(proposed ? 'proposed-tab' : 'screen');
  });
});

document.querySelectorAll('[data-filter]').forEach(button => {
  button.addEventListener('click', () => {
    const filter = button.dataset.filter;
    let count = 0;
    let fixedCount = 0;
    document.querySelectorAll('.finding-card').forEach(card => {
      card.hidden = filter !== 'all' && card.dataset.priority !== filter;
      if (!card.hidden) {
        count += 1;
        if (card.dataset.status === 'fixed') fixedCount += 1;
      }
    });
    document.querySelectorAll('[data-filter]').forEach(item => {
      item.setAttribute('aria-pressed', String(item === button));
    });
    document.getElementById('result-count').textContent = `显示 ${count} 项 · 已修复 ${fixedCount} 项`;
  });
});
