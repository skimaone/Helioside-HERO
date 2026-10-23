#include <juce_audio_utils/juce_audio_utils.h>
#include <thread>
#include <atomic>
#include <cstdio>
#if JUCE_WINDOWS
#include <windows.h>
#endif
using namespace juce;
int failures=0;
void check(bool b,const String& s){std::printf("[%s] %s\n",b?"PASS":"FAIL",s.toRawUTF8());std::fflush(stdout);if(!b)++failures;}
void pump(int ms){auto end=Time::getMillisecondCounter()+ms;do{
#if JUCE_WINDOWS
 MSG m;while(PeekMessageW(&m,nullptr,0,0,PM_REMOVE)){TranslateMessage(&m);DispatchMessageW(&m);}
#else
 MessageManager::getInstance()->runDispatchLoopUntil(2);
#endif
 Thread::sleep(2);
}while(Time::getMillisecondCounter()<end);}
int main(int argc,char**argv){ScopedJuceInitialiser_GUI gui;if(argc!=3)return 2;File bundle=File::getCurrentWorkingDirectory().getChildFile(argv[1]),out=File::getCurrentWorkingDirectory().getChildFile(argv[2]);out.createDirectory();
 VST3PluginFormat format;OwnedArray<PluginDescription> types;format.findAllTypesForFile(types,bundle.getFullPathName());check(types.size()==1,"Built VST3 factory loads exactly one effect");if(types.size()!=1)return 1;
 auto& desc=*types[0];check(desc.name=="Suntech Audio HERO"&&desc.manufacturerName=="Suntech Audio"&&!desc.isInstrument,"Correct native VST3 product/manufacturer identity");String error;auto p=format.createInstanceFromDescription(desc,48000,512,error);check(p!=nullptr,"Native instance initializes: "+error);if(!p)return 1;
 check(p->getMainBusNumInputChannels()==2&&p->getMainBusNumOutputChannels()==2,"Native stereo I/O");check(p->getParameters().size()>=13,"Native parameter export");

 MemoryBlock saved;p->getStateInformation(saved);p->prepareToPlay(48000,512);int latency=p->getLatencySamples();check(latency==65,"Native VST3 reports oversampling latency");
 // Verify the actual dynamically loaded module's POWER bypass, not only a linked processor.
 for(auto* parameter:p->getParameters())if(parameter->getName(64)=="Power")parameter->setValueNotifyingHost(0.f);
 p->releaseResources();p->prepareToPlay(48000,512);AudioBuffer<float> bypass(jmax(2,p->getTotalNumInputChannels()),512);MidiBuffer bypassMidi;std::vector<float> input(4096);
 // The VST3 host delivers queued control changes on process; allow the 20 ms switch fade to settle.
 for(int k=0;k<8;++k){bypass.clear();p->processBlock(bypass,bypassMidi);}float null=0;
 for(int i=0;i<4096;++i)input[(size_t)i]=(float)(.6*std::sin(2*MathConstants<double>::pi*19000*i/48000.));
 for(int k=0;k<8;++k){for(int c=0;c<bypass.getNumChannels();++c)for(int i=0;i<512;++i)bypass.setSample(c,i,c<2?input[(size_t)(k*512+i)]:0.f);p->processBlock(bypass,bypassMidi);for(int c=0;c<2;++c)for(int i=0;i<512;++i){int pos=k*512+i;null=jmax(null,std::abs(bypass.getSample(c,i)-(pos<65?0.f:input[(size_t)(pos-65)])));}}
 std::printf("Actual module bypass maximum residual: %.9g\n",null);check(null==0.f,"Actual VST3 POWER-off high-frequency null is sample-exact at 65 samples");
 p->setStateInformation(saved.getData(),(int)saved.getSize());p->releaseResources();p->prepareToPlay(48000,512);
 int openings=3;
#if JUCE_LINUX
 if(std::getenv("DISPLAY")==nullptr){openings=0;std::printf("[SKIP] Native window lifecycle requires an X display; Windows CI always runs it.\n");}
#endif
 for(int opening=0;opening<openings;++opening){p->setStateInformation(saved.getData(),(int)saved.getSize());std::unique_ptr<AudioProcessorEditor> editor(p->createEditorIfNeeded());check(editor!=nullptr,"Native editor open "+String(opening+1));if(!editor)break;editor->addToDesktop(0);editor->setVisible(true);pump(100);
  std::atomic<bool> stop{false},finite{true};std::atomic<int> count{0};std::thread audio([&]{AudioBuffer<float>b(jmax(2,p->getTotalNumInputChannels()),512);MidiBuffer midi;int position=0;while(!stop.load()){for(int c=0;c<b.getNumChannels();++c)for(int i=0;i<512;++i)b.setSample(c,i,(float)(.45*std::sin(2*MathConstants<double>::pi*997*(position+i)/48000.)));p->processBlock(b,midi);for(int c=0;c<2;++c)for(int i=0;i<512;++i)if(!std::isfinite(b.getSample(c,i)))finite=false;position+=512;++count;Thread::sleep(1);}});
  for(int k=0;k<8;++k){for(auto*q:p->getParameters())if(q->getName(64)=="VCA Colour"||q->getName(64)=="Oversampling")q->setValueNotifyingHost(k%2?1.f:0.f);if(k==3)editor->setSize(900,83);if(k==5)editor->setSize(1200,111);editor->repaint();pump(65);}
  stop=true;audio.join();check(finite.load()&&count.load()>0,"Native GUI paints during live playback and automation");p->setStateInformation(saved.getData(),(int)saved.getSize());pump(40);
  if(opening==0){auto image=editor->createComponentSnapshot(editor->getLocalBounds());auto file=out.getChildFile("native-vst3-editor.png");file.deleteFile();auto s=file.createOutputStream();check(s&&PNGImageFormat().writeImageToStream(image,*s),"Native VST3 screenshot");}
  editor->setVisible(false);editor->removeFromDesktop();editor.reset();pump(30);check(p->getLatencySamples()==latency,"Project recall/reopening preserves latency");
 }p->releaseResources();out.getChildFile("native-result.txt").replaceWithText("Failures: "+String(failures)+"\n");return failures?1:0;}
