#include "TitanCore.h"
#include <complex>
#include <cstdio>
#include <vector>
using namespace titan;
int failures=0;
void check(bool b,const char* text){std::printf("[%s] %s\n",b?"PASS":"FAIL",text);if(!b)++failures;}
struct Result{std::vector<float> audio;double gr=0,power=0;std::complex<double> h1{},h2{},h3{};};
Result render(Parameters p,double amplitude=.35,double fs=48000.,bool bursts=false){
 Engine e;e.prepare(fs,1);e.setParameters(p);Result r;const int n=int(fs*2);r.audio.resize((size_t)n);
 for(int i=0;i<n;++i){float x=float(amplitude*(bursts&&(i%int(fs))>int(fs*.2)?.1:1.)*std::sin(2*pi*1000*i/fs));float y;const float* in[]={&x};float* out[]={&y};e.process(in,out,1);r.audio[(size_t)i]=y;
  if(i>=n/2){r.gr+=e.gainReductionDb();r.power+=double(y)*y;r.h1+=double(y)*std::polar(1.,-2*pi*1000*i/fs);r.h2+=double(y)*std::polar(1.,-4*pi*1000*i/fs);r.h3+=double(y)*std::polar(1.,-6*pi*1000*i/fs);}}
 r.gr/=n/2;r.power/=n/2;return r;
}
double difference(const Result&a,const Result&b){double aa=0,ab=0,bb=0,err=0;for(size_t i=a.audio.size()/2;i<a.audio.size();++i){aa+=double(a.audio[i])*a.audio[i];ab+=double(a.audio[i])*b.audio[i];bb+=double(b.audio[i])*b.audio[i];}double g=ab/std::max(bb,1e-30);for(size_t i=a.audio.size()/2;i<a.audio.size();++i){double v=a.audio[i]-g*b.audio[i];err+=v*v;}return 10*std::log10(std::max(err,1e-30)/std::max(aa,1e-30));}
int main(){
 Parameters p;p.threshold=99;p.shape=0;
 auto off=render(p,.1);p.dynamicColor=15;auto dynamic=render(p,.1);check(off.audio==dynamic.audio,"Dynamic Colour is exactly inactive below compression onset, including the solver floor");
 p.vcaColor=15;check(render(p,.1).audio==off.audio,"Colour activation does not amplify the numerical GR floor");
 p=Parameters{};auto clean=render(p);p.vcaColor=15;auto vintage=render(p);p.vcaColor=0;p.dynamicColor=15;auto punch=render(p);
 check(difference(clean,vintage)>-45.,"Full VCA Colour has resolved level-matched contrast during compression");
 check(difference(clean,punch)>-55.,"Full Dynamic Colour has resolved level-matched contrast during compression");
 check(punch.gr>clean.gr,"Dynamic waveform expansion increases the existing feedback GR indication");
 // Isolate waveform shape at identical gain history to examine relative H3 sign
 // and stepped control range; this is a model property, not hardware phase data.
 p=Parameters{};p.feedbackLaw=FeedbackLaw::Equilibrium;clean=render(p);
 double lastVintage=0,lastDynamic=0;
 for(int step=0;step<16;++step){p.vcaColor=step;p.dynamicColor=0;auto v=render(p);p.vcaColor=0;p.dynamicColor=step;auto d=render(p);double vh=std::abs(v.h3/v.h1),dh=std::abs(d.h3/d.h1);check(vh+1e-10>=lastVintage,"VCA 16-step range increases isolated H3 contrast");check(dh+1e-10>=lastDynamic,"Dynamic 16-step range increases isolated H3 contrast");lastVintage=vh;lastDynamic=dh;if(step==15){auto a=v.h3/v.h1-clean.h3/clean.h1,b=d.h3/d.h1-clean.h3/clean.h1;double cosine=std::real(a*std::conj(b))/(std::abs(a)*std::abs(b));std::printf("Isolated H3 phase cosine %.9f\n",cosine);check(cosine<-.95,"Dynamic H3 opposes Vintage H3 in the coherent model probe");}}
 for(double fs:{44100.,48000.,96000.,192000.})for(int v:{0,7,15})for(int d:{0,7,15}){Parameters q;q.vcaColor=v;q.dynamicColor=d;q.attack=0;q.release=0;q.threshold=15;auto r=render(q,.9,fs,true);bool finite=true;for(float x:r.audio)finite&=std::isfinite(x)&&std::abs(x)<10.;check(finite,"Combined colour endpoints stay finite for hot bursts and supported rates");}
 Parameters fast;fast.dynamicColor=15;fast.attack=0;fast.release=0;Parameters slow=fast;slow.attack=99;slow.release=99;check(difference(render(fast,.7,48000,true),render(slow,.7,48000,true))>-40.,"Dynamic Colour follows the compressor's attack/release history");
 return failures?1:0;
}
