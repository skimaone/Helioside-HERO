#include "TitanCore.h"
#include <vector>
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace titan;
static double run(double frequency, Parameters p){
 constexpr int fs=48000,N=96000;
 std::vector<float> in(N),out(N);
 for(int n=0;n<N;++n)in[n]=float(.6*std::sin(2*pi*frequency*n/fs));
 Engine e;e.prepare(fs,1);e.setParameters(p);
 const float* ip[]={in.data()};float* op[]={out.data()};e.process(ip,op,N);
 double v=0;for(int n=N/2;n<N;++n)v+=double(out[n])*out[n];
 return std::sqrt(v/(N/2));
}
int main(){
 Parameters p;p.threshold=0;p.parallelMix=15;p.attack=5;p.release=20;
 p.vcaColor=0;p.dynamicColor=0;p.audioToneDepth=0.;
 p.shape=0; const auto hard=run(500.,p);
 p.shape=99;const auto soft=run(500.,p);
 assert(hard<soft);
 p.shape=25;p.tone=Parameters::Normal;
 const auto normalBass=run(65.,p);
 const auto normalHigh=run(8000.,p);
 p.tone=Parameters::Fat;const auto fatBass=run(65.,p);
 p.tone=Parameters::Air;const auto airHigh=run(8000.,p);
 printf("shape0 %.6f shape99 %.6f; bass normal %.6f fat %.6f; high normal %.6f air %.6f\n",hard,soft,normalBass,fatBass,normalHigh,airHigh);
 assert(fatBass>normalBass*1.001);
 assert(airHigh>normalHigh*1.001);
}
