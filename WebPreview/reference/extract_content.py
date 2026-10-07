"""Read-only Unreal package extraction for the standalone web preview."""
from pathlib import Path
import concurrent.futures, json, re, struct, subprocess

ROOT = Path(__file__).resolve().parents[2]
CONTENT = ROOT / 'Plugins/GameMainMenu/Content'
TOOL = r'E:\UnrealLibrary\UE_5.7\Engine\Binaries\Win64\UnrealPackageTool.exe'
OUT = ROOT / 'WebPreview/reference'
OUT.mkdir(parents=True, exist_ok=True)
cache = {}

def package(path):
    path = Path(path)
    key = str(path)
    if key not in cache:
        result = subprocess.run([TOOL, '-j', 'PackageInfo', '-p', str(path), '--names', '--exports', '--imports', '--text'], capture_output=True, check=True)
        cache[key] = json.loads(result.stdout.decode('utf-8-sig'))
    return cache[key]

class Reader:
    def __init__(self, data, metadata, pos=0): self.b, self.m, self.p = data, metadata, pos
    def take(self, n):
        b=self.b[self.p:self.p+n]; assert len(b)==n, (self.p,n); self.p+=n; return b
    def n(self, fmt): return struct.unpack('<'+fmt,self.take(struct.calcsize('<'+fmt)))[0]
    def name(self):
        i,num=self.n('i'),self.n('i'); assert 0<=i<len(self.m['Names']), (self.p,i)
        s=self.m['Names'][i]; return s if num==0 else s+'_'+str(num-1)
    def string(self):
        n=self.n('i'); assert abs(n)<1000000, (self.p,n)
        if n==0:return ''
        return self.take(n).rstrip(b'\x00').decode('utf-8') if n>0 else self.take(-n*2).decode('utf-16-le').rstrip('\x00')
    def typename(self):
        name=self.name(); count=self.n('i'); assert 0<=count<20
        return (name,[self.typename() for _ in range(count)])
    def text(self):
        flags=self.n('I'); hist=self.n('b')
        if hist==-1:
            has=self.n('i')
            return {'source':self.string() if has else ''}
        if hist==0:return {'namespace':self.string(),'key':self.string(),'source':self.string()}
        if hist==11:return {'table':self.name(),'key':self.string()}
        raise ValueError(('text history',hist,self.p))
    def obj(self):
        i=self.n('i')
        if i==0:return None
        return self.m['Imports'][-i-1]['PathName'].removeprefix('Object:') if i<0 else self.m['Exports'][i-1]['Path'].removeprefix('Object:')
    def value(self,t,flags=0):
        typ,args=t
        if typ=='NameProperty':return self.name()
        if typ=='StrProperty':return self.string()
        if typ=='TextProperty':return self.text()
        if typ=='BoolProperty':return bool(flags&16)
        if typ in ('EnumProperty','ByteProperty'):return self.name() if args else self.n('B')
        if typ=='FloatProperty':return round(self.n('f'),6)
        if typ=='DoubleProperty':return self.n('d')
        if typ=='IntProperty':return self.n('i')
        if typ=='UInt32Property':return self.n('I')
        if typ=='ObjectProperty':return self.obj()
        if typ=='ArrayProperty':return [self.value(args[0]) for _ in range(self.n('i'))]
        if typ=='MapProperty':
            removed=self.n('i'); assert removed==0
            return [{'key':self.value(args[0]),'value':self.value(args[1])} for _ in range(self.n('i'))]
        if typ=='StructProperty':return self.props()
        raise ValueError(('type',t,self.p))
    def props(self):
        out={}
        while True:
            name=self.name()
            if name=='None':return out
            typ=self.typename(); size=self.n('i'); flags=self.n('B')
            if flags&1:self.n('i')
            if flags&2:self.take(16)
            assert not flags&4, ('extensions',name)
            start=self.p
            try: out[name]=self.value(typ,flags)
            except Exception as e: out[name]={'_unparsed':str(e),'hex':self.b[start:start+size].hex()}
            self.p=start+size

def props(path, export=0):
    m=package(path); e=m['Exports'][export]
    r=Reader(Path(path).read_bytes(),m,e['SerialOffset']); prefix=r.n('B')
    assert prefix==0, (path,prefix)
    return r.props()

