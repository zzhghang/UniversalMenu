"""Lossless extraction of UI texture source pixels without launching Unreal.
Uses the project's installed Oodle decompressor, UE FCompressedBuffer layout,
and the documented UE ImageCoreDelta row-difference transform.
"""
import ctypes,json,pathlib,re,struct,subprocess,zlib
from extract_widget_properties import Reader

ROOT=pathlib.Path(__file__).resolve().parents[2]
REF=ROOT/'WebPreview/reference'
OUT=ROOT/'WebPreview/assets'
TOOL='E:/UnrealLibrary/UE_5.7/Engine/Binaries/Win64/UnrealPackageTool.exe'
lib=ctypes.CDLL('E:/UnrealLibrary/UE_5.7/Engine/Binaries/DotNET/UnrealBuildTool/oo2core_9_win64.dll')
dec=lib.OodleLZ_Decompress
dec.restype=ctypes.c_int64
dec.argtypes=[ctypes.c_void_p,ctypes.c_int64,ctypes.c_void_p,ctypes.c_int64,ctypes.c_int,ctypes.c_int,ctypes.c_int,ctypes.c_void_p,ctypes.c_int64,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_int64,ctypes.c_int]

def decompress(b):
    start=b.find(bytes.fromhex('b7756362'))
    assert start>=0,'no compressed source data'
    method,compressor,level,exp,count,raw,total=struct.unpack_from('>4BIQQ',b,start+8)
    if method==0:return b[start+64:start+64+raw]
    assert method==3,'unsupported compression method'
    sizes=struct.unpack_from('>'+'I'*count,b,start+64)
    pos=start+64+count*4; chunks=[]
    for i,size in enumerate(sizes):
        expected=min(1<<exp,raw-i*(1<<exp)); part=b[pos:pos+size];pos+=size
        if size==expected:chunks.append(part);continue
        src=ctypes.create_string_buffer(part);dst=ctypes.create_string_buffer(expected)
        result=dec(src,size,dst,expected,1,0,0,None,0,None,None,None,0,3)
        assert result==expected,(result,expected)
        chunks.append(dst.raw)
    return b''.join(chunks)

def cuts(w,h,bpp):
    if w*h<=136*136:return [(0,w,0,h)]
    stride=w*bpp
    if stride<=4096:part=w
    else:
        n=(stride+4095)//4096;partbytes=(stride+n//2)//n
        part=((partbytes+63)&~63)//bpp
    result=[]
    for x in range(0,w,part):
        sw=min(part,w-x);n=max(1,sw*h//32768)
        while n>512:n>>=1
        rows=(h+n-1)//n
        for y in range(0,h,rows):result.append((x,sw,y,min(rows,h-y)))
    return result

def delta_reverse(raw,w,h,bpp):
    out=bytearray(raw)
    for x,sw,y,sh in cuts(w,h,bpp):
        for row in range(y+1,y+sh):
            pos=(row*w+x)*bpp;prev=pos-w*bpp
            for k in range(sw*bpp):out[pos+k]=(out[pos+k]+out[prev+k])&255
    return bytes(out)

def png(raw,w,h,path,bpp):
    if bpp==4:
        rgba=bytearray(len(raw));rgba[0::4]=raw[2::4];rgba[1::4]=raw[1::4];rgba[2::4]=raw[0::4];rgba[3::4]=raw[3::4]
    else:rgba=raw
    def chunk(tag,data):return struct.pack('>I',len(data))+tag+data+struct.pack('>I',zlib.crc32(tag+data)&0xffffffff)
    scan=b''.join(b'\0'+rgba[y*w*bpp:(y+1)*w*bpp] for y in range(h))
    path.write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,6 if bpp==4 else 0,0,0,0))+chunk(b'IDAT',zlib.compress(scan,9))+chunk(b'IEND',b''))

def texture(path):
    jp=REF/(path.stem+'-package.json')
    if not jp.exists():subprocess.run([TOOL,'-j','PackageInfo','--all','-p',str(path),'-o',str(jp)],check=True,stdout=subprocess.DEVNULL)
    j=json.loads(jp.read_text(encoding='utf-8-sig'));b=path.read_bytes()
    e=next(e for e in j['Exports'] if e['Class'].endswith('.Texture2D'))
    p=Reader(b[e['SerialOffset']:e['SerialOffset']+e['ScriptSerializationEndOffset']],j['Names'],j,1).tags()
    s=p['Source'];w=s['SizeX'];h=s['SizeY'];fmt=s['Format'];compression=s.get('CompressionFormat','TSCF_None')
    bpp={'TSF_BGRA8':4,'TSF_G8':1}[fmt]
    raw=decompress(b)
    if compression=='TSCF_UEDELTA':raw=delta_reverse(raw,w,h,bpp)
    elif compression!='TSCF_None':raise ValueError('unsupported source format '+compression)
    assert len(raw)>=w*h*bpp
    output=OUT/(path.stem+'.png');png(raw[:w*h*bpp],w,h,output,bpp)
    return {'source':str(path.relative_to(ROOT)),'output':str(output.relative_to(ROOT)),'width':w,'height':h,'format':fmt,'lossless':True}

if __name__=='__main__':
    paths=list((ROOT/'Plugins/GameMainMenu/Content/Assets/Others').glob('T*.uasset'))
    for c in ['CData_MouseKeyboard','CData_Gamepad']:
        b=(ROOT/'Plugins/GameMainMenu/Content/BP/InputData'/(c+'.uasset')).read_bytes()
        for m in re.finditer(rb'/GameMainMenu/Assets/MengGe_Game_UI_Icons/[A-Za-z0-9_/-]+',b):
            p=ROOT/('Plugins/GameMainMenu/Content/'+m.group().decode()[len('/GameMainMenu/'):]+'.uasset')
            if p.exists() and p not in paths:paths.append(p)
    report=[]
    for p in paths:
        try:r=texture(p);report.append(r);print(p.name,r['width'],r['height'],flush=True)
        except Exception as e:report.append({'source':str(p),'error':str(e)});print(p.name,'ERROR',e,flush=True)
    (REF/'texture-manifest.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
