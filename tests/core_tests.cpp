#define titan original_titan
#include "../research/src/TitanCore.h"
#undef titan
#include "TitanCore.h"
#include <cstdio>
#include <chrono>
#include <vector>
int main(){
 double error=0.;
 // Preserve zero-colour compression, curves, stereo histories and research modes.
 // 0.1.3 deliberately changes the two nonzero gain-cell colour paths.
 for(int rate:{44100,48000,96000,192000})for(int scenario=0;scenario<24;++scenario){
  titan::Engine a;original_titan::Engine b;a.prepare(rate,2);b.prepare(rate,2);
  titan::Parameters p;original_titan::Parameters q;
  p.threshold=q.threshold=(scenario*17)%100;p.shape=q.shape=(scenario*31)%100;
  p.attack=q.attack=(scenario*13)%100;p.release=q.release=(scenario*7)%100;
  p.vcaColor=q.vcaColor=0;p.dynamicColor=q.dynamicColor=0;
  p.parallelMix=q.parallelMix=(scenario*5)%16;p.makeupStep=q.makeupStep=scenario%24;
  p.tone=static_cast<titan::Parameters::Tone>(scenario%3-1);q.tone=static_cast<original_titan::Parameters::Tone>(scenario%3-1);
  p.stereoLink=q.stereoLink=scenario%2==0;
  p.feedbackLaw=static_cast<titan::FeedbackLaw>(scenario%3);q.feedbackLaw=static_cast<original_titan::FeedbackLaw>(scenario%3);
  p.detectorMode=static_cast<titan::DetectorMode>(scenario%3);q.detectorMode=static_cast<original_titan::DetectorMode>(scenario%3);
  a.setParameters(p);b.setParameters(q);
  std::vector<float>x(8192),y(8192),aa(8192),ab(8192),ba(8192),bb(8192);
  for(int i=0;i<8192;++i){x[i]=(float)(.7*std::sin(2*titan::pi*91*i/rate)+.2*std::sin(2*titan::pi*7043*i/rate));y[i]=(float)(.4*std::sin(2*titan::pi*2701*i/rate));}
  const float* input[]={x.data(),y.data()};float* oa[]={aa.data(),ab.data()};float* ob[]={ba.data(),bb.data()};
  a.process(input,oa,x.size());b.process(input,ob,x.size());
  for(int i=0;i<8192;++i){error=std::max(error,std::abs(double(aa[i])-ba[i]));error=std::max(error,std::abs(double(ab[i])-bb[i]));}
 }
 std::printf("Clean production vs untouched v10: 96 scenarios; max sample error %.12g\n",error);
 return error<2e-6?0:1;
}
