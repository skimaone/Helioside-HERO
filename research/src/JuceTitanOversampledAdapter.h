#pragma once
// Requires JUCE DSP module. Include only in JUCE plugin project, NOT the standalone CMake tests.
// This oversamples the WHOLE feedback-loop model so controller timing remains coherent.
// A narrower nonlinear-only oversampling island is a later optimization requiring aligned
// feedback and proper conversion between gain-control/audio sampling grids.
#include <juce_dsp/juce_dsp.h>
#include "TitanCore.h"
#include <array>
#include <memory>
namespace titan {
template<typename T=float>
class JuceTitanOversampledAdapter {
 static_assert(std::is_same_v<T,float>,"TitanCore currently processes float buffers");
 Engine engine_;
 std::unique_ptr<juce::dsp::Oversampling<T>> oversampler_;
 int channels_=2, factor_=4;
 double hostRate_=48000.;
public:
 void prepare(double fs,int channels,int maxBlock,int factor=4){
  hostRate_=fs;channels_=juce::jlimit(1,2,channels);factor_=factor;
  jassert(factor==2||factor==4||factor==8||factor==16);
  const int stages=factor==2?1:(factor==4?2:(factor==8?3:4));
  oversampler_=std::make_unique<juce::dsp::Oversampling<T>>(
   static_cast<size_t>(channels_),static_cast<size_t>(stages),
   juce::dsp::Oversampling<T>::filterHalfBandFIREquiripple,true,true);
  oversampler_->initProcessing(static_cast<size_t>(maxBlock));
  engine_.prepare(fs*factor,channels_);
 }
 void reset(){if(oversampler_)oversampler_->reset();engine_.reset();}
 void setParameters(Parameters p){engine_.setParameters(p);}
 int latencySamples()const {return oversampler_?static_cast<int>(std::lround(oversampler_->getLatencyInSamples())):0;}
 double gainReductionDb()const{return engine_.gainReductionDb();}
 void process(juce::AudioBuffer<T>& buffer){
  jassert(oversampler_ && buffer.getNumChannels()==channels_);
  if(!oversampler_)return;
  if(buffer.getNumSamples()==0)return;
  juce::dsp::AudioBlock<T> block(buffer);
  auto up=oversampler_->processSamplesUp(block);
  const float* ins[2]={up.getChannelPointer(0),channels_==2?up.getChannelPointer(1):nullptr};
  float* outs[2]={up.getChannelPointer(0),channels_==2?up.getChannelPointer(1):nullptr};
  engine_.process(ins,outs,up.getNumSamples());
  oversampler_->processSamplesDown(block);
 }
};
}
