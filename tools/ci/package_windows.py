"""Package and independently check a complete, runnable AMD64 VST3 bundle."""
import argparse, hashlib, json, re, struct, zipfile, tempfile
from pathlib import Path
NAME = 'Suntech Audio HERO'
VERSION = '0.1.3'
def digest(data): return hashlib.sha256(data).hexdigest()
def exports(path):
    data=path.read_bytes()
    if data[:2]!=b'MZ': raise ValueError('Not a Windows binary')
    pe=struct.unpack_from('<I',data,0x3c)[0]
    if data[pe:pe+4]!=b'PE\0\0': raise ValueError('Invalid PE')
    machine,count=struct.unpack_from('<HH',data,pe+4)
    optional_size,flags=struct.unpack_from('<HH',data,pe+20); optional=pe+24
    if machine!=0x8664 or struct.unpack_from('<H',data,optional)[0]!=0x20b or not flags&0x2000: raise ValueError('Expected AMD64 DLL')
    sections=optional+optional_size
    def offset(rva):
        for i in range(count):
            size,address,raw_size,raw=struct.unpack_from('<IIII',data,sections+i*40+8)
            if address<=rva<address+max(size,raw_size): return raw+rva-address
        raise ValueError('Unmapped PE RVA')
    directory=offset(struct.unpack_from('<I',data,optional+112)[0])
    n=struct.unpack_from('<I',data,directory+24)[0]
    rva=struct.unpack_from('<I',data,directory+32)[0]
    result=[]
    for i in range(n):
        start=offset(struct.unpack_from('<I',data,offset(rva)+4*i)[0]);result.append(data[start:data.index(b'\0',start)].decode('ascii'))
    if not {'GetPluginFactory','InitDll','ExitDll'}.issubset(result): raise ValueError('Missing VST3 exports')
    return sorted(result)
def metadata(bundle):
    info=json.loads(re.sub(r',\s*([}\]])',r'\1',(bundle/'Contents/Resources/moduleinfo.json').read_text(encoding='utf-8-sig')))
    if info['Name']!=NAME or info['Version']!=VERSION: raise ValueError('Wrong product/version')
    if info['Factory Info']['Vendor']!='Suntech Audio': raise ValueError('Wrong manufacturer')
    expected={'ABCDEF019182FAEB536E74634865726F','ABCDEF011234ABCD536E74634865726F','ABCDEF01C0DEF00D536E74634865726F'}
    if {c['CID'] for c in info['Classes']}!=expected: raise ValueError('Wrong plugin identity')
    for f in ['DejaVu_LICENSE.txt','DSP_MIT.txt']:
        if not (bundle/'Contents/Resources/Licenses'/f).is_file():raise ValueError('Font notice missing')
    return info

def verify(directory):
    checksum,name=(directory/'SHA256SUMS.txt').read_text().split()
    if Path(name).name!=name:raise ValueError('Unsafe name')
    archive=directory/name
    if digest(archive.read_bytes())!=checksum:raise ValueError('Download checksum mismatch')
    manifest=json.loads((directory/'build-manifest.json').read_text())
    with zipfile.ZipFile(archive) as z:
        prefix=NAME+'.vst3/'
        if z.testzip() is not None or set(z.namelist())!={prefix+p for p in manifest['files']}:raise ValueError('Incomplete bundle')
        for p,h in manifest['files'].items():
            if digest(z.read(prefix+p))!=h:raise ValueError('Bundle file checksum mismatch')
        with tempfile.TemporaryDirectory() as temp:
            z.extractall(temp);bundle=Path(temp)/(NAME+'.vst3');metadata(bundle)
            if exports(bundle/'Contents/x86_64-win'/(NAME+'.vst3'))!=manifest['exports']:raise ValueError('Export mismatch')
    report=dict(status='PASS',revision=manifest['revision'],sha256=checksum,files=len(manifest['files']),exports=manifest['exports'])
    (directory/'verified-delivery.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--bundle',type=Path);ap.add_argument('--output',type=Path);ap.add_argument('--revision');ap.add_argument('--verify-download',type=Path);a=ap.parse_args()
    if a.verify_download:return verify(a.verify_download)
    metadata(a.bundle);symbols=exports(a.bundle/'Contents/x86_64-win'/(NAME+'.vst3'));a.output.mkdir(parents=True,exist_ok=True)
    files={str(p.relative_to(a.bundle)).replace('\\','/'):digest(p.read_bytes()) for p in a.bundle.rglob('*') if p.is_file()}
    archive=a.output/f'SuntechAudioHERO-{VERSION}-Windows-x64-VST3.zip'
    with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for p in sorted(files):z.write(a.bundle/p,NAME+'.vst3/'+p)
    (a.output/'SHA256SUMS.txt').write_text(digest(archive.read_bytes())+'  '+archive.name+'\n')
    (a.output/'build-manifest.json').write_text(json.dumps(dict(product=NAME,version=VERSION,revision=a.revision,architecture='AMD64',exports=symbols,files=files),indent=2)+'\n')
    verify(a.output)
if __name__=='__main__':main()
