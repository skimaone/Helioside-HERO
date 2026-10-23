#include "OversampledNonlinearity.h"
#include <cassert>
#include <cmath>
#include <vector>
#include <cstdio>
static double bin(const std::vector<double>&x,double f,int fs){
 double a=0,b=0;const int start=int(x.size()/2),n=int(x.size())-start;
 for(int i=start;i<int(x.size());++i){double theta=2.*3.141592653589793*f*i/fs;a+=x[i]*std::cos(theta);b+=x[i]*std::sin(theta);}
 return 2.*std::hypot(a,b)/n;
}
int main(){
 constexpr int fs=48000,N=96000;const double fundamental=15000,alias=3000;
 titan::OversampledNonlinearity<4> a;
 std::vector<double> reference(N),oversampled(N),native(N);
 for(int i=0;i<N;++i){double x=.85*std::sin(2.*3.141592653589793*fundamental*i/fs);
  reference[i]=x; native[i]=x+.5*x*x*x;oversampled[i]=a.tick(x,[](double y){return y+.5*y*y*y;});
  assert(std::isfinite(oversampled[i]));
 }
 auto n=bin(native,alias,fs),o=bin(oversampled,alias,fs);
 printf("Cubic alias at 3k: native %.6f, oversampled %.6f, rejection %.1f dB\n",n,o,20*std::log10(n/std::max(o,1e-15)));
 assert(o<n*.2);
}