def source_path(ref):
    p=ref.removeprefix('/GameMainMenu/').split('.')[0]
    return CONTENT/(p+'.uasset')

def localized(t):
    if isinstance(t,str):return {'en':t,'zh':''}
    if not t:return {'en':'','zh':''}
    if 'table' in t:
        tab=t['table'].rsplit('/',1)[-1].split('.')[0]
        return {**strings.get(tab,{}).get(t['key'],{'en':t['key'],'zh':''}),'table':tab,'key':t['key']}
    s=t.get('source','')
    return {'en':s,'zh':s if re.search('[\u4e00-\u9fff]',s) else ''}

def entry(path):
    p=props(path); typ=p.get('ValueType','Bool').split('::')[-1]; key=p.get('BindingKey','')
    options=[]
    if typ=='Bool':
        default=p.get('bDefaultBool',False)
        options=[{'value':True,'label':localized({'table':'ST_OptionsValues','key':'Generic_On'})},{'value':False,'label':localized({'table':'ST_OptionsValues','key':'Generic_Off'})}]
    elif typ=='Scalar': default=float(p.get('DefaultValue') or 0)
    elif typ=='Integer':default=p.get('DefaultIntValue',0)
    else:default=p.get('DefaultStringValue','')
    for pair in p.get('AvailableIntegerOptions' if typ=='Integer' else 'AvailableOptions',[]):
        options.append({'value':pair['key'],'label':localized(pair['value'])})
    if key=='FullscreenMode':
        options=[{'value':x,'label':localized({'table':'ST_OptionsValues','key':y})} for x,y in [('Fullscreen','Window_Fullscreen'),('WindowedFullscreen','Window_Borderless'),('Windowed','Window_Windowed')]]
        default='WindowedFullscreen'
    if typ=='Culture':
        # These are the cultures explicitly configured by DefaultGameMainMenu.ini.
        options=[{'value':x,'label':{'en':y,'zh':y}} for x,y in [('en','English'),('zh-Hans','简体中文'),('zh-Hant','繁體中文'),('ja','日本語'),('ko','한국어'),('fr','Français'),('de','Deutsch'),('it','Italiano'),('ru','Русский')]]
    e={'id':p.get('DataId',Path(path).stem),'type':typ.lower(),'bindingKey':key,'label':localized(p.get('DisplayName')),'description':localized(p.get('Description')),'defaultValue':default,'options':options,'dependencies':p.get('EditDependencyIds',[]),'applyImmediately':p.get('bApplyImmediately',True),'packagedOnly':p.get('bPackagedBuildOnly',False),'hint':localized(p.get('HintText')),'disabledReason':localized(p.get('DisableRichReason')),'dynamicDetails':localized(p.get('DynamicDetails')),'maxChars':p.get('MaxCharCount',0),'isPassword':p.get('bIsPassword',False),'charFilter':p.get('CharFilter','All').split('::')[-1],'source':str(Path(path).relative_to(ROOT)).replace('\\','/')}
    if typ=='Scalar':e.update(min=p.get('ScalarMin',0),max=p.get('ScalarMax',1),outputMin=p.get('ScalarOutMin',0),outputMax=p.get('ScalarOutMax',2),step=p.get('StepSize',.01),numericType=p.get('NumericType','Percentage').split('::')[-1])
    return e

def screen(path):
    tabs=[]
    for t in props(path).get('Tabs',[]):
        tp=props(source_path(t)); cats=[]
        for c in tp.get('Categories',[]):
            cp=props(source_path(c)); cats.append({'id':cp.get('CategoryId',''),'label':localized(cp.get('CategoryName')),'headers':[localized(h) for h in cp.get('HeaderColumns',[])],'entries':[entry(source_path(x)) for x in cp.get('Entries',[])]})
        tabs.append({'id':tp.get('TabId',''),'label':localized(tp.get('TabName')),'categories':cats})
    return {'tabs':tabs}

