"""Read-only UE 5.7 tagged property extraction for web reference material.

Package table JSON is produced by Epic's UnrealPackageTool. Values not covered
by the small decoder are explicitly retained as raw hex rather than guessed.
"""
import json, pathlib, struct, re

ROOT = pathlib.Path(__file__).resolve().parents[2]
REF = ROOT / 'WebPreview/reference'

class Reader:
    def __init__(self, data, names, package, offset=0):
        self.b, self.names, self.package, self.p = data, names, package, offset
    def read(self, fmt):
        value=struct.unpack_from('<'+fmt,self.b,self.p); self.p+=struct.calcsize('<'+fmt)
        return value[0] if len(value)==1 else list(value)
    def name(self):
        i,n=self.read('ii')
        if i<0 or i>=len(self.names): raise ValueError('invalid name '+str(i))
        return self.names[i]+('_'+str(n-1) if n else '')
    def string(self):
        n=self.read('i')
        if abs(n)>len(self.b):raise ValueError('bad string')
        v=self.b[self.p:self.p+abs(n)*(2 if n<0 else 1)]
        self.p+=len(v)
        return v.decode('utf-16le' if n<0 else 'utf-8',errors='replace').rstrip('\0')
    def type(self):
        n=self.name(); count=self.read('i')
        if count<0 or count>10:raise ValueError('bad type count')
        return {'name':n,'params':[self.type() for _ in range(count)]}
    def ref(self,i):
        if not i:return None
        t=self.package['Exports'][i-1] if i>0 else self.package['Imports'][-i-1]
        return t.get('Path',t.get('ObjectName'))
    def tags(self):
        out={}
        for _ in range(1000):
            name=self.name()
            if name=='None':return out
            typ=self.type();size=self.read('i');flags=self.read('B')
            if size<0 or size>len(self.b)-self.p:raise ValueError('bad property size')
            if flags&1:self.read('i')
            if flags&2:self.p+=16
            if flags&4:
                ext=self.read('B')
                if ext&2:self.p+=2
            raw=self.b[self.p:self.p+size];self.p+=size
            r=Reader(raw,self.names,self.package)
            try:value=r.value(typ,flags)
            except Exception as e:value={'raw':raw.hex(),'type':typ,'error':str(e)}
            out[name]=value
        raise ValueError('too many tags')
    def text(self):
        flags=self.read('I');history=self.read('b')
        if history==11:return {'stringTable':self.name(),'key':self.string()}
        if history==0:return {'namespace':self.string(),'key':self.string(),'source':self.string()}
        if history==-1:
            return {'text':self.string()} if self.p<len(self.b) and self.read('i') else ''
        return {'history':history,'raw':self.b.hex()}
    def value(self,t,flags=0):
        n=t['name']; args=t['params']
        if n=='BoolProperty':return bool(flags&16)
        if n=='FloatProperty':return self.read('f')
        if n=='DoubleProperty':return self.read('d')
        if n in ('IntProperty','UInt32Property'):return self.read('i')
        if n=='Int64Property':return self.read('q')
        if n in ('ObjectProperty','ClassProperty','WeakObjectProperty'):return self.ref(self.read('i'))
        if n=='NameProperty':return self.name()
        if n=='StrProperty':return self.string()
        if n=='TextProperty':return self.text()
        if n in ('ByteProperty','EnumProperty'):return self.name() if len(self.b)>=8 else self.read('B')
        if n=='StructProperty':
            sn=args[0]['name'] if args else ''
            if sn=='LinearColor':return dict(zip('rgba',self.read('ffff')))
            if sn=='Color':return dict(zip('bgra',self.read('BBBB')))
            if sn in ('Vector2D','Vector2f'):return dict(zip('xy',self.read('dd' if len(self.b)==16 else 'ff')))
            if sn=='Vector':return dict(zip('xyz',self.read('ddd' if len(self.b)==24 else 'fff')))
            if sn in ('Guid','SoftObjectPath'):return {'struct':sn,'raw':self.b.hex()}
            return self.tags()
        if n=='ArrayProperty':
            count=self.read('i')
            if count<0 or count>100000:raise ValueError('bad array count')
            if not count:return []
            if args[0]['name']=='StructProperty':return [self.tags() for _ in range(count)]
            return [self.value(args[0]) for _ in range(count)]
        return {'type':t,'raw':self.b.hex()}

def read_package(jp):
    package=json.loads(jp.read_text(encoding='utf-8-sig'))
    pn=package['PackageFileSummary']['PackageName']
    if not pn.startswith('/GameMainMenu/'):return
    asset=ROOT / ('Plugins/GameMainMenu/Content/'+pn[len('/GameMainMenu/'):]+'.uasset')
    if not asset.exists():return
    b=asset.read_bytes();out=[]
    for i,e in enumerate(package['Exports']):
        raw=b[e['SerialOffset']:e['SerialOffset']+e['ScriptSerializationEndOffset']]
        if not raw:continue
        try:props=Reader(raw,package['Names'],package,1).tags()
        except Exception as ex:
            try:props=Reader(raw,package['Names'],package,0).tags()
            except Exception:props={'_error':str(ex),'_raw':raw.hex()}
        out.append({'index':i+1,'name':e['ObjectName'],'class':e['Class'],'path':e['Path'],'properties':props})
    result={'asset':str(asset.relative_to(ROOT)),'package':pn,'objects':out,'softReferences':package.get('SoftObjectPaths'),'text':package.get('GatherableTextMap')}
    (REF/(jp.stem.replace('-package','')+'-properties.json')).write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
    return pn,len(out),sum('_error' not in o['properties'] for o in out)

if __name__=='__main__':
    for jp in REF.glob('*-package.json'):
        try:print(read_package(jp))
        except Exception as e:print(jp.name,repr(e))
