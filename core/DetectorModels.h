#pragma once
#include <algorithm>
#include <cmath>
namespace titan {
enum class DetectorMode { Peak=0, RMS=1, Hybrid=2 };
struct DetectorResponse {
 double peak=0.0, power=0.0;
 double lastFs=0.,lastRise=0.,lastFall=0.,rise=0.,fall=0.;
 void reset(){peak=power=0.0;}
 static double coefficient(double seconds,double fs){return std::exp(-1.0/(std::max(1.e-6,seconds)*fs));}
 double tick(double sample,double fs,double riseSec,double fallSec,DetectorMode mode,double hybridWeight){
  if(fs!=lastFs||riseSec!=lastRise||fallSec!=lastFall){lastFs=fs;lastRise=riseSec;lastFall=fallSec;rise=coefficient(riseSec,fs);fall=coefficient(fallSec,fs);}
  double magnitude=std::abs(sample);
  double a=(magnitude>peak?rise:fall);
  peak=a*peak+(1-a)*magnitude;
  double square=sample*sample;
  a=(square>power?rise:fall);
  power=a*power+(1-a)*square;
  double rms=std::sqrt(std::max(0.0,power));
  if(mode==DetectorMode::Peak)return peak;
  if(mode==DetectorMode::RMS)return rms;
  const double w=std::clamp(hybridWeight,0.0,1.0);
  return w*peak+(1-w)*rms;
 }
};
}
