#include "TitanCore.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
using namespace titan;
int main(int argc,char**argv){
 const double fs=48000;const int N=96000;
 std::ofstream csv(argc>1?argv[1]:"controlled_response_v08.csv");
 csv<<"mode,attack_setting,release_setting,time_s,gr_db,input_rms_10ms,output_rms_10ms\n";
 for(auto mode:{DetectorMode::Peak,DetectorMode::RMS,DetectorMode::Hybrid})
 for(auto att:{0,30,70,99}) for(auto rel:{0,30,70,99}){
  Parameters p;p.threshold=25;p.shape=0;p.attack=att;p.release=rel;
  p.vcaColor=0;p.dynamicColor=0;p.makeupStep=0;p.parallelMix=15;p.detectorMode=mode;
  Engine e;e.prepare(fs,1);e.setParameters(p);
  const int hop=480;double xi=0,yi=0;
  for(int i=0;i<N;++i){
   const double t=i/fs;
   // Gate from 0.20 to 0.90 s, silent recovery 0.90-1.3, burst 1.3-1.5.
   double env=(t>=.2&&t<.9)||(t>=1.3&&t<1.5)?1.0:0.0;
   float x=float(.65*env*std::sin(2*pi*500*t)),y=0;
   const float* in[]={&x};float* out[]={&y};e.process(in,out,1);
   xi+=x*x;yi+=y*y;
   if((i+1)%hop==0){
    const char* name=mode==DetectorMode::Peak?"peak":mode==DetectorMode::RMS?"rms":"hybrid";
    csv<<name<<","<<att<<","<<rel<<","<<t<<","<<e.gainReductionDb()<<","<<std::sqrt(xi/hop)<<","<<std::sqrt(yi/hop)<<"\n";
    xi=yi=0;
   }
  }
 }
 std::cout<<"Saved 48 synthetic step/burst response traces\n";
}
