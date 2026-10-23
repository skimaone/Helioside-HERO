#include "TitanCore.h"
#include <vector>
#include <cmath>
#include <cassert>
#include <iostream>
using namespace titan;
static double probe(double hz,Parameters p){
 constexpr int fs=48000,N=96000;
 std::vector<float> in(N),out(N);
 for(int i=0;i<N;++i) in[i]=(float)(.65*std::sin(2*pi*hz*i/fs));
 Engine e;e.prepare(fs,1);e.setParameters(p);
 const float* i[]={in.data()};float* o[]={out.data()};e.process(i,o,N);
 double z=0;for(int j=N/2;j<N;++j)z+=(double)out[j]*out[j];return std::sqrt(z/(N/2));
}
int main(){
 Parameters p;p.threshold=0;p.shape=30;p.attack=5;p.release=10;p.parallelMix=15;
 p.tone=Parameters::Normal;auto nL=probe(65,p),nH=probe(8500,p);
 p.tone=Parameters::Fat;auto fL=probe(65,p);
 p.tone=Parameters::Air;auto aH=probe(8500,p);
 std::cout<<"bass fat/normal="<<fL/nL<<" high air/normal="<<aH/nH<<"\n";
 assert(fL>nL);assert(aH>nH);
 p.tone=Parameters::Normal;p.shape=0;double h=probe(500,p);
 p.shape=99;double s=probe(500,p);assert(h<s);
}
