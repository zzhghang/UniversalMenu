'use strict';
(() => {
  const D = window.MENU_DATA;
  const $ = (selector, root = document) => root.querySelector(selector);
  const $$ = (selector, root = document) => [...root.querySelectorAll(selector)];
  const screen = $('#screen'), stage = $('#stage'), overlay = $('#overlay');
  const text = value => typeof value === 'string' ? value : (value?.en || '');
  const escapeHTML = value => String(value ?? '').replace(/[&<>"']/g, c => ({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  const tr = (table, key) => text(D.strings[table]?.[key]) || key;
  const common = key => tr('ST_UI_Common', key);
  const stripRich = value => text(value).replace(/<[^>]*>/g, '');
  const allOptions = D.options.tabs.flatMap(t => t.categories.flatMap(c => c.entries));
  const allMultiplayer = D.multiplayer.tabs.flatMap(t => t.categories.flatMap(c => c.entries));
  const optionById = new Map(allOptions.map(e => [e.id, e]));
  const multiplayerById = new Map(allMultiplayer.map(e => [e.id, e]));
  const defaults = Object.fromEntries(allOptions.map(e => [e.id, e.defaultValue]));
  // Window size comes from the environment in UE; the DA resolution is only a fallback.
  defaults.Resolution = `${innerWidth}x${innerHeight}`;
  let persisted = {};
  try { persisted = JSON.parse(localStorage.getItem('universal-menu-preview-v1') || '{}'); } catch {}
  const values = {...defaults};
  for (const e of allOptions) {
    const value = persisted.settings?.[e.id];
    if (value === undefined) continue;
    if (e.type === 'scalar' && typeof value === 'number' && Number.isFinite(value)) {
      values[e.id] = Math.max(e.outputMin, Math.min(e.outputMax, value));
    } else if (e.type === 'resolution' && typeof value === 'string' && /^\d{3,5}x\d{3,5}$/.test(value)) values[e.id] = value;
    else if ((e.id === 'OverallQuality' && value === -1) || e.options.some(o => o.value === value)) values[e.id] = value;
  }
  const room = Object.fromEntries(allMultiplayer.map(e => [e.id, e.defaultValue]));
  const bindings = Object.fromEntries(D.controls.map(e => [e.id, {keyboard:e.keyboard, gamepad:e.gamepad}]));
  for (const [id, pair] of Object.entries(persisted.bindings || {})) {
    if (!bindings[id] || !pair || typeof pair !== 'object') continue;
    for (const device of ['keyboard','gamepad']) {
      if (typeof pair[device] === 'string' && /^[A-Za-z0-9_]{1,50}$/.test(pair[device])) bindings[id][device] = pair[device];
    }
  }
  let page = 'main', tabIndex = 0, selection = '', modal = null, timers = [], creditsFrame = 0;
  let sampleSessions = false, searchState = 'initial', selectedSession = false;
  let lastFocus = null, currentEntries = [], routeSerial = 0, noticeTimer = 0;
  const sample = {name:'SessionRow preview',map:'Map1',players:'1/8',private:'No',ping:'24ms'};
  const iconFiles = {
    SpaceBar:'T_Keyboard_Space',LeftShift:'T_Keyboard_Shift_L',RightShift:'T_Keyboard_Shift_L',
    LeftControl:'T_Keyboard_Ctrl_L',RightControl:'T_Keyboard_Ctrl_R',LeftAlt:'T_Keyboard_Alt_L',RightAlt:'T_Keyboard_Alt_R',
    BackSpace:'T_Keyboard_BackSpace',Tab:'T_Keyboard_Tab',Enter:'T_Keyboard_Enter',Delete:'T_Keyboard_Delete',
    LeftMouseButton:'T_Mouse_Key_1_Left',RightMouseButton:'T_Mouse_Key_2_Right',
    Gamepad_FaceButton_Bottom:'T_Xbox_Button_A',Gamepad_FaceButton_Right:'T_Xbox_Button_B',
    Gamepad_FaceButton_Left:'T_Xbox_Button_X',Gamepad_FaceButton_Top:'T_Xbox_Button_Y',
    Gamepad_Special_Right:'T_Xbox_Button_Menu',Gamepad_Special_Left:'T_Xbox_Button_View',
    Gamepad_LeftThumbstick:'T_Xbox_Stick_L',Gamepad_RightThumbstick:'T_Xbox_Stick_R',
    Gamepad_DPad_Up:'T_Xbox_D-Pad_Up',Gamepad_DPad_Down:'T_Xbox_D-Pad_Round_Down',
    Gamepad_DPad_Left:'T_Xbox_D-Pad_Round_Left',Gamepad_DPad_Right:'T_Xbox_D-Pad_Round_Right'
  };
  const keyLabels = {SpaceBar:'SPACE',LeftShift:'LSHIFT',RightShift:'RSHIFT',BackSpace:'BACK',
    Gamepad_LeftTriggerAxis:'LT',Gamepad_RightTriggerAxis:'RT',Gamepad_LeftShoulder:'LB',Gamepad_RightShoulder:'RB',
    MiddleMouseButton:'MOUSE 3'};
  function keyImage(key, className = 'key-image') {
    const file = iconFiles[key] || (/^[A-Z]$/.test(key) ? `T_Keyboard_${key}` : null);
    const source = window.MENU_KEY_ICONS?.[key] || (file ? `assets/${file}.png` : null);
    return source ? `<img class="${className}" src="${source}" alt="${escapeHTML(keyLabels[key] || key)}">` : `<span class="key-chip">${escapeHTML(keyLabels[key] || key)}</span>`;
  }
  function persist() {
    try { localStorage.setItem('universal-menu-preview-v1', JSON.stringify({settings:values,bindings})); }
    catch { notice('浏览器未允许本地存储；当前设置会保留到页面关闭。'); }
  }
  function notice(message) {
    clearTimeout(noticeTimer); $('#notice').textContent = message; $('#notice').classList.add('visible');
    noticeTimer = setTimeout(() => $('#notice').classList.remove('visible'), 5000);
  }
  function later(fn, delay) { const id = setTimeout(fn, delay); timers.push(id); return id; }
  function resize() {
    const scale = Math.min(innerHeight / 1080, innerWidth / 1440);
    const width = Math.max(1440, innerWidth / scale);
    stage.style.setProperty('--scale', scale);
    stage.style.setProperty('--stage-width', `${width}px`);
    stage.style.setProperty('--stage-height', '1080px');
  }
  function navigate(route) {
    if (location.hash.slice(1) === route) renderRoute();
    else location.hash = route;
  }
  function clearPage() {
    timers.forEach(clearTimeout); timers = []; cancelAnimationFrame(creditsFrame);
    if(searchState === 'searching')searchState = 'initial';
    closeModal(false); currentEntries = []; selection = ''; routeSerial++;
    screen.classList.remove('modal-open'); screen.innerHTML = '';
  }
  function renderRoute() {
    clearPage();
    const route = location.hash.slice(1) || 'main';
    const parts = route.split('/');
    page = parts[0];
    $('#page-select').value = [...$('#page-select').options].some(o => o.value === route) ? route : 'main';
    if (page === 'main') renderMain();
    else if (page === 'options') {tabIndex = Math.max(0,['gameplay','audio','video','control'].indexOf(parts[1])); renderSettings();}
    else if (page === 'multiplayer') {tabIndex = parts[1] === 'join' ? 1 : 0; renderSettings();}
    else if (page === 'credits') renderCredits();
    else if (page === 'press-any-key') renderPressAnyKey();
    else if (page === 'startup') renderStartup();
    else if (page === 'loading') renderLoading(false);
    else if (page === 'remap') {
      page='options';tabIndex=3;renderSettings();openRemap(D.controls[0],'keyboard');
    } else if (page.startsWith('confirm')) {
      renderMain();
      if (page === 'confirm-ok') showConfirm(tr('ST_Multiplayer','Validation_Title'),tr('ST_Multiplayer','Validation_NumbersOnly'),[common('OK')]);
      else if (page === 'confirm-cancel') showConfirm(tr('ST_Multiplayer','Confirm_JoinGame_Title'),tr('ST_Multiplayer','Confirm_JoinGame_Message'),[common('OK'),common('Cancel')]);
      else showQuit();
    } else {page='main';renderMain();}
    document.title = `UniversalMenu · ${page === 'options' ? ['Gameplay','Audio','Video','Control'][tabIndex] : page}`;
  }
  function renderMain() {
    const items = [
      ['continue','Continue-desc',() => startLoading('Continue')],
      ['NewStory','NewStory-Desc',() => startLoading('New Story')],
      ['Multiplayer','Multiplayer-Desc',() => navigate('multiplayer/create')],
      ['Options','Options-Desc',() => navigate('options/gameplay')],
      ['Credits','Credits-Desc',() => navigate('credits')],
      ['Exit','Exit-Desc',showQuit]
    ];
    screen.innerHTML = '<nav class="main-links" aria-label="Main menu"></nav><div class="main-description"></div>';
    for (const [label,desc,action] of items) {
      const button = document.createElement('button');button.textContent = common(label);button.dataset.menu = label;
      button.addEventListener('click', action);
      for (const event of ['mouseenter','focus']) button.addEventListener(event, () => {$('.main-description').textContent=tr('ST_GameMessages',desc);});
      button.addEventListener('mouseleave', () => {$('.main-description').textContent='';});
      $('.main-links').append(button);
    }
  }
  function currentTab() {return page === 'options' ? D.options.tabs[tabIndex] : D.multiplayer.tabs[tabIndex];}
  function currentValue(entry) {return page === 'multiplayer' ? room[entry.id] : values[entry.id];}
  function isDisabled(entry) {
    if (entry.id === 'Password') return !room.HasPassword;
    if (entry.id === 'Resolution') return values.WindowMode === 'WindowedFullscreen';
    if (entry.id === 'Entry-VSync') return values.WindowMode !== 'Fullscreen';
    return false;
  }
  function renderSettings(keepScroll = false) {
    const scrollTop = keepScroll ? $('.list-panel')?.scrollTop || 0 : 0;
    const savedSelection = keepScroll ? selection : '';
    const active = document.activeElement;
    const focusedEntry = keepScroll ? active?.closest('[data-entry]')?.dataset.entry : null;
    const focusedControl = active?.dataset.device ? `[data-device="${active.dataset.device}"]` : active?.matches('.previous') ? '.previous' : active?.matches('.next') ? '.next' : active?.matches('input') ? 'input' : null;
    const tabs = page === 'options' ? [...D.options.tabs.map(t=>text(t.label)),'Control'] : D.multiplayer.tabs.map(t=>text(t.label));
    screen.innerHTML = `<nav class="tab-bar" aria-label="${page} tabs"><button class="tab-cycle" data-cycle="-1" aria-label="Previous tab">${keyImage('Q')}</button>${tabs.map((t,i)=>`<button role="tab" aria-selected="${i===tabIndex}" class="${i===tabIndex?'active':''}" data-tab="${i}">${escapeHTML(t)}</button>`).join('')}<button class="tab-cycle" data-cycle="1" aria-label="Next tab">${keyImage('E')}</button></nav><div class="settings-body"><div class="list-panel" role="list" tabindex="-1" aria-label="${escapeHTML(tabs[tabIndex])}"></div><aside class="detail-panel" aria-live="polite"><h1 class="detail-title"></h1><div class="detail-description"></div><div class="detail-warning"></div><div class="detail-extra"></div></aside></div><footer class="screen-footer"></footer>`;
    $$('[data-cycle]').forEach(b=>b.onclick=()=>changeTab(Number(b.dataset.cycle)));
    $$('[data-tab]').forEach(b=>b.onclick=()=>setTab(Number(b.dataset.tab)));
    currentEntries = [];
    if (page === 'options' && tabIndex === 3) renderControls();
    else if (page === 'multiplayer' && tabIndex === 1) renderSessions();
    else for (const category of currentTab().categories) {
      if(text(category.label)) addCategory(text(category.label));
      for (const entry of category.entries) {currentEntries.push(entry);$('.list-panel').append(createSettingRow(entry));}
    }
    selection = savedSelection || currentEntries[0]?.id || '';
    selectEntry(selection); renderFooter();
    $('.list-panel').scrollTop=scrollTop;
    if(focusedEntry) {
      const row=$$('[data-entry]').find(item=>item.dataset.entry===focusedEntry);
      (focusedControl?$(focusedControl,row||screen):row)?.focus({preventScroll:true});
    }
  }
  function addCategory(label) {
    const header=document.createElement('div');header.className='category-header';
    const span=document.createElement('span');span.textContent=label;header.append(span);$('.list-panel').append(header);
  }
  function createSettingRow(entry) {
    const row=document.createElement('div');row.className=`setting-row ${entry.type}${isDisabled(entry)?' disabled':''}`;
    row.setAttribute('role','listitem');row.tabIndex=0;row.dataset.entry=entry.id;
    row.innerHTML=`<span class="entry-label">${escapeHTML(text(entry.label))}</span>`;
    const value=currentValue(entry), disabled=isDisabled(entry);
    if(entry.type==='scalar') {
      const position=(value-entry.outputMin)/(entry.outputMax-entry.outputMin);
      row.innerHTML+=`<div class="slider-wrap"><input type="range" aria-label="${escapeHTML(text(entry.label))}" min="${entry.min}" max="${entry.max}" step="${entry.step || .01}" value="${entry.min+position*(entry.max-entry.min)}" ${disabled?'disabled':''}><output class="slider-value">${Math.round(position*100)}%</output></div>`;
      $('input',row).addEventListener('input',event=>{
        const fraction=(Number(event.target.value)-entry.min)/(entry.max-entry.min);
        setValue(entry,entry.outputMin+fraction*(entry.outputMax-entry.outputMin),false);
        $('output',row).textContent=`${Math.round(fraction*100)}%`;selectEntry(entry.id);renderFooter();
      });
    } else if(entry.type==='editable') {
      row.innerHTML+=`<div class="editable-wrap"><input class="editable" type="${entry.isPassword?'password':'text'}" autocomplete="off" aria-label="${escapeHTML(text(entry.label))}" maxlength="${entry.maxChars || 100}" placeholder="${escapeHTML(text(entry.hint))}" value="${escapeHTML(value)}" ${entry.charFilter==='NumbersOnly'?'inputmode="numeric"':''} ${disabled?'disabled':''}></div>`;
      $('input',row).addEventListener('input',event=>{
        let next=event.target.value;
        if(entry.charFilter==='NumbersOnly')next=next.replace(/[^0-9]/g,'');
        if(entry.charFilter==='LettersOnly')next=next.replace(/[^a-zA-Z]/g,'');
        if(entry.charFilter==='Alphanumeric')next=next.replace(/[^a-zA-Z0-9]/g,'');
        if(next!==event.target.value)notice(tr('ST_Multiplayer','Validation_NumbersOnly'));
        event.target.value=next;room[entry.id]=next;
      });
    } else {
      row.innerHTML+=`<div class="rotator"><button class="arrow previous" aria-label="Previous ${escapeHTML(text(entry.label))}" ${disabled?'disabled':''}><img src="assets/T_HorizontalArrow.png" alt=""></button><span class="option-value">${escapeHTML(valueLabel(entry))}</span><button class="arrow next" aria-label="Next ${escapeHTML(text(entry.label))}" ${disabled?'disabled':''}><img src="assets/T_HorizontalArrow.png" alt=""></button></div>`;
      $('.previous',row).onclick=()=>cycle(entry,-1);$('.next',row).onclick=()=>cycle(entry,1);
    }
    row.addEventListener('mouseenter',()=>showDetails(entry));
    row.addEventListener('mouseleave',()=>showDetails(findEntry(selection)));
    row.addEventListener('focusin',()=>selectEntry(entry.id));
    row.addEventListener('click',()=>selectEntry(entry.id));
    return row;
  }
  function findEntry(id) {return currentEntries.find(e=>e.id===id);}
  function selectEntry(id) {
    selection=id;$$('[data-entry]').forEach(row=>row.classList.toggle('selected',row.dataset.entry===id));
    showDetails(findEntry(id));
    if(page==='multiplayer'&&tabIndex===1)selectedSession=!!findEntry(id)?.session;
    renderFooter();
  }
  function showDetails(entry) {
    if(!$('.detail-title'))return;
    $('.detail-title').textContent=entry?text(entry.label):'';
    $('.detail-description').textContent=entry?stripRich(entry.description):'';
    $('.detail-warning').textContent=entry&&isDisabled(entry)?(stripRich(entry.disabledReason)|| (entry.id==='Resolution'?stripRich(tr('ST_OptionsScreen','Warning_BorderlessDisabled')):entry.id==='Entry-VSync'?stripRich(tr('ST_OptionsScreen','Warning_FullscreenOnly')):'')):'';
    $('.detail-extra').textContent=entry?stripRich(entry.dynamicDetails):'';
  }
  function choices(entry) {
    if(entry.type==='resolution') {
      return [...new Set([values.Resolution,defaults.Resolution,'1280x720','1600x900','1920x1080','2560x1440','3840x2160'])].map(value=>({value,label:{en:value.replace('x',' x ')}}));
    }
    return entry.options;
  }
  function valueLabel(entry) {
    const value=currentValue(entry);
    if(entry.id==='WindowMode') return {Fullscreen:'Fullscreen',WindowedFullscreen:'Windowed Fullscreen',Windowed:'Windowed'}[value]||String(value);
    if(entry.id==='OverallQuality'&&value===-1)return 'Custom';
    return text(choices(entry).find(o=>o.value===value)?.label) || String(value);
  }
  function cycle(entry, direction) {
    if(isDisabled(entry))return;
    const options=choices(entry), i=options.findIndex(o=>o.value===currentValue(entry));
    if(!options.length)return;
    selection=entry.id;setValue(entry,options[(i+direction+options.length)%options.length].value);
  }
  function setValue(entry,value,rerender=true) {
    if(page==='multiplayer')room[entry.id]=value;
    else {
      values[entry.id]=value;
      if(entry.id==='WindowMode'&&value!=='Fullscreen')values['Entry-VSync']=false;
      if(entry.bindingKey)for(const e of allOptions)if(e.bindingKey===entry.bindingKey)values[e.id]=value;
      if(entry.id==='OverallQuality') for(const e of allOptions)if(e.type==='integer'&&e.dependencies?.includes('OverallQuality'))values[e.id]=value;
      if(entry.type==='integer'&&entry.dependencies?.includes('OverallQuality')) {
        const quality=allOptions.filter(e=>e.type==='integer'&&e.dependencies?.includes('OverallQuality')).map(e=>values[e.id]);
        values.OverallQuality=quality.every(v=>v===quality[0])?quality[0]:-1;
      }
      persist();
    }
    if(rerender)renderSettings(true);
  }
  function setTab(index) {
    const slugs=page==='options'?['gameplay','audio','video','control']:['create','join'];
    navigate(`${page}/${slugs[index]}`);
  }
  function changeTab(direction) {setTab((tabIndex+direction+(page==='options'?4:2))%(page==='options'?4:2));}
  function footerButton(label,key,action,disabled=false) {
    const button=document.createElement('button');button.className='footer-action';button.disabled=disabled;
    button.innerHTML=`${keyImage(key)}<span>${escapeHTML(label)}</span>`;button.onclick=action;$('.screen-footer').append(button);
  }
  function entryChanged(entry) {
    if(entry.keyboard)return bindings[entry.id].keyboard!==entry.keyboard||bindings[entry.id].gamepad!==entry.gamepad;
    return values[entry.id]!==defaults[entry.id];
  }
  function renderFooter() {
    const footer=$('.screen-footer');if(!footer)return;footer.replaceChildren();
    if(page==='options'&&currentEntries.some(entryChanged)) {
      const selected=findEntry(selection);
      if(selected&&entryChanged(selected))footerButton(common('ResetEntry'),'R',resetEntry);
      footerButton(common('ResetCategory'),'F',resetCategory);
    }
    if(page==='multiplayer') {
      if(tabIndex===0)footerButton(common('CreateGame'),'Enter',createSession);
      else {footerButton(common('Join'),'Enter',joinSession,!selectedSession);footerButton(common('Refresh'),'R',refreshSessions,searchState==='searching');}
    }
    footerButton(common('Back'),'BackSpace',goBack);
  }
  function resetEntry() {
    const e=findEntry(selection);if(!e)return;
    if(e.keyboard)bindings[e.id]={keyboard:e.keyboard,gamepad:e.gamepad};
    else {setValue(e,defaults[e.id]);return;}
    persist();renderSettings(true);
  }
  function resetCategory() {
    showConfirm('',tr('ST_OptionsScreen','Confirm_ResetCategory'),[common('Yes'),common('No')],()=>{
      for(const e of currentEntries) {
        if(e.keyboard)bindings[e.id]={keyboard:e.keyboard,gamepad:e.gamepad};
        else values[e.id]=defaults[e.id];
      }
      persist();renderSettings(true);
    });
  }
  function renderControls() {
    for(const category of ['Combat','Movement','Interaction','Menu','Survival']) {
      const entries=D.controls.filter(e=>text(e.category)===category);if(!entries.length)continue;
      addCategory(category==='Interaction'?'Environmental Interaction':category);
      for(const entry of entries) {
        currentEntries.push(entry);
        const row=document.createElement('div');row.className='setting-row binding-row';row.dataset.entry=entry.id;row.tabIndex=0;row.setAttribute('role','listitem');
        row.innerHTML=`<span class="entry-label">${escapeHTML(text(entry.label))}</span>${['keyboard','gamepad'].map(device=>`<button class="key-binding" data-device="${device}" aria-label="Remap ${escapeHTML(text(entry.label))} ${device}">${keyImage(bindings[entry.id][device])}</button>`).join('')}`;
        $$('[data-device]',row).forEach(button=>button.onclick=()=>{selectEntry(entry.id);openRemap(entry,button.dataset.device);});
        row.onmouseenter=()=>showDetails(entry);row.onmouseleave=()=>showDetails(findEntry(selection));
        row.onfocusin=()=>selectEntry(entry.id);row.onclick=()=>selectEntry(entry.id);$('.list-panel').append(row);
      }
    }
  }
  function showConfirm(title,message,buttons=[common('OK')],confirmAction=null) {
    const returnFocus=modal?lastFocus:document.activeElement;
    closeModal(false);lastFocus=returnFocus;
    modal={type:'confirm',buttons,confirmAction};screen.classList.add('modal-open');screen.inert=true;
    overlay.innerHTML=`<div class="modal" role="dialog" aria-modal="true" aria-label="${escapeHTML(title||message)}"><div class="confirm-content"><img class="caution" src="assets/T_Caution.png" alt=""><div class="confirm-title">${escapeHTML(title)}</div><div class="confirm-message">${escapeHTML(message)}</div><div class="confirm-buttons">${buttons.map((label,i)=>`<button data-answer="${i}">${escapeHTML(label)}</button>`).join('')}</div></div></div>`;
    $$('[data-answer]').forEach(button=>button.onclick=()=>{
      const isConfirm=Number(button.dataset.answer)===0, action=modal?.confirmAction;
      closeModal();if(isConfirm)action?.();
    });
    $('.confirm-buttons').lastElementChild.focus({preventScroll:true});
  }
  function closeModal(restoreFocus=true) {
    modal=null;overlay.innerHTML='';screen.classList.remove('modal-open');screen.inert=false;
    if(restoreFocus&&lastFocus?.isConnected)lastFocus.focus({preventScroll:true});
  }
  function showQuit() {showConfirm('',tr('ST_GameMessages','Confirm-Exit'),[common('Yes'),common('No')],()=>{navigate('press-any-key');notice('退出流程已演示完成；网页返回启动页。');});}
  function openRemap(entry,device) {
    lastFocus=document.activeElement;modal={type:'remap',entry,device,opened:performance.now()};
    screen.classList.add('modal-open');screen.inert=true;
    overlay.innerHTML=`<div class="modal" role="dialog" aria-modal="true" aria-label="Press any ${device} ${device==='keyboard'?'key':'button'}; Escape to cancel" tabindex="-1"><div class="remap-message">Press any <em>${device}</em> ${device==='keyboard'?'key':'button'}</div></div>`;
    $('.modal').focus({preventScroll:true});
  }
  function completeRemap(key) {
    if(modal?.type!=='remap')return;
    const {entry,device}=modal;
    const conflict=D.controls.find(e=>e.id!==entry.id&&bindings[e.id][device]===key);
    const apply=()=>{bindings[entry.id][device]=key;persist();closeModal();renderSettings(true);};
    if(conflict)showConfirm('Key Already Bound',`'${key}' is already used by '${text(conflict.label)}'. Do you want to rebind it?`,[common('Yes'),common('No')],apply);
    else apply();
  }
  function renderSessions() {
    const list=$('.list-panel');
    if(searchState==='searching')list.innerHTML='<div class="throbber" style="margin:150px auto 0" role="status" aria-label="Searching sessions"></div>';
    else if(sampleSessions) {
      currentEntries.push({id:'preview-session',session:true,label:{en:sample.name},description:{en:`${sample.map}\n${sample.players}\n${sample.ping}`}});
      list.innerHTML=`<div class="sample-tag">SessionRow preview · 演示数据</div><div class="session-header"><span>ServerName</span><span>Map</span><span>Players</span><span>Private</span><span>Ping</span><span></span></div><div class="session-row" data-entry="preview-session" tabindex="0" role="listitem" aria-label="Select sample session"><span>${sample.name}</span><span>${sample.map}</span><span>${sample.players}</span><span>${sample.private}</span><span>${sample.ping}</span><button class="session-join">${common('Join')}</button></div>`;
      $('.session-row').onclick=()=>selectEntry('preview-session');
      $('.session-row').onfocusin=()=>selectEntry('preview-session');
      $('.session-join').onclick=event=>{event.stopPropagation();selectedSession=true;joinSession();};
    } else if(searchState==='empty')list.innerHTML=`<div class="session-empty">${escapeHTML(tr('ST_Multiplayer','Server-not-found'))}</div>`;
  }
  function refreshSessions() {
    if(searchState==='searching')return;
    searchState='searching';selectedSession=false;renderSettings();
    const serial=routeSerial;
    later(()=>{searchState=sampleSessions?'results':'empty';if(serial===routeSerial&&page==='multiplayer'&&tabIndex===1)renderSettings();},1400);
    notice('网页正在演示搜索流程，没有连接 Steam；真实服务器列表需要在 UE 中运行。');
  }
  function createSession() {
    // Project creates the session from the same six fields; this preview has no network backend.
    notice('创建会话为网页流程演示；不会建立真实 Steam / LAN 房间。');
    startLoading('CreateGame');
  }
  function joinSession() {
    if(!selectedSession)return;
    notice('加入的是 SessionRow 演示数据，没有真实网络会话。');startLoading('JoinGame');
  }
  function renderPressAnyKey() {
    screen.innerHTML=`<div class="press-any-key"><button>${escapeHTML(tr('ST_GameMessages','PressAnyKey'))}</button></div><div class="copyright">Copyright © 2026 Hangning. All rights reserved.</div><div class="version">1.0.0.0</div>`;
    $('.press-any-key').onclick=()=>navigate('main');
  }
  function renderStartup() {
    screen.innerHTML='<video class="startup-video" autoplay muted playsinline preload="auto" aria-label="Unreal Engine startup movie"><source src="assets/UELogoAnim.mp4" type="video/mp4"></video>';
    const video=$('video');video.onended=()=>navigate('press-any-key');
    video.play().catch(()=>{notice('点击画面播放项目启动视频。');video.onclick=()=>video.play();});
  }
  function renderCredits() {
    screen.innerHTML=`<div class="credits-window" tabindex="0" aria-label="Credits"><div class="credits-content">${D.credits.entries.map(e=>`<div class="credit-entry">${escapeHTML(text(e.role))} : ${escapeHTML(text(e.name))}</div>`).join('')}<img class="credits-logo" src="assets/T_UE-logo_W.png" alt="Unreal Engine"></div></div><footer class="screen-footer"></footer>`;
    renderFooter();const scroller=$('.credits-window');let last=0, resumeAt=performance.now()+D.credits.startDelay*1000;
    scroller.addEventListener('wheel',()=>{resumeAt=performance.now()+1800;},{passive:true});
    const step=now=>{if(last&&now>=resumeAt&&!modal&&!document.hidden)scroller.scrollTop+=D.credits.scrollSpeed*(now-last)/1000;last=now;creditsFrame=requestAnimationFrame(step);};
    creditsFrame=requestAnimationFrame(step);
  }
  const loadingPhases=['Loading_LevelLoad','Loading_WorldInit','Loading_BeginPlay','Loading_PlayerController','Loading_TextureStreaming'];
  function renderLoading(autoFinish,from='Preview') {
    screen.innerHTML=`<div class="loading-content"><div class="loading-head"><span>Loading</span><div class="throbber" aria-hidden="true"></div></div><div class="loading-reason" role="status">${escapeHTML(tr('ST_GameMessages',loadingPhases[0]))}</div></div>`;
    loadingPhases.slice(1).forEach((phase,index)=>later(()=>{const reason=$('.loading-reason');if(reason)reason.textContent=tr('ST_GameMessages',phase);},(index+1)*850));
    if(autoFinish)later(()=>{navigate('main');notice(`${from} 加载流程演示完成。游戏地图与真实存档仍由 UE 项目运行。`);},4800);
  }
  function startLoading(from) {clearPage();page='loading';$('#page-select').value='loading';renderLoading(true,from);}
  function goBack() {
    if(modal){closeModal();return;}
    if(page==='options')persist();
    if(page==='startup')return;
    navigate('main');
  }
  function keyboardKey(event) {
    const map={Space:'SpaceBar',ShiftLeft:'LeftShift',ShiftRight:'RightShift',ControlLeft:'LeftControl',ControlRight:'RightControl',AltLeft:'LeftAlt',AltRight:'RightAlt',Backspace:'BackSpace',ArrowUp:'Up',ArrowDown:'Down',ArrowLeft:'Left',ArrowRight:'Right'};
    return map[event.code] || (/^Key[A-Z]$/.test(event.code)?event.code.slice(3):/^Digit[0-9]$/.test(event.code)?['Zero','One','Two','Three','Four','Five','Six','Seven','Eight','Nine'][Number(event.code.slice(5))]:event.code);
  }
  document.addEventListener('keydown',event=>{
    if(event.key==='F1'){event.preventDefault();toggleTools();return;}
    if(event.target.closest('#preview-tools'))return;
    if(modal) {
      if(modal.type==='remap') {
        event.preventDefault();
        if(event.key==='Escape'){closeModal();return;}
        if(modal.device==='keyboard')completeRemap(keyboardKey(event));
        else {closeModal();notice('手柄重绑定已取消：检测到键盘输入。连接手柄后可捕获真实按键。');}
      } else if(event.key==='Escape'||event.key==='Backspace'){event.preventDefault();closeModal();}
      else if(['ArrowUp','ArrowDown','Tab'].includes(event.key)) {
        event.preventDefault();const buttons=$$('.confirm-buttons button'),index=buttons.indexOf(document.activeElement);
        buttons[(index+(event.key==='ArrowUp'||event.shiftKey?-1:1)+buttons.length)%buttons.length]?.focus();
      }
      return;
    }
    if(event.target.matches('input:not([type=range])')) {
      if(event.key==='Escape'){event.target.blur();event.preventDefault();}return;
    }
    if(page==='press-any-key'){event.preventDefault();navigate('main');return;}
    if(event.key==='Escape'||event.key==='Backspace'){event.preventDefault();goBack();return;}
    if(page==='main'||page.startsWith('confirm')) {
      if(['ArrowDown','ArrowUp'].includes(event.key)) {
        event.preventDefault();const items=$$('.main-links button'),index=items.indexOf(document.activeElement);
        items[(index+(event.key==='ArrowDown'?1:-1)+items.length)%items.length]?.focus();
      }return;
    }
    if(!['options','multiplayer'].includes(page))return;
    if(event.key.toLowerCase()==='q'||event.key.toLowerCase()==='e'){event.preventDefault();changeTab(event.key.toLowerCase()==='q'?-1:1);return;}
    if(['ArrowDown','ArrowUp'].includes(event.key)) {
      event.preventDefault();const index=currentEntries.findIndex(e=>e.id===selection);
      const next=currentEntries[(index+(event.key==='ArrowDown'?1:-1)+currentEntries.length)%currentEntries.length];
      if(next){selectEntry(next.id);const row=$$('[data-entry]').find(r=>r.dataset.entry===next.id);row?.focus({preventScroll:true});row?.scrollIntoView({block:'nearest'});renderFooter();}return;
    }
    if(['ArrowLeft','ArrowRight'].includes(event.key)&&!event.target.matches('input[type=range]')) {
      const entry=findEntry(selection);if(!entry||entry.session)return;event.preventDefault();
      if(entry.keyboard){const row=$$('[data-entry]').find(r=>r.dataset.entry===selection);$(`[data-device="${event.key==='ArrowRight'?'gamepad':'keyboard'}"]`,row)?.focus();return;}
      if(entry.type==='scalar')setValue(entry,Math.max(entry.outputMin,Math.min(entry.outputMax,currentValue(entry)+(event.key==='ArrowRight'?1:-1)*(entry.step||.01)*(entry.outputMax-entry.outputMin))));
      else cycle(entry,event.key==='ArrowRight'?1:-1);return;
    }
    if(page==='options'&&event.key.toLowerCase()==='r'){event.preventDefault();resetEntry();}
    if(page==='options'&&event.key.toLowerCase()==='f'&&currentEntries.some(entryChanged)){event.preventDefault();resetCategory();}
    if(page==='multiplayer'&&tabIndex===1&&event.key.toLowerCase()==='r'){event.preventDefault();refreshSessions();}
    if(event.key==='Enter'&&!event.target.closest('button')) {
      if(page==='multiplayer'){event.preventDefault();tabIndex===0?createSession():joinSession();}
      else if(tabIndex===3){event.preventDefault();const entry=findEntry(selection);if(entry)openRemap(entry,'keyboard');}
    }
  });
  overlay.addEventListener('pointerdown',event=>{
    if(modal?.type!=='remap'||performance.now()-modal.opened<200)return;
    event.preventDefault();if(modal.device==='keyboard')completeRemap(['LeftMouseButton','MiddleMouseButton','RightMouseButton'][event.button]||'LeftMouseButton');
    else {closeModal();notice('手柄重绑定已取消：检测到鼠标输入。');}
  });
  overlay.addEventListener('contextmenu',event=>event.preventDefault());
  const padKeys=['Gamepad_FaceButton_Bottom','Gamepad_FaceButton_Right','Gamepad_FaceButton_Left','Gamepad_FaceButton_Top','Gamepad_LeftShoulder','Gamepad_RightShoulder','Gamepad_LeftTriggerAxis','Gamepad_RightTriggerAxis','Gamepad_Special_Left','Gamepad_Special_Right','Gamepad_LeftThumbstick','Gamepad_RightThumbstick','Gamepad_DPad_Up','Gamepad_DPad_Down','Gamepad_DPad_Left','Gamepad_DPad_Right'];
  let previousPad=[];
  function pollGamepad() {
    const pad=navigator.getGamepads?.()?.[0];
    if(pad) {
      const pressed=pad.buttons.map(button=>button.pressed);
      pressed.forEach((down,i)=>{
        if(!down||previousPad[i])return;
        if(modal?.type==='remap'&&modal.device==='gamepad'&&performance.now()-modal.opened>250){completeRemap(padKeys[i]||`Gamepad_Button_${i}`);return;}
        if(modal?.type==='remap')return;
        const key={0:'Enter',1:'Escape',4:'q',5:'e',12:'ArrowUp',13:'ArrowDown',14:'ArrowLeft',15:'ArrowRight'}[i];
        if(key){const target=document.activeElement||document;target.dispatchEvent(new KeyboardEvent('keydown',{key,bubbles:true}));if(key==='Enter'&&target instanceof HTMLButtonElement)target.click();}
      });previousPad=pressed;
    }else previousPad=[];
    requestAnimationFrame(pollGamepad);
  }
  function toggleTools(force) {
    const expanded=typeof force==='boolean'?force:$('#tools-panel').hidden;
    $('#tools-panel').hidden=!expanded;$('#tools-toggle').setAttribute('aria-expanded',String(expanded));
  }
  $('#tools-toggle').onclick=()=>toggleTools();
  $('#page-select').onchange=event=>{navigate(event.target.value);toggleTools(false);event.target.blur();};
  $('#fullscreen').onclick=async()=>{try{if(document.fullscreenElement)await document.exitFullscreen();else await document.documentElement.requestFullscreen();toggleTools(false);}catch{notice('当前浏览器面板不支持全屏，可在外部浏览器打开同一地址。');}};
  $('#sample-session').onclick=()=>{sampleSessions=true;selectedSession=false;searchState='results';navigate('multiplayer/join');toggleTools(false);notice('当前显示 SessionRow 独立样例，内容是演示数据。');};
  stage.addEventListener('pointermove',event=>{
    const rect=stage.getBoundingClientRect(),scale=rect.height/1080;
    $('#cursor-glow').style.left=`${(event.clientX-rect.left)/scale}px`;$('#cursor-glow').style.top=`${(event.clientY-rect.top)/scale}px`;
  });
  addEventListener('resize',resize);addEventListener('hashchange',renderRoute);
  resize();renderRoute();requestAnimationFrame(pollGamepad);
})();
