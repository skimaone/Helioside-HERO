#include "TitanCore.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
using namespace titan;
static std::vector<float> render(Parameters p,const std::vector<float>& source){
 Engine e;e.prepare(48000,1);e.setParameters(p);
 std::vector<float> result(source.size());const float* in[]={source.data()};float* out[]={result.data()};
 e.process(in,out,source.size());return result;
}
static double rms(const std::vector<float>&x,size_t lo=0){double s=0;for(size_t i=lo;i<x.size();++i)s+=double(x[i])*x[i];return std::sqrt(s/(x.size()-lo));}
int main(){
 std::vector<float> sine(96000);for(size_t i=0;i<sine.size();++i)sine[i]=.5f*std::sin(2*pi*257*i/48000.);
 Parameters p;p.threshold=99;p.shape=99;p.vcaColor=15;p.dynamicColor=15;
 std::vector<float> quiet=sine;for(auto&v:quiet)v*=0.001f;
 const auto clean=render(p,quiet);p.dynamicColor=0;const auto off=render(p,quiet);
 double maxDiff=0;for(size_t i=0;i<sine.size();++i)maxDiff=std::max(maxDiff,std::abs(double(clean[i])-off[i]));
 assert(maxDiff<1e-8); // no reduction => Dynamic Color does not change output
 p.threshold=0;p.shape=0;p.attack=0;p.release=10;p.vcaColor=0;p.dynamicColor=15;
 const auto on=render(p,sine);p.dynamicColor=0;const auto baseline=render(p,sine);
 maxDiff=0;for(size_t i=24000;i<sine.size();++i)maxDiff=std::max(maxDiff,std::abs(double(on[i])-baseline[i]));
 assert(maxDiff>1e-5); // color acts during compression
 assert(rms(on,24000)>0 && std::isfinite(rms(on,24000)));
 p.parallelMix=0;p.makeupStep=0;p.dynamicColor=0;p.vcaColor=0;
 const auto direct=render(p,sine);
 // direct position not bypass, and is dominated by dry input
 assert(std::abs(rms(direct,24000)-rms(sine,24000))<.08);
 p.bypass=true;const auto bypass=render(p,sine);
 for(size_t i=0;i<sine.size();++i)assert(bypass[i]==sine[i]);
 // Stress switching: finite across large inputs and extreme settings.
 p.bypass=false;p.threshold=0;p.dynamicColor=15;p.vcaColor=15;p.parallelMix=15;
 p.vintageDrive=5;p.colorStrength=1;
 std::vector<float> loud(96000);for(size_t i=0;i<loud.size();++i)loud[i]=(i%101<50?4.f:-4.f);
 for(float x:render(p,loud))assert(std::isfinite(x));
 std::puts("Research regression tests passed");
}
