/* Browser acceptance checks. Requires Playwright; no packages are needed to run the site. */
const assert = require('node:assert/strict');
const fs = require('node:fs');
const path = require('node:path');
const playwrightPath = process.env.PLAYWRIGHT_MODULE || 'playwright';
const { chromium } = require(playwrightPath);

(async () => {
  const browser = await chromium.launch({channel: 'msedge', headless: true});
  const context = await browser.newContext({viewport: {width: 1152, height: 864}, deviceScaleFactor: 1});
  const page = await context.newPage();
  const errors = [], failedResources = [], checks = [];
  page.on('pageerror', error => errors.push(error.message));
  page.on('response', response => {if (response.status() >= 400) failedResources.push(`${response.status()} ${response.url()}`);});
  const check = (label, condition) => {assert.ok(condition, label);checks.push(label);};
  const go = async route => {
    await page.goto(`http://127.0.0.1:8765/#${route}`);
    await page.evaluate(() => document.fonts.ready);
    await page.waitForTimeout(260);
  };
  const shot = name => page.screenshot({path:path.join(__dirname,'reference',`web-${name}.png`)});
  const row = id => page.locator(`[data-entry="${id}"]`);
  const value = id => row(id).locator('.option-value').textContent();
  try {
    await go('main');
    check('Main menu has all six source actions', await page.locator('.main-links button').count() === 6);
    await shot('main');
    await page.getByRole('button',{name:'Options',exact:true}).click();
    check('Main menu opens Options', await page.locator('.setting-row').count() === 7);
    await page.evaluate(() => document.fonts.ready);
    await shot('gameplay');
    await row('Entry-DamageNumbers').getByRole('button',{name:'Next DamageNumbers'}).click();
    check('Boolean arrow changes value', await value('Entry-DamageNumbers') === 'On');
    await page.reload();await page.waitForSelector('.setting-row');
    check('Settings survive reload', await value('Entry-DamageNumbers') === 'On');
    await page.getByRole('button',{name:/Reset Category/}).click();
    check('Reset opens a modal with default focus on No', await page.locator('.modal').count() === 1 && await page.evaluate(()=>document.activeElement.textContent)==='No');
    await page.getByRole('button',{name:'Yes',exact:true}).click();
    check('Reset restores the tab defaults', await value('Entry-DamageNumbers') === 'Off');

    await go('options/audio');
    check('Audio has all twelve fields', await page.locator('.setting-row').count() === 12);
    check('Backend audio default maps to 50 percent', await row('OverallVolume').locator('output').textContent() === '50%');
    await row('OverallVolume').locator('input').fill('0.75');
    check('Audio slider works', await row('OverallVolume').locator('output').textContent() === '75%');
    await shot('audio');

    await go('options/video');
    check('Resolution is disabled for borderless', await row('Resolution').locator('.next').isDisabled());
    await row('WindowMode').locator('.previous').click();
    check('Fullscreen enables resolution and VSync', !await row('Resolution').locator('.next').isDisabled() && !await row('Entry-VSync').locator('.next').isDisabled());
    await row('OverallQuality').locator('.previous').click();
    check('Overall quality updates the integer subsettings', await value('ShadowQuality') === 'High');
    const scale = Number((await row('ResolutionScale').locator('output').textContent()).replace('%',''));
    check('Quality preset never writes an invalid scalar', scale >= 0 && scale <= 100);
    await row('ShadowQuality').locator('.previous').click();
    check('Individual quality produces Custom preset', await value('OverallQuality') === 'Custom');
    await page.reload();await page.waitForSelector('.setting-row');
    check('Custom quality survives reload', await value('OverallQuality') === 'Custom');
    await row('OverallQuality').click();await page.getByRole('button',{name:/Reset Entry/}).click();
    check('Resetting preset updates child values', await value('ShadowQuality') === 'Epic');
    await page.locator('.list-panel').evaluate(node=>node.scrollTop=0);
    await page.evaluate(() => document.fonts.ready);await shot('video');

    await go('multiplayer/create');
    check('Create game has six source fields', await page.locator('.setting-row').count() === 6);
    check('Password initially disabled', await row('Password').locator('input').isDisabled());
    await row('HasPassword').locator('.next').click();
    check('HasPassword enables password', !await row('Password').locator('input').isDisabled());
    await row('Password').locator('input').fill('12ab34');
    check('Password filters nonnumeric input', await row('Password').locator('input').inputValue() === '1234');
    await row('RoomName').locator('input').fill('My game');
    await shot('multiplayer');
    await go('multiplayer/join');
    check('Join does not fabricate live sessions', await page.locator('.session-row').count() === 0);
    await page.getByRole('button',{name:/Refresh/}).click();
    check('Refresh shows the search state', await page.locator('.throbber').count() === 1);
    await page.getByRole('tab',{name:'CreateGame'}).click();
    await page.getByRole('tab',{name:'JoinGame'}).click();
    check('Leaving a search never leaves it stuck', !await page.getByRole('button',{name:/Refresh/}).isDisabled());
    await page.getByRole('button',{name:/Refresh/}).click();
    await page.waitForSelector('.session-empty');
    check('Empty search uses the source message', await page.locator('.session-empty').textContent() === 'Server not found');
    await shot('join');
    await page.getByRole('button',{name:/预览导航/}).click();
    await page.locator('#tools-panel details').evaluate(node=>node.open=true);
    await page.locator('#sample-session').click();
    check('Explicit session sample includes inline Join', await page.locator('.session-join').count() === 1);
    await page.locator('.session-row').focus();await page.keyboard.press('ArrowDown');
    check('Session sample supports keyboard selection', await page.locator('.session-row').evaluate(node=>document.activeElement===node));

    await go('options/control');
    check('Control includes all fourteen actions', await page.locator('.binding-row').count() === 14);
    await shot('control');
    await page.getByRole('button',{name:'Remap Aim keyboard'}).click();
    await page.keyboard.press('KeyZ');
    check('Keyboard remap captures the new key', await page.getByRole('button',{name:'Remap Aim keyboard'}).locator('img').getAttribute('alt') === 'Z');
    await page.getByRole('button',{name:'Remap Aim keyboard'}).click();await page.keyboard.press('KeyR');
    check('Conflicting key opens source confirmation', await page.locator('.confirm-title').textContent() === 'Key Already Bound');
    await page.getByRole('button',{name:'No',exact:true}).click();
    check('Cancel keeps the existing binding', await page.getByRole('button',{name:'Remap Aim keyboard'}).locator('img').getAttribute('alt') === 'Z');
    await row('ID_Aim').focus();await page.keyboard.press('ArrowRight');
    check('Directional navigation can select gamepad binding', await page.evaluate(()=>document.activeElement.dataset.device)==='gamepad');
    await page.keyboard.press('Enter');
    check('Gamepad remap has its own prompt', await page.locator('.remap-message').textContent() === 'Press any gamepad button');
    await shot('remap');await page.keyboard.press('Escape');
    check('Escape cancels the remap overlay', await page.locator('.modal').count() === 0);

    for (const route of ['credits','press-any-key','loading','confirm','confirm-ok','confirm-cancel','startup']) {
      await go(route);check(`Route ${route} renders`, await page.locator('#screen').evaluate(node=>node.childElementCount>0));
      if(route==='startup')check('Original startup movie is playable', await page.locator('video').evaluate(video=>video.readyState>=2));
      if(['credits','confirm','press-any-key','loading'].includes(route))await shot(route);
    }
    await go('main');await page.getByRole('button',{name:'New Story',exact:true}).click();
    check('New Story starts a simulated loading flow', await page.locator('.loading-content').count()===1);
    await page.waitForSelector('.main-links',{timeout:7000});
    check('Simulated loading returns to a usable menu', await page.locator('.main-links button').count()===6);
    await page.goto('http://127.0.0.1:8765/review.html');
    await page.getByRole('button',{name:/建议结构/}).click();
    check('Review architecture toggle works', await page.locator('#proposed-diagram').isVisible());
    await page.locator('[data-filter="P1"]').click();
    check('Review priority filter works', await page.locator('.finding-card:visible').count()===3);
    await shot('review');
    await page.setViewportSize({width:390,height:844});await go('options/gameplay');
    check('Narrow viewport does not horizontally overflow', await page.evaluate(()=>document.documentElement.scrollWidth<=innerWidth));
    await page.getByRole('button',{name:/预览导航/}).click();
    check('Mobile navigation remains accessible', await page.locator('#page-select').isVisible());
    check('No uncaught JavaScript errors', errors.length===0);
    check('No missing browser resources', failedResources.length===0);
    const report={passed:checks.length,checks,errors,failedResources,testedAt:new Date().toISOString()};
    fs.writeFileSync(path.join(__dirname,'reference','verification.json'),JSON.stringify(report,null,2));
    console.log(JSON.stringify(report,null,2));
  } finally {await browser.close();}
})().catch(error=>{console.error(error);process.exitCode=1;});
