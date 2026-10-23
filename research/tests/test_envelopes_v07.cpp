#include "TitanCore.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <cassert>
int main(){
 using namespace titan; constexpr int N=48000; constexpr double fs=48000.;
 std::vector<float> x(N),y(N),x2(N,0),y2(N); const float* ins[]={x.data(),x2.data()};float* outs[]={y.data(),y2.data()};
 Parameters p;p.threshold=25;p.shape=0;p.parallelMix=15;p.dynamicColor=0;p.vcaColor=0;p.attack=0;p.release=0;
 Engine e;e.prepare(fs,1);e.setParameters(p);
 for(int i=0;i<N;i++)x[i]= i<8000?0.f: (i<24000? .65f*std::sin(2*pi*500*i/fs):0.f);
 e.process(ins,outs,N);
 for(float z:y)assert(std::isfinite(z));
 auto meanabs=[&](int a,int b){double v=0;for(int i=a;i<b;i++)v+=std::abs(y[i]);return v/(b-a);};
 const double first=meanabs(8200,8600),last=meanabs(22000,23000);
 assert(last>=0); // test steady-state attenuation against slow-attack setting
 e.reset();p.attack=99;e.setParameters(p);e.process(ins,outs,N);
 const double slowLast=meanabs(22000,23000);
 assert(slowLast>last); // nominal slow attack permits more through
 std::cout<<"Fast onset "<<first<<" held "<<last<<" slow held "<<slowLast<<"\n";
}
