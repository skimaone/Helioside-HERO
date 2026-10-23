#include "TitanCore.h"
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
int main(){
 titan::Engine e; e.prepare(48000,2); titan::Parameters p; p.tone=titan::Parameters::Fat; p.detectorToneDepth=.85; p.feedbackLaw=titan::FeedbackLaw::Regularized; p.stereoLink=false; e.setParameters(p);
 const int N=8192; std::vector<float> in0(N),in1(N),out0(N),out1(N);
 for(int n=0;n<N;n++){in0[n]=.22f*std::sin(2*titan::pi*91*n/48000);in1[n]=.14f*std::sin(2*titan::pi*3000*n/48000);}
 const float* ins[]={in0.data(),in1.data()}; float* outs[]={out0.data(),out1.data()};
 e.process(ins,outs,N);
 for(int n=0;n<N;n++){assert(std::isfinite(out0[n]));assert(std::isfinite(out1[n]));}
 titan::Engine ee;ee.prepare(48000,2);ee.setParameters(p);
 std::vector<float> x0(N),x1(N); const float* src[]={in0.data(),in1.data()};
 for(int n=0;n<N;n+=127){int len=std::min(127,N-n); const float* a[]={src[0]+n,src[1]+n};float* b[]={x0.data()+n,x1.data()+n};ee.process(a,b,len);}
 double mx=0; for(int i=0;i<N;i++)mx=std::max(mx,std::max(std::abs(double(out0[i]-x0[i])),std::abs(double(out1[i]-x1[i]))));
 assert(mx<1e-6);std::cout<<"Independent sidechain / block invariance max diff: "<<mx<<"\n";
}
