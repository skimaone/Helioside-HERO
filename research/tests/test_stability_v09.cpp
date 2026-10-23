#include "TitanCore.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace titan;
struct Metrics {double max=0,steady=0,over=0,recoveryMs=0; bool finite=true;};
Metrics run(FeedbackLaw law,int attack,int release,DetectorMode mode,double fs=48000.,bool split=false){
 Parameters p;p.feedbackLaw=law;p.threshold=25;p.shape=0;p.attack=attack;p.release=release;
 p.detectorMode=mode;p.vcaColor=0;p.dynamicColor=0;
 Engine e;e.prepare(fs,1);e.setParameters(p);Metrics m;std::vector<double> trace;
 int N=int(fs*1.45), begin=int(fs*.2),end=int(fs*.95);
 trace.reserve(N); double steadySum=0;int steadyN=0;
 for(int i=0;i<N;i++){
  double t=i/fs;double env=(i>=begin&&i<end)?1.:0.;
  float inS=float(env*.65*std::sin(2*pi*500*t)),outS=0;
  const float*in[]={&inS}; float*out[]={&outS};e.process(in,out,1);
  double g=e.gainReductionDb();trace.push_back(g);
  if(!std::isfinite(g)||!std::isfinite(outS))m.finite=false;
  if(i>=begin&&i<end)m.max=std::max(m.max,g);
  if(t>=.8&&t<.94){steadySum+=g;steadyN++;}
 }
 m.steady=steadyN?steadySum/steadyN:0.;m.over=std::max(0.,m.max-m.steady);
 for(int i=end;i<N;i++)if(trace[i]<=1.){m.recoveryMs=1000.*(i-end)/fs;break;}
 return m;
}
int main(int argc,char**argv){
 const char*path=argc>1?argv[1]:"stability_metrics_v09.csv";
 std::ofstream csv(path);csv<<"law,detector,attack,release,max_gr_db,steady_gr_db,overshoot_db,recovery_to_1db_ms,finite\n";
 bool ok=true;
 for(auto law:{FeedbackLaw::Legacy,FeedbackLaw::Regularized,FeedbackLaw::Equilibrium})
 for(auto mode:{DetectorMode::Peak,DetectorMode::RMS,DetectorMode::Hybrid})
 for(int attack:{0,30,70,99})for(int release:{0,30,70,99}){
  auto m=run(law,attack,release,mode);
  const char*label=law==FeedbackLaw::Legacy?"legacy":law==FeedbackLaw::Regularized?"regularized":"equilibrium";
  const char*det=mode==DetectorMode::Peak?"peak":mode==DetectorMode::RMS?"rms":"hybrid";
  csv<<label<<","<<det<<","<<attack<<","<<release<<","<<m.max<<","<<m.steady<<","<<m.over<<","<<m.recoveryMs<<","<<m.finite<<"\n";
  ok&=m.finite&&m.max<=50.0001;
 }
 if(!ok)return 1;
 std::cout<<"144 feedback law/detector/ballistics scenarios evaluated\n";
}
