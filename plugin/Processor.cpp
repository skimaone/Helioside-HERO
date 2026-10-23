#include "Processor.h"
#include "Editor.h"
using namespace juce;
AudioProcessorValueTreeState::ParameterLayout HeroProcessor::layout(){
    AudioProcessorValueTreeState::ParameterLayout l;
    auto integer=[&](const char* id,const char* name,int hi,int def){l.add(std::make_unique<AudioParameterInt>(ParameterID{id,1},name,0,hi,def));};
    auto choice=[&](const char* id,const char* name,StringArray items,int def){l.add(std::make_unique<AudioParameterChoice>(ParameterID{id,1},name,items,def));};
    l.add(std::make_unique<AudioParameterBool>(ParameterID{"power",1},"Power",true));
    integer("threshold","Threshold",99,45);integer("attack","Attack",99,30);integer("release","Release",99,30);integer("shape","Shape",99,45);
    integer("vcaColour","VCA Colour",15,0);integer("parallelMix","Parallel Mix",15,15);integer("dynamicColour","Dynamic Colour",15,0);integer("gain","Make-up Gain",23,0);
    choice("mode","Mode",{"LINK","IN","OUT"},0);choice("meterMode","Meter",{"GR","LEVEL"},0);
    choice("tone","Tone",{"FAT","NORMAL","AIR"},1);choice("oversampling","Oversampling",{"4x","8x"},0);return l;
}
HeroProcessor::HeroProcessor():AudioProcessor(BusesProperties().withInput("Input",AudioChannelSet::stereo(),true).withOutput("Output",AudioChannelSet::stereo(),true)),state(*this,nullptr,"SUNTECH_HERO",layout()){
    for(size_t i=0;i<ids.size();++i)values[i]=state.getRawParameterValue(ids[i]);
    state.addParameterListener("mode",this);
}
HeroProcessor::~HeroProcessor(){state.removeParameterListener("mode",this);}
void HeroProcessor::parameterChanged(const String&,float v){const int mode=roundToInt(v);if(mode<2)engagedMode.store(jlimit(0,1,mode));}
void HeroProcessor::selectPreferredMode(int mode){mode=jlimit(0,1,mode);const bool changed=engagedMode.exchange(mode)!=mode;
    if(value(9)<1.5f){auto* p=state.getParameter("mode");p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1((float)mode));p->endChangeGesture();}
    else if(changed)updateHostDisplay(ChangeDetails{}.withNonParameterStateChanged(true));
}
bool HeroProcessor::isBusesLayoutSupported(const BusesLayout& b)const{
    const auto in=b.getMainInputChannelSet();return (in==AudioChannelSet::mono()||in==AudioChannelSet::stereo())&&in==b.getMainOutputChannelSet();
}
std::array<double,11> HeroProcessor::targets()const noexcept {
    const auto safe=[&](int i){const float v=value(i);return std::isfinite(v)?double(v):0.;};
    return {{safe(1),safe(2),safe(3),safe(4),safe(5),safe(6),safe(7),safe(8),safe(11)==0?1.:0.,safe(11)==2?1.:0.,safe(9)==0?1.:0.}};
}
void HeroProcessor::setTargets()noexcept{const auto t=targets();for(size_t i=0;i<t.size();++i)controls[i].setTargetValue(t[i]);}
void HeroProcessor::updateControls()noexcept{
    std::array<double,11> v{};for(size_t i=0;i<v.size();++i)v[i]=controls[i].skip(quantum);
    titan::Parameters p;p.threshold=v[0];p.attack=v[1];p.release=v[2];p.shape=v[3];p.vcaColor=v[4];p.parallelMix=v[5];p.dynamicColor=v[6];p.makeupStep=v[7];p.fatWeight=v[8];p.airWeight=v[9];p.linkAmount=v[10];
    for(auto& path:paths)path.engine.setParameters(p);
}
void HeroProcessor::Delay::process(AudioBuffer<float>& b,int n,int channels)noexcept{
    if(length==0)return;
    for(int i=0;i<n;++i){for(int c=0;c<channels;++c){auto x=b.getSample(c,i);b.setSample(c,i,data[(size_t)c][(size_t)position]);data[(size_t)c][(size_t)position]=x;}if(++position==length)position=0;}
}
void HeroProcessor::Path::process(int n,int channels)noexcept{
    const auto block=dsp::AudioBlock<float>(input).getSubBlock(0,(size_t)n);auto up=os->processSamplesUp(block);
    const float* ins[]={up.getChannelPointer(0),channels==2?up.getChannelPointer(1):nullptr};float* outs[]={up.getChannelPointer(0),channels==2?up.getChannelPointer(1):nullptr};
    engine.process(ins,outs,up.getNumSamples());auto dest=dsp::AudioBlock<float>(output).getSubBlock(0,(size_t)n);os->processSamplesDown(dest);padding.process(output,n,channels);
}
void HeroProcessor::prepareToPlay(double fs,int){
    ready=false;channels=getMainBusNumInputChannels();int latency=0;fs=jmax(8000.,fs);
    for(int k=0;k<2;++k){auto& p=paths[(size_t)k];p.os=std::make_unique<dsp::Oversampling<float>>((size_t)channels,(size_t)(2+k),dsp::Oversampling<float>::filterHalfBandFIREquiripple,true,true);p.os->initProcessing(quantum);p.input.setSize(channels,quantum);p.output.setSize(channels,quantum);p.engine.prepare(fs*(4<<k),channels);latency=jmax(latency,roundToInt(p.os->getLatencyInSamples()));}
    jassert(latency<delayCapacity);for(auto& p:paths){p.padding.length=latency-roundToInt(p.os->getLatencyInSamples());p.padding.clear();}
    dry.setSize(channels,quantum);dryDelay.length=latency;dryDelay.clear();setLatencySamples(latency);
    const auto t=targets();for(size_t i=0;i<t.size();++i){controls[i].reset(fs,.020);controls[i].setCurrentAndTargetValue(t[i]);}
    wet.reset(fs,.020);wet.setCurrentAndTargetValue(value(0)>.5f&&value(9)<1.5f?1.f:0.f);
    osMix.reset(fs,.020);osMix.setCurrentAndTargetValue(value(12)>.5f?1.f:0.f);controlLeft=0;grPeak=outputPeak=0;ready=true;
}
void HeroProcessor::reset(){for(auto& p:paths){p.engine.reset();if(p.os)p.os->reset();p.padding.clear();}dryDelay.clear();controlLeft=0;grPeak=outputPeak=0;}
void HeroProcessor::processBlock(AudioBuffer<float>& b,MidiBuffer&){process(b,false);}
void HeroProcessor::processBlockBypassed(AudioBuffer<float>& b,MidiBuffer&){process(b,true);}
void HeroProcessor::process(AudioBuffer<float>& b,bool bypass){
    ScopedNoDenormals denormals;if(!ready||b.getNumChannels()<channels){b.clear();return;}if(b.getNumSamples()==0)return;
    setTargets();wet.setTargetValue(!bypass&&value(0)>.5f&&value(9)<1.5f?1.f:0.f);osMix.setTargetValue(value(12)>.5f?1.f:0.f);
    float blockGr=0,blockPeak=0;
    for(int offset=0;offset<b.getNumSamples();){
        if(controlLeft==0){updateControls();controlLeft=quantum;}const int n=jmin(controlLeft,b.getNumSamples()-offset);
        for(int c=0;c<channels;++c)for(int i=0;i<n;++i){float x=b.getSample(c,offset+i);if(!std::isfinite(x))x=0;dry.setSample(c,i,x);for(auto& p:paths)p.input.setSample(c,i,x);}
        dryDelay.process(dry,n,channels);for(auto& p:paths)p.process(n,channels);
        blockGr=jmax(blockGr,(float)jmax(paths[0].engine.gainReductionDb(),paths[1].engine.gainReductionDb()));
        for(int i=0;i<n;++i){const float amount=wet.getNextValue(),osAmount=osMix.getNextValue();for(int c=0;c<channels;++c){const float raw=dry.getSample(c,i);float y=raw;if(amount>0.f){const float a=paths[0].output.getSample(c,i),z=paths[1].output.getSample(c,i);y=raw+amount*(a+osAmount*(z-a)-raw);}if(!std::isfinite(y))y=0;b.setSample(c,offset+i,y);blockPeak=jmax(blockPeak,std::abs(y));}}
        offset+=n;controlLeft-=n;
    }
    const auto publish=[](std::atomic<float>& dest,float v){float old=dest.load(std::memory_order_relaxed);while(old<v&&!dest.compare_exchange_weak(old,v,std::memory_order_relaxed)){};};
    publish(grPeak,wet.getCurrentValue()>0.f?blockGr:0.f);publish(outputPeak,blockPeak);
    for(int c=channels;c<b.getNumChannels();++c)b.clear(c,0,b.getNumSamples());
}
void HeroProcessor::getStateInformation(MemoryBlock& b){auto xml=state.copyState().createXml();xml->setAttribute("formatVersion",1);xml->setAttribute("preferredMode",preferredMode());copyXmlToBinary(*xml,b);}
void HeroProcessor::setStateInformation(const void* data,int size){
    if(auto xml=getXmlFromBinary(data,size);xml&&xml->hasTagName("SUNTECH_HERO")&&xml->getIntAttribute("formatVersion",1)<=1){auto tree=ValueTree::fromXml(*xml);
        for(int i=tree.getNumChildren();--i>=0;){auto node=tree.getChild(i);auto* p=state.getParameter(node["id"].toString());if(!p){tree.removeChild(i,nullptr);continue;}float v=(float)node["value"];if(!std::isfinite(v))v=p->convertFrom0to1(p->getDefaultValue());v=p->convertFrom0to1(jlimit(0.f,1.f,p->convertTo0to1(v)));node.setProperty("value",v,nullptr);}state.replaceState(tree);engagedMode.store(value(9)<1.5f?roundToInt(value(9)):jlimit(0,1,xml->getIntAttribute("preferredMode",0)));}
}
AudioProcessorEditor* HeroProcessor::createEditor(){return new HeroEditor(*this);}
AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new HeroProcessor;}