if __name__=='__main__':
    files=list((CONTENT/'BP/StringTable').glob('*.uasset'))+list((CONTENT/'BP/DataAsset/OptionScreen').rglob('*.uasset'))+list((CONTENT/'BP/DataAsset/MultiplayerScreen').rglob('*.uasset'))+[CONTENT/'BP/DataAsset/DA_Credits.uasset']+list((CONTENT/'BP/InputData/Input').rglob('*.uasset'))
    with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:list(pool.map(package,files))
    strings={}
    for path in (CONTENT/'BP/StringTable').glob('*.uasset'):
        table={}
        for text in package(path).get('GatherableTextMap',[]):
            for site in text['SourceSiteContexts']:
                source=text['SourceData']['SourceString'];table[site['KeyName']]={'en':source,'zh':source if re.search('[\u4e00-\u9fff]',source) else ''}
        strings[path.stem]=table
    credits_path=CONTENT/'BP/DataAsset/DA_Credits.uasset'; cp=props(credits_path)
    credits={'entries':[],'scrollSpeed':cp.get('ScrollSpeed',50),'startDelay':cp.get('StartDelay',1),'lineHeight':cp.get('LineHeight',30),'entrySpacing':cp.get('EntrySpacing',0),'logoSpacing':cp.get('LogoSpacing',120)}
    # Gatherable source sites preserve exact entry order without guessing names/roles.
    ce={}
    for text in package(credits_path).get('GatherableTextMap',[]):
        for site in text['SourceSiteContexts']:
            match=re.search(r'Entries\((\d+)\).*\.(Name|Role)$',site['SiteDescription'])
            if match:ce.setdefault(int(match[1]),{})[match[2].lower()]=localized({'source':text['SourceData']['SourceString']})
    credits['entries']=[ce[i] for i in sorted(ce)]
    raw_inputs={str(p.relative_to(ROOT)).replace('\\','/'):{'properties':[props(p,i) for i in range(len(package(p)['Exports']))],'metadata':package(p)} for p in (CONTENT/'BP/InputData/Input').rglob('*.uasset')}
    controls=[]
    mappings=raw_inputs['Plugins/GameMainMenu/Content/BP/InputData/Input/IMC_Default.uasset']['properties'][0]['DefaultKeyMappings']['Mappings']
    for p,v in raw_inputs.items():
        if '/IA_' not in p:continue
        action=v['properties'][0]; config=v['properties'][1]
        ref=v['metadata']['Exports'][0]['Path'].removeprefix('Object:')
        keys=[m['Key'].get('KeyName','') for m in mappings if m['Action']==ref]
        controls.append({'id':config['Name'],'label':localized(config.get('DisplayName')),'description':localized(action.get('ActionDescription')),'category':localized(config.get('DisplayCategory')),'keyboard':next((k for k in keys if not k.startswith('Gamepad_')),''),'gamepad':next((k for k in keys if k.startswith('Gamepad_')),''),'source':p})
    data={'strings':strings,'options':screen(CONTENT/'BP/DataAsset/OptionScreen/DA_OptionsConfig.uasset'),'multiplayer':screen(CONTENT/'BP/DataAsset/MultiplayerScreen/DA_MultiplayerConfig.uasset'),'credits':credits,'controls':controls,'sourceNotes':['All source labels and descriptions come from Unreal StringTable/DataAsset packages. No project localization archives or locres were found; empty zh fields mean no verified Chinese translation.','Resolution options are populated dynamically by Unreal from the display hardware and are not stored in the DataAsset.','Culture options follow SupportedCultures in Config/DefaultGameMainMenu.ini; display names use conventional native culture names.']}
    session_row=props(CONTENT/'BP/DataAsset/MultiplayerScreen/Tabs/Category/JoinGame/DA_SessionRow.uasset')
    data['multiplayer']['sessionHeaders']=[localized(t) for t in session_row.get('HeaderColumns',[])]
    (OUT/'raw-inputs.json').write_text(json.dumps(raw_inputs,ensure_ascii=False,indent=2),encoding='utf-8')
    (OUT/'asset-properties.json').write_text(json.dumps({str(p.relative_to(ROOT)).replace('\\','/'):props(p) for p in files if '/StringTable/' not in p.as_posix() and '/InputData/' not in p.as_posix()},ensure_ascii=False,indent=2),encoding='utf-8')
    (OUT/'content.json').write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
    (ROOT/'WebPreview/data.js').write_text('window.MENU_DATA = '+json.dumps(data,ensure_ascii=False,indent=2)+';\n',encoding='utf-8')
    print(json.dumps({'stringTables':{k:len(v) for k,v in strings.items()},'optionsTabs':len(data['options']['tabs']),'multiplayerTabs':len(data['multiplayer']['tabs']),'credits':len(credits['entries'])}))
