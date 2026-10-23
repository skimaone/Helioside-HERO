#pragma once
#include <algorithm>
#include <cmath>
namespace titan {
enum class DetectorMode { Peak=0, RMS=1, Hybrid=2 };
struct DetectorResponse {
 double peak=0.0, power=0.0;
 void reset(){peak=power=0.0;}
 static double coefficient(double seconds,double fs){return std::exp(-1.0/(std::max(1.e-6,seconds)*fs));}
 double tick(double sample,double fs,double riseSec,double fallSec,DetectorMode mode,double hybridWeight){
  double magnitude=std::abs(sample);
  double a=coefficient(magnitude>peak?riseSec:fallSec,fs);
  peak=a*peak+(1-a)*magnitude;
  double square=sample*sample;
  a=coefficient(square>power?riseSec:fallSec,fs);
  power=a*power+(1-a)*square;
  double rms=std::sqrt(std::max(0.0,power));
  if(mode==DetectorMode::Peak)return peak;
  if(mode==DetectorMode::RMS)return rms;
  const double w=std::clamp(hybridWeight,0.0,1.0);
  return w*peak+(1-w)*rms;
 }
};
}
