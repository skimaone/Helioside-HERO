#include "TitanCore.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
using namespace titan;
void run(Engine &e,std::vector<float>&a,std::vector<float>&b){
 const float *in[]={a.data(),b.data()}; float *out[]={a.data(),b.data()};e.process(in,out,a.size());
}
int main(){
 Engine e;e.prepare(48000,2);Parameters p;p.bypass=true;e.setParameters(p);
 std::vector<float>a(48000),b(48000);for(size_t i=0;i<a.size();++i){a[i]=0.3f*std::sin(2*pi*440*i/48000);b[i]=-a[i];}
 auto orig=a;run(e,a,b);for(size_t i=0;i<a.size();++i)assert(std::abs(a[i]-orig[i])<1e-7);
 e.reset();p.bypass=false;p.threshold=0;p.shape=0;p.attack=0;p.release=15;p.parallelMix=15;p.dynamicColor=0;p.vcaColor=0;e.setParameters(p);
 a.assign(48000,0.8f);b.assign(48000,0.8f);run(e,a,b);
 assert(std::isfinite(e.gainReductionDb()));assert(e.gainReductionDb()>1.0);
 for(auto x:a)assert(std::isfinite(x));
 // Without GR, dynamicColor should not modify input beyond model's clean path.
 Engine f; f.prepare(48000,2);p.threshold=99;p.shape=99;p.dynamicColor=15;p.parallelMix=15;f.setParameters(p);
 std::vector<float>z(1000,0.0f),w(1000,0.0f);run(f,z,w);for(float x:z)assert(x==0.0f);
 // Mono mode should work independently of second pointer.
 f.prepare(44100,1);const float* mi[]={a.data(),nullptr};float* mo[]={a.data(),nullptr};f.process(mi,mo,100);
 for(int i=0;i<100;i++)assert(std::isfinite(a[i]));
 std::puts("TitanCore smoke tests passed");
}
