import soundfile as sf,numpy as np,glob,json
from scipy.signal import butter,sosfilt, correlate
from pathlib import Path
out={}
for path in glob.glob('/mnt/data/Dave-Hill*mp3'):
 name=Path(path).stem.split('Titan-')[-1]
 if name not in ['SHAPE-99-0-99','ATTACK-0-99-0','RELEASE-0-99-0','NORMAL-FAT-NORMAL-AIR']:continue
 a,fs=sf.read(path,dtype='float32'); x=np.mean(a,axis=1);length=len(x)/fs
 n=4 if name.startswith('NORMAL') else 3
 # comparison windows: central 70% of each nominal equal division, excluding switching boundaries
 seg=[]
 for i in range(n):
  lo=int((i+.15)*len(x)/n);hi=int((i+.85)*len(x)/n);v=x[lo:hi]
  rms=np.sqrt(np.mean(v*v));peak=np.max(np.abs(v));
  bands={}
  for label,low,high in [('low',30,150),('mid',150,2500),('high',2500,12000)]:
   s=sosfilt(butter(3,[low,high],btype='bandpass',fs=fs,output='sos'),v)
   bands[label]=round(float(20*np.log10(np.sqrt(np.mean(s*s))+1e-12)),2)
  # local crest 50ms and 100ms
  crests={}
  for ms in [25,100]:
   block=int(ms*fs/1000);z=v[:len(v)//block*block].reshape(-1,block)
   cc=20*np.log10(np.max(abs(z),axis=1)/(np.sqrt(np.mean(z*z,axis=1))+1e-10)+1e-10)
   crests[str(ms)]=round(float(np.median(cc)),2)
  seg.append(dict(t=[round(lo/fs,2),round(hi/fs,2)],rmsDb=round(float(20*np.log10(rms+1e-12)),2),peakDb=round(float(20*np.log10(peak+1e-12)),2),crestDb=round(float(20*np.log10(peak/rms)),2),medianCrest=crests,bands=bands))
 out[name]=dict(duration=round(length,3),sampleRate=fs,segments=seg)
print(json.dumps(out,indent=2))
Path('/mnt/data/titan_audio_v06_results.json').write_text(json.dumps(out,indent=2))
