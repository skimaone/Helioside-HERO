#include "TitanCore.h"
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
using namespace titan;
int main(){
 constexpr double fs=48000;
 // Rectified peak and power RMS share the same nominal rise/fall constants
 // but react differently to duty cycle, as expected from their estimators.
 DetectorResponse pk,rms,hyb;
 double a=0,b=0,h=0;
 for(int n=0;n<48000;n++){
  double x=(n%480<48)?0.8:0.0;
  a=pk.tick(x,fs,.00025,.004,DetectorMode::Peak,.65);
  b=rms.tick(x,fs,.00025,.004,DetectorMode::RMS,.65);
  h=hyb.tick(x,fs,.00025,.004,DetectorMode::Hybrid,.65);
  assert(std::isfinite(a)&&std::isfinite(b)&&std::isfinite(h));
  assert(h>=std::min(a,b)-1e-12 && h<=std::max(a,b)+1e-12);
 }
 // For a held constant, each detector should converge to the same level.
 pk.reset();rms.reset();
 for(int n=0;n<48000;n++){
  a=pk.tick(.45,fs,.00025,.004,DetectorMode::Peak,.65);
  b=rms.tick(.45,fs,.00025,.004,DetectorMode::RMS,.65);
 }
 assert(std::abs(a-.45)<1e-5 && std::abs(b-.45)<1e-5);
 for(auto mode:{DetectorMode::Peak,DetectorMode::RMS,DetectorMode::Hybrid}){
  Engine e;e.prepare(fs,1);Parameters p;p.threshold=25;p.shape=0;p.attack=0;p.release=0;p.detectorMode=mode;
  e.setParameters(p);
  std::vector<float> x(48000),y(48000);
  for(int i=0;i<48000;i++)x[i]=(i>4000&&i<29000)?float(.7*std::sin(2*pi*440*i/fs)):0.f;
  const float* in[]={x.data()};float* out[]={y.data()};
  // Block size invariance: two blocks must equal one block, with continuous state.
  e.process(in,out,48000);
  double total=0;for(float v:y){assert(std::isfinite(v));total+=v*v;}
  assert(total>0);
  e.reset();std::vector<float> y2(48000);
  const float* in1[]={x.data()};float* out1[]={y2.data()};e.process(in1,out1,13001);
  const float* in2[]={x.data()+13001};float* out2[]={y2.data()+13001};e.process(in2,out2,34999);
  for(int i=0;i<48000;i++)assert(std::abs(y[i]-y2[i])<1e-6);
 }
 std::cout<<"Detector models + block invariance pass\n";
}
