#include "Processor.h"
#include "Editor.h"
#define titan original_titan
#include "../research/src/TitanCore.h"
#undef titan
#include "Allocations.h"
#include <cstdio>
#include <vector>
#include <chrono>
using namespace juce;
static int failures=0;
static void check(bool good,const String& text){std::printf("[%s] %s\n",good?"PASS":"FAIL",text.toRawUTF8());std::fflush(stdout);if(!good)++failures;}
static void set(HeroProcessor& p,const char* id,float v){auto* q=p.state.getParameter(id);q->setValueNotifyingHost(q->convertTo0to1(v));}
static void process(HeroProcessor& p,AudioBuffer<float>& b){MidiBuffer m;p.processBlock(b,m);}
static std::vector<float> render(int block,int os=0){HeroProcessor p;set(p,"oversampling",(float)os);set(p,"vcaColour",11);set(p,"dynamicColour",12);set(p,"tone",0);p.prepareToPlay(48000,64);std::vector<float> result(16384);for(int off=0;off<16384;){int n=jmin(block,16384-off);AudioBuffer<float>b(2,n);for(int c=0;c<2;++c)for(int i=0;i<n;++i)b.setSample(c,i,(float)(.5*std::sin(2*titan::pi*(c?137:997)*(off+i)/48000.)));process(p,b);for(int i=0;i<n;++i)result[(size_t)(off+i)]=b.getSample(0,i);off+=n;}return result;}
static void wrappedReference(){
 for(int factor:{4,8}){HeroProcessor p;set(p,"oversampling",factor==4?0.f:1.f);p.prepareToPlay(48000,512);original_titan::Engine e;e.prepare(48000.*factor,2);e.setParameters(original_titan::Parameters{});
 dsp::Oversampling<float> os(2,factor==4?2:3,dsp::Oversampling<float>::filterHalfBandFIREquiripple,true,true);os.initProcessing(512);int padding=p.getLatencySamples()-roundToInt(os.getLatencyInSamples());std::vector<float> history((size_t)jmax(1,padding));int position=0;float maxError=0;
 for(int k=0;k<20;++k){AudioBuffer<float>b(2,512),ref(2,512);for(int c=0;c<2;++c)for(int i=0;i<512;++i)b.setSample(c,i,(float)(.5*std::sin(2*titan::pi*(c?157:997)*(k*512+i)/48000.)));ref.makeCopyOf(b);auto up=os.processSamplesUp(dsp::AudioBlock<float>(ref));const float* in[]={up.getChannelPointer(0),up.getChannelPointer(1)};float* out[]={up.getChannelPointer(0),up.getChannelPointer(1)};e.process(in,out,up.getNumSamples());auto down=dsp::AudioBlock<float>(ref);os.processSamplesDown(down);process(p,b);
 for(int i=0;i<512;++i){float expected=ref.getSample(0,i);if(padding){const float old=history[(size_t)position];history[(size_t)position]=expected;expected=old;position=(position+1)%padding;}maxError=jmax(maxError,std::abs(expected-b.getSample(0,i)));}}
 std::printf("Independent %dx reference wrapper max error: %.9g\n",factor,maxError);check(maxError<2e-6f,"Plugin matches original core inside independently constructed FIR wrapper");}
}
static void processorTests(){
 HeroProcessor p;check(p.getName()=="Suntech Audio HERO"&&p.getParameters().size()==13,"Product identity and thirteen stable host parameters");
 check(p.value(0)==1&&p.value(1)==45&&p.value(2)==30&&p.value(3)==30&&p.value(4)==45&&p.value(6)==15&&p.value(9)==0,"Default values preserve supplied engine calibration and stereo link");
 for(int i=0;i<13;++i)set(p,HeroProcessor::ids[(size_t)i],float(i%2));set(p,"gain",17);set(p,"threshold",73);MemoryBlock state;p.getStateInformation(state);HeroProcessor q;q.setStateInformation(state.getData(),(int)state.getSize());bool same=true;for(int i=0;i<13;++i)same&=p.value(i)==q.value(i);check(same,"All parameter values restore into a new instance");
 q.setStateInformation("junk",4);check(q.value(1)==73,"Invalid state safely ignored");
 HeroProcessor remembered;set(remembered,"mode",1);set(remembered,"mode",2);MemoryBlock memory;remembered.getStateInformation(memory);HeroProcessor restored;restored.setStateInformation(memory.getData(),(int)memory.getSize());check(restored.value(9)==2&&restored.preferredMode()==1,"OUT state remembers independent stereo operation across new-instance recall");
 restored.selectPreferredMode(0);check(restored.value(9)==2&&restored.preferredMode()==0,"Stereo preference can change in menu while OUT stays bypassed");set(restored,"mode",(float)restored.preferredMode());check(restored.value(9)==0,"Front IN restores the selected stereo preference");
 wrappedReference();
 for(int factor=0;factor<2;++factor){const auto a=render(512,factor),b=render(127,factor),c=render(1,factor);float error=0;for(size_t i=0;i<a.size();++i)error=jmax(error,std::abs(a[i]-b[i]),std::abs(a[i]-c[i]));check(error<2e-6f,"Whole-loop audio is invariant across 1/127/512-sample host blocks at "+String(factor?8:4)+"x");}
 for(int mode=0;mode<3;++mode){HeroProcessor bypass;if(mode==0)set(bypass,"power",0);if(mode==1)set(bypass,"mode",2);bypass.prepareToPlay(48000,512);MidiBuffer m;AudioBuffer<float>b(2,4096);b.clear();for(int k=0;k<4;++k){if(mode==2)bypass.processBlockBypassed(b,m);else bypass.processBlock(b,m);}std::vector<float> source(4096);for(int i=0;i<4096;++i){source[(size_t)i]=(float)(.6*std::sin(2*titan::pi*19000*i/48000.));for(int c=0;c<2;++c)b.setSample(c,i,source[(size_t)i]);}if(mode==2)bypass.processBlockBypassed(b,m);else bypass.processBlock(b,m);float error=0;for(int i=0;i<4096;++i)error=jmax(error,std::abs(b.getSample(0,i)-(i<bypass.getLatencySamples()?0.f:source[(size_t)(i-bypass.getLatencySamples())])));check(error==0.f,"Power / OUT / host bypass is a sample-exact delayed raw copy, case "+String(mode));}
 bool finite=true;hero_alloc::calls=0;hero_alloc::watching=true;void* volatile witness=::operator new(31);hero_alloc::watching=false;::operator delete(witness);check(hero_alloc::calls>0,"Allocation counter self-check");hero_alloc::calls=0;
 for(int nch:{1,2})for(double fs:{44100.,48000.,96000.,192000.}){HeroProcessor x;auto layout=x.getBusesLayout();layout.inputBuses.set(0,nch==1?AudioChannelSet::mono():AudioChannelSet::stereo());layout.outputBuses.set(0,layout.inputBuses[0]);check(x.setBusesLayout(layout),"Mono/stereo layout accepted");x.prepareToPlay(fs,64);check(x.getLatencySamples()==65,"Fixed latency is 65 samples at "+String(fs)+" Hz");
 for(int k=0;k<16;++k){set(x,"oversampling",float(k%2));set(x,"tone",float(k%3));set(x,"vcaColour",float(k%16));set(x,"threshold",float(k*6));set(x,"mode",float(k%3));int n=k%4==0?4097:k%4==1?1:k%4==2?127:512;AudioBuffer<float>b(nch,n);for(int c=0;c<nch;++c)for(int i=0;i<n;++i)b.setSample(c,i,(float)(.6*std::sin(2*titan::pi*997*i/fs)));if(k==8)b.setSample(0,0,std::numeric_limits<float>::quiet_NaN());hero_alloc::watching=true;process(x,b);hero_alloc::watching=false;for(int c=0;c<nch;++c)for(int i=0;i<n;++i)finite&=std::isfinite(b.getSample(c,i));}}
 check(finite,"Rates, automation, mono/stereo, oversized/short buffers and NaN input stay finite");check(hero_alloc::calls==0,"No C++ heap allocation during prepared audio callback or oversampling/parameter transitions");
}
static double bin(const std::vector<float>& x,double freq,int fs){double re=0,im=0;const int start=(int)x.size()/2,n=(int)x.size()-start;for(int i=start;i<(int)x.size();++i){re+=x[(size_t)i]*std::cos(2*titan::pi*freq*i/fs);im+=x[(size_t)i]*std::sin(2*titan::pi*freq*i/fs);}return 2*std::hypot(re,im)/n;}
static void analysis(const File& directory){
 String csv="factor,alias_3k_dbfs,relative_to_native_db,realtime_speed\n";constexpr int n=48000;std::vector<float> raw(n);titan::Engine native;native.prepare(48000,1);titan::Parameters params;params.threshold=60;params.attack=0;params.shape=0;params.vcaColor=15;params.dynamicColor=15;native.setParameters(params);std::vector<float> input(n);for(int i=0;i<n;++i)input[(size_t)i]=(float)(.9*std::sin(2*titan::pi*15000*i/48000.));const float* in[]={input.data()};float* out[]={raw.data()};native.process(in,out,n);double baseline=bin(raw,3000,48000);check(baseline>1e-5,"Native alias reference is above the numerical noise floor");
 for(int setting=0;setting<2;++setting){HeroProcessor p;set(p,"oversampling",(float)setting);set(p,"threshold",60);set(p,"attack",0);set(p,"shape",0);set(p,"vcaColour",15);set(p,"dynamicColour",15);p.prepareToPlay(48000,512);std::vector<float> rendered(n);auto start=std::chrono::steady_clock::now();for(int off=0;off<n;){int size=jmin(512,n-off);AudioBuffer<float>b(2,size);for(int c=0;c<2;++c)b.copyFrom(c,0,input.data()+off,size);process(p,b);for(int i=0;i<size;++i)rendered[(size_t)(off+i)]=b.getSample(0,i);off+=size;}double seconds=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();double alias=bin(rendered,3000,48000),relative=20*std::log10(jmax(1e-15,alias)/jmax(1e-15,baseline));csv+=String(setting?8:4)+","+String(20*std::log10(jmax(1e-15,alias)),3)+","+String(relative,3)+","+String(1./seconds,3)+"\n";check(std::isfinite(relative)&&relative<-20,"Actual whole-engine FIR wrapper reduces 15 kHz foldback by at least 20 dB");std::printf("%dx alias %.2f dBFS, rejection %.2f dB; %.2fx realtime (both paths kept warm)\n",setting?8:4,20*std::log10(jmax(1e-15,alias)),relative,1./seconds);}
 directory.getChildFile("alias-cpu.csv").replaceWithText(csv);
}
static MouseEvent event(Component* c,float dy=0,bool shift=false){const auto t=Time::getCurrentTime();return {Desktop::getInstance().getMainMouseSource(),{20.f,20.f-dy},ModifierKeys(ModifierKeys::leftButtonModifier|(shift?ModifierKeys::shiftModifier:0)),1.f,0.f,0.f,0.f,0.f,c,c,t,{20.f,20.f},t,1,dy!=0};}
static void interactionTests(HeroEditor& e,HeroProcessor& p){
 auto* red=e.controlForTesting("vcaColour");auto* teal=e.controlForTesting("threshold");auto* gain=e.controlForTesting("gain");if(!red||!teal||!gain){check(false,"Control sensitivity test components exist");return;}
 struct Gesture final : AudioProcessorParameter::Listener {int begins=0,ends=0;void parameterValueChanged(int,float)override{}void parameterGestureChanged(int,bool on)override{on?++begins:++ends;}} gesture;
 auto* parameter=p.state.getParameter("vcaColour");parameter->addListener(&gesture);
 set(p,"vcaColour",7);red->mouseDown(event(red));red->mouseDrag(event(red,2));check(p.value(5)==7,"Red knob ignores a 2-pixel drag");red->mouseDrag(event(red,15));check(p.value(5)==7,"Red detent requires a full 16 pixels");red->mouseDrag(event(red,16));check(p.value(5)==8,"Red knob advances one detent at 16 pixels");red->mouseDrag(event(red,15));check(p.value(5)==8,"One-pixel reversal does not chatter around a red detent");red->mouseDrag(event(red,0));check(p.value(5)==7,"Full reverse detent returns to previous value");red->mouseUp(event(red));
 check(gesture.begins==1&&gesture.ends==1,"One drag produces one balanced host automation gesture");
 red->mouseDown(event(red));red->mouseDrag(event(red,16,true));check(p.value(5)==7,"Shift-drag keeps red detents still at 16 pixels");red->mouseDrag(event(red,64,true));check(p.value(5)==8,"Shift-drag advances one red detent at 64 pixels");red->mouseUp(event(red,64,true));
 red->mouseDown(event(red));red->mouseDrag(event(red,16));const float v=p.value(5);red->mouseDrag(event(red,16,true));check(p.value(5)==v,"Engaging Shift mid-drag does not jump the knob");red->mouseUp(event(red,16,true));
 set(p,"vcaColour",15);red->mouseDown(event(red));red->mouseDrag(event(red,16));check(p.value(5)==0,"Slow mouse drag preserves red 15-to-0 wrapping");red->mouseUp(event(red,16));
 const int before=gesture.begins;red->mouseDown(event(red));red->mouseDoubleClick(event(red));red->mouseUp(event(red));check(p.value(5)==0&&gesture.begins==before+1&&gesture.ends==gesture.begins,"Double-click resets without nesting host gestures");
 MouseWheelDetails wheel{};wheel.deltaY=.001f;wheel.isSmooth=true;set(p,"vcaColour",7);for(int i=0;i<10;++i)red->mouseWheelMove(event(red),wheel);check(p.value(5)==7,"Tiny trackpad events do not spin the red encoder");wheel.deltaY=.46875f;wheel.isSmooth=false;red->mouseWheelMove(event(red),wheel);check(p.value(5)==8,"An ordinary wheel notch advances one red detent");parameter->removeListener(&gesture);
 set(p,"threshold",0);teal->mouseDown(event(teal));teal->mouseDrag(event(teal,360));check(p.value(1)==99,"Teal range requires a 360-pixel full throw");teal->mouseUp(event(teal,360));
 set(p,"gain",0);gain->mouseDown(event(gain));gain->mouseDrag(event(gain,360));check(p.value(8)==23,"Make-up knob uses the same full throw despite its smaller range");gain->mouseUp(event(gain,360));
 auto* mode=e.controlForTesting("mode");p.selectPreferredMode(1);mode->keyPressed(KeyPress(KeyPress::upKey));check(p.value(9)==2,"Two-position front Mode selects OUT");mode->keyPressed(KeyPress(KeyPress::downKey));check(p.value(9)==1,"Two-position front Mode restores Independent IN");
 auto* tone=e.controlForTesting("tone");set(p,"tone",1);tone->keyPressed(KeyPress(KeyPress::leftKey));check(p.value(11)==0,"Tone left selects FAT");tone->keyPressed(KeyPress(KeyPress::rightKey));check(p.value(11)==2,"Tone right selects AIR");tone->keyPressed(KeyPress(KeyPress::downKey));check(p.value(11)==1,"Tone down selects NORMAL");tone->mouseDoubleClick(event(tone));check(p.value(11)==1,"NORMAL remains available as the original default");

 auto clickTone=[&](float x,float y){const auto t=Time::getCurrentTime();const Point<float> point{x*tone->getWidth(),y*tone->getHeight()};MouseEvent ev{Desktop::getInstance().getMainMouseSource(),point,ModifierKeys(ModifierKeys::leftButtonModifier),1.f,0.f,0.f,0.f,0.f,tone,tone,t,point,t,1,false};tone->mouseDown(ev);tone->mouseUp(ev);};
 clickTone(.2f,.4f);check(p.value(11)==0,"Clicking left throw selects FAT");clickTone(.8f,.4f);check(p.value(11)==2,"Clicking right throw selects AIR");clickTone(.5f,.85f);check(p.value(11)==1,"Clicking lower throw selects NORMAL");
 tone->mouseDown(event(tone));tone->mouseDrag(event(tone,-12));tone->mouseUp(event(tone,-12));check(p.value(11)==1,"Downward toggle drag selects NORMAL");
 set(p,"threshold",45);set(p,"gain",0);p.selectPreferredMode(0);set(p,"vcaColour",0);
}
static void editorTests(const File& out){HeroProcessor p;p.prepareToPlay(48000,512);std::unique_ptr<AudioProcessorEditor> base(p.createEditorIfNeeded());auto* e=dynamic_cast<HeroEditor*>(base.get());check(e&&e->assetsValidForTesting(),"Every supplied control image, panel, meter and embedded LCD font decodes");if(!e)return;
#if JUCE_WINDOWS
 e->addToDesktop(0);e->setVisible(true);
#endif
 interactionTests(*e,p);
 auto snap=[&](const String& name){e->refreshForTesting();if(p.value(0)>.5f)check(e->lcdHasReadoutsForTesting(),"LCD contains visible parameter readouts: "+name);auto file=out.getChildFile(name+".png");file.deleteFile();auto stream=file.createOutputStream();auto image=e->createComponentSnapshot(e->getLocalBounds());check(stream&&PNGImageFormat().writeImageToStream(image,*stream),"Editor capture "+name);};
 for(int width:{900,1200,1500,1800,2400}){e->setSize(width,roundToInt(width*111./1200.));snap("hero-width-"+String(width));}
 {auto file=out.getChildFile("hero-lcd-800x640.png");file.deleteFile();auto stream=file.createOutputStream();check(stream&&PNGImageFormat().writeImageToStream(e->lcdForTesting(),*stream),"Live LCD high-resolution capture");}

 set(p,"tone",0);snap("hero-tone-fat");set(p,"tone",2);snap("hero-tone-air");set(p,"tone",1);snap("hero-tone-normal");
 set(p,"threshold",45);set(p,"shape",45);set(p,"attack",0);set(p,"release",30);snap("hero-fast-attack");const auto fast=e->curvesForTesting();
 set(p,"attack",99);snap("hero-slow-attack");const auto slow=e->curvesForTesting();float attackDelta=0;bool sameSteady=true;
 for(size_t i=0;i<fast.timing.size()/2;++i)attackDelta=jmax(attackDelta,std::abs(fast.timing[i].y-slow.timing[i].y));
 for(size_t i=0;i<fast.steady.size();++i)sameSteady&=fast.steady[i]==slow.steady[i];
 check(attackDelta>.1f&&sameSteady,"Attack changes the clean-engine timing trajectory without changing the static knee");
 set(p,"attack",30);set(p,"release",0);snap("hero-fast-release");const auto shortRelease=e->curvesForTesting();
 set(p,"release",99);snap("hero-slow-release");const auto longRelease=e->curvesForTesting();float releaseDelta=0;
 for(size_t i=shortRelease.timing.size()/2;i<shortRelease.timing.size();++i)releaseDelta=jmax(releaseDelta,std::abs(shortRelease.timing[i].y-longRelease.timing[i].y));
 check(releaseDelta>.01f,"Release changes the falling clean-engine timing trajectory");
 bool finite=true;for(const auto& curve:{fast,slow,shortRelease,longRelease})for(const auto& point:curve.timing)finite&=std::isfinite(point.x)&&std::isfinite(point.y)&&point.y<=point.x+.001f;
 check(finite,"Timing trajectories stay finite and show attenuation, not invented gain");
 std::printf("Display attack trajectory change %.4f dB; release change %.4f dB\n",attackDelta,releaseDelta);
 set(p,"attack",30);set(p,"release",30);
 auto* red=e->controlForTesting("vcaColour");check(red!=nullptr,"Endless red control attached");if(red){set(p,"vcaColour",15);red->keyPressed(KeyPress(KeyPress::upKey));check(p.value(5)==0,"Red control wraps 15 to 0");red->keyPressed(KeyPress(KeyPress::downKey));check(p.value(5)==15,"Red control wraps 0 to 15");}
 for(const char* id:{"threshold","attack","release","shape","gain","mode","meterMode","tone","power"}){auto* c=e->controlForTesting(id);check(c!=nullptr,String("Live control attached: ")+id);if(c){const float before=p.state.getRawParameterValue(id)->load();c->keyPressed(KeyPress(String(id)=="tone"?KeyPress::leftKey:before>0?KeyPress::downKey:KeyPress::upKey));check(p.state.getRawParameterValue(id)->load()!=before,String("Control updates host parameter: ")+id);}}
 e->setSize(1200,111);set(p,"power",1);set(p,"mode",0);snap("hero-link");set(p,"mode",1);snap("hero-in");set(p,"mode",2);snap("hero-out");set(p,"mode",0);set(p,"power",0);snap("hero-power-off");set(p,"power",1);set(p,"threshold",10);set(p,"attack",0);set(p,"shape",0);set(p,"vcaColour",15);set(p,"dynamicColour",15);AudioBuffer<float>b(2,512);for(int k=0;k<30;++k){for(int c=0;c<2;++c)for(int i=0;i<512;++i)b.setSample(c,i,(float)(.7*std::sin(2*titan::pi*997*(k*512+i)/48000.)));process(p,b);}snap("hero-compressing-gr");set(p,"meterMode",1);snap("hero-compressing-level");e->removeFromDesktop();e->setSize(1200,111);
}
int main(int argc,char**argv){ScopedJuceInitialiser_GUI gui;if(argc!=3)return 2;File out=File::getCurrentWorkingDirectory().getChildFile(argv[2]);out.createDirectory();String mode(argv[1]);if(mode=="editor")editorTests(out);else if(mode=="analysis")analysis(out);else processorTests();out.getChildFile("result.txt").replaceWithText("Failures: "+String(failures)+"\n");return failures?1:0;}
