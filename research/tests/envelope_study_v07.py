"""Repeat-passage envelope comparisons: diagnostic, NOT a measured gain-reduction trace.
Usage: python tests/envelope_study_v07.py [path_to_reference_audio_dir]
"""
import json, sys
from pathlib import Path
import numpy as np, soundfile as sf
from scipy.signal import correlate, correlation_lags, butter, sosfilt
ROOT=Path(sys.argv[1]) if len(sys.argv)>1 else Path(__file__).resolve().parents[1]/'reference/audio'
FILES={'shape':'Dave-Hill-Designs-Titan-SHAPE-99-0-99.mp3','attack':'Dave-Hill-Designs-Titan-ATTACK-0-99-0.mp3','release':'Dave-Hill-Designs-Titan-RELEASE-0-99-0.mp3','tone':'Dave-Hill-Designs-Titan-NORMAL-FAT-NORMAL-AIR.mp3'}
results={}
for key, fname in FILES.items():
    x,fs=sf.read(ROOT/fname, always_2d=True); x=x.mean(axis=1)
    sections=4 if key=='tone' else 3
    seglen=len(x)//sections
    # Integrate ~10 ms windows, robust to MP3 phase perturbations.
    h=max(1,round(fs*.01)); sig=x[:len(x)//h*h].reshape(-1,h)
    env=np.sqrt(np.mean(sig**2,axis=1)+1e-15)
    count=len(env)//sections
    parts=[env[i*count:(i+1)*count] for i in range(sections)]
    output=[]
    for idx,p in enumerate(parts):
        core=p[int(.1*len(p)):int(.9*len(p))]
        output.append({'index':idx,'rms_dbfs':float(20*np.log10(np.sqrt(np.mean(core**2))+1e-12)),
                       'envelope_p10_dbfs':float(20*np.log10(np.percentile(core,10)+1e-12)),
                       'envelope_p90_dbfs':float(20*np.log10(np.percentile(core,90)+1e-12)),
                       'envelope_dynamic_range_db':float(20*np.log10((np.percentile(core,90)+1e-12)/(np.percentile(core,10)+1e-12)))})
    # Align comparison passages in time using envelope cross-correlation (+/- 1 second).
    comparisons=[]
    for a,b in ([(0,1),(0,2),(1,2)] if sections==3 else [(0,2),(0,1),(2,3)]):
        p,q=parts[a],parts[b]; n=min(len(p),len(q)); p=p[:n]; q=q[:n]
        p=p-np.mean(p); q=q-np.mean(q)
        cc=correlate(q,p,'full',method='fft'); lags=correlation_lags(n,n,mode='full')
        allowed=np.abs(lags)<=100
        ind=np.argmax(np.where(allowed,cc,-np.inf)); lag=int(lags[ind]);
        if lag>=0: aa=p[:n-lag];bb=q[lag:]
        else: aa=p[-lag:];bb=q[:n+lag]
        corr=float(np.dot(aa,bb)/(np.linalg.norm(aa)*np.linalg.norm(bb)+1e-12))
        comparisons.append({'from':a,'to':b,'lag_seconds':lag*.01,'envelope_correlation':corr})
    results[key]={'duration_seconds':len(x)/fs,'section_count':sections,'section_assumption':'equal-duration sections; switch points not verified', 'sections':output,'envelope_correlations':comparisons}
print(json.dumps(results,indent=2)); output=Path(__file__).resolve().parents[1]/'docs/ENVELOPE_STUDY_V07.json'; output.write_text(json.dumps(results,indent=2));print('saved',output)
