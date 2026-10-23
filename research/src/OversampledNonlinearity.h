#pragma once
// Standalone FIR oversampling research module (not yet inserted into the
// closed-loop engine, because dry/wet and detector delay need compensation).
// One sample in/out; fixed-size state, no allocation during processing.
#include <array>
#include <cmath>
#include <algorithm>
namespace titan {
template<int Factor> class OversampledNonlinearity {
 static_assert(Factor==4 || Factor==8,"Use 4x or 8x");
 static constexpr int Taps=Factor*96+1, Hist=64;
 std::array<double,Hist> input_{};
 std::array<double,Taps> output_{};
 std::array<std::array<double,Hist>,Factor> interp_{};
 std::array<double,Taps> decim_{};
 int ip_=0,op_=0;
 static double sinc(double x){return std::abs(x)<1.e-12?1.:std::sin(3.141592653589793*x)/(3.141592653589793*x);}
 static double blackman(double u){return .42-.5*std::cos(2.*3.141592653589793*u)+.08*std::cos(4.*3.141592653589793*u);}
 public:
 OversampledNonlinearity(){
  for(int phase=0;phase<Factor;++phase){
   double total=0.;
   for(int t=0;t<Hist;++t){
    // Causal fractional delay kernel centered at 15 samples.
    double lag=t-31.-double(phase)/Factor;
    interp_[phase][t]=sinc(lag)*blackman(double(t)/(Hist-1));
    total+=interp_[phase][t];
   }
   for(auto &c:interp_[phase])c/=total;
  }
  double total=0;
  for(int k=0;k<Taps;++k){
   const double lag=k-(Taps-1)/2.;
   decim_[k]=(.84/Factor)*sinc((.84/Factor)*lag)*blackman(double(k)/(Taps-1));
   total+=decim_[k];
  }
  for(auto &c:decim_)c/=total;
 }
 void reset(){input_.fill(0.);output_.fill(0.);ip_=op_=0;}
 // Caller function is a stateless nonlinear waveshaper at high sample rate.
 template<class Fn> double tick(double x,Fn&&fn){
  input_[ip_]=x;
  for(int phase=0;phase<Factor;++phase){
   double v=0.;
   for(int k=0;k<Hist;++k)v+=interp_[phase][k]*input_[(ip_-k+Hist)%Hist];
   output_[op_]=fn(v);
   op_=(op_+1)%Taps;
  }
  double y=0.;
  for(int k=0;k<Taps;++k)y+=decim_[k]*output_[(op_-1-k+Taps)%Taps];
  ip_=(ip_+1)%Hist;
  return y;
 }
 // Group delay (input interpolation + output decimation) ~31 host samples.
 // The actual delay is fractional because of causal interpolation.
 static constexpr int approximateLatencySamples=79;
};
}
