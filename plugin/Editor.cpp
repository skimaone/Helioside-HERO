#include "Editor.h"
#include "HeroAssets.h"
using namespace juce;
namespace {
Image asset(const char* name){int n=0;const auto* data=HeroAssets::getNamedResource(name,n);return data?ImageCache::getFromMemory(data,n):Image{};}
struct Position{int index;float x,y,size;bool red,toggle;int steps;};
constexpr Position positions[]={{0,226,188,96,false,true,2},{5,800,112,120,true,false,16},{6,970,112,120,true,false,16},{7,1140,112,120,true,false,16},
 {1,1340,128,136,false,false,100},{2,1497,128,136,false,false,100},{3,1654,128,136,false,false,100},{4,1811,128,136,false,false,100},{8,1968,128,136,false,false,24},
 {9,2096,40,96,false,true,3},{10,2096,106,96,false,true,2},{11,2096,172,96,false,true,3}};
}
HeroControl::HeroControl(HeroProcessor& p,int id,bool r,bool t,int steps):processor(p),parameter(p.state.getParameter(HeroProcessor::ids[(size_t)id])),index(id),count(steps),red(r),toggle(t){
    setComponentID(HeroProcessor::ids[(size_t)id]);setName(parameter->getName(64));setWantsKeyboardFocus(true);
    strip=asset(toggle?"hero_toggle_strip_png":red?"hero_knob_red_strip_png":"hero_knob_teal_strip_png");refresh();
}
bool HeroControl::assetsValid()const{return strip.isValid()&&strip.getHeight()==strip.getWidth()*(toggle?2:red?16:128);}
void HeroControl::paint(Graphics& g){g.setImageResamplingQuality(Graphics::highResamplingQuality);
    int frame=0;
    if(toggle){const int v=roundToInt(current());frame=index==9?(v==2?1:0):index==11?(v==2?1:0):v;
        if(index==11&&v==1)g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi,getWidth()*.5f,getHeight()*.5f));}
    else {const int frames=red?16:128;frame=jlimit(0,frames-1,red?roundToInt(current()):roundToInt(current()/(count-1)*(frames-1)));}
    const int side=strip.getWidth();g.drawImage(strip,0,0,getWidth(),getHeight(),0,frame*side,side,side);
}
void HeroControl::set(float v){v=red?(float)((roundToInt(v)%count+count)%count):jlimit(0.f,float(count-1),v);parameter->setValueNotifyingHost(parameter->convertTo0to1(v));refresh();}
void HeroControl::setTogglePosition(bool right){set(index==9?(right?2.f:(float)processor.preferredMode()):index==11?(right?2.f:0.f):(right?1.f:0.f));}
void HeroControl::setToneFromPoint(Point<float> point){set(point.y>getHeight()*.65f||std::abs(point.x-getWidth()*.5f)<getWidth()*.17f?1.f:point.x<getWidth()*.5f?0.f:2.f);}
void HeroControl::mouseDown(const MouseEvent& e){if(e.mods.isPopupMenu()){if(contextMenu)contextMenu();return;}grabKeyboardFocus();start=current();dragSteps=wheelSteps=0;lastDragY=0;parameter->beginChangeGesture();dragging=true;}
void HeroControl::mouseDrag(const MouseEvent& e){if(!dragging)return;
    if(toggle){const int dx=e.getDistanceFromDragStartX(),dy=e.getDistanceFromDragStartY();if(index==11&&dy>6&&dy>std::abs(dx)){set(1);return;}if(std::abs(dx)<6)return;setTogglePosition(dx>0);return;}
    const int distance=e.getDistanceFromDragStartY();const float pixels=float(lastDragY-distance);lastDragY=distance;
    const float rate=red?1.f/(e.mods.isShiftDown()?64.f:16.f):float(count-1)/(e.mods.isShiftDown()?1440.f:360.f);
    dragSteps+=pixels*rate;
    if(red){const float step=std::trunc(dragSteps);if(step!=0){dragSteps-=step;set(current()+step);}}
    else {dragSteps=jlimit(-start,float(count-1)-start,dragSteps);set(start+std::round(dragSteps));}
}
void HeroControl::mouseUp(const MouseEvent& e){if(!dragging)return;if(toggle&&e.getDistanceFromDragStart()<4){if(index==11)setToneFromPoint(e.position);else {const bool wasRight=index==9?start==2:start>0;setTogglePosition(!wasRight);}}parameter->endChangeGesture();dragging=false;}
void HeroControl::mouseDoubleClick(const MouseEvent&){if(!dragging)parameter->beginChangeGesture();parameter->setValueNotifyingHost(parameter->getDefaultValue());if(!dragging)parameter->endChangeGesture();start=current();dragSteps=0;lastDragY=0;refresh();}
void HeroControl::mouseWheelMove(const MouseEvent& e,const MouseWheelDetails& wheel){if(wheel.deltaY==0||dragging)return;const auto now=Time::getMillisecondCounter();const float delta=wheel.deltaY;
    if(now-lastWheelTime>400||(wheelSteps*delta<0))wheelSteps=0;lastWheelTime=now;
    const float amount=wheel.isSmooth?delta*(red?2.f:4.f):std::copysign(jmin(1.f,std::abs(delta)/.1f),delta);
    wheelSteps=jlimit(-1.f,1.f,wheelSteps+amount*(e.mods.isShiftDown()?.25f:1.f));const float step=std::trunc(wheelSteps);if(step==0)return;wheelSteps-=step;
    parameter->beginChangeGesture();if(toggle&&index!=11)setTogglePosition(step>0);else set(current()+step);parameter->endChangeGesture();}
bool HeroControl::keyPressed(const KeyPress& key){int step=0;if(key==KeyPress::upKey||key==KeyPress::rightKey)step=1;else if(key==KeyPress::downKey||key==KeyPress::leftKey)step=-1;else return false;parameter->beginChangeGesture();if(index==11){if(key==KeyPress::leftKey)set(0);else if(key==KeyPress::rightKey)set(2);else set(1);}else if(toggle)setTogglePosition(step>0);else set(current()+(float)step);parameter->endChangeGesture();return true;}
void HeroControl::refresh(){const float v=current();if(v==last)return;last=v;String text=parameter->getName(40)+": "+String(roundToInt(v));
    if(index==1)text+=" ("+String(-40.+40.*v/99.,1)+" dBFS reference)";
    if(index==2)text+=" (nominal "+String(.05*std::pow(8000.,v/99.),2)+" ms)";
    if(index==3)text+=" (nominal "+String(50.*std::pow(120.,v/99.),1)+" ms)";
    if(index==8)text=String(.5*v,1)+" dB make-up";
    if(toggle)text=parameter->getName(40)+": "+parameter->getCurrentValueAsText();if(index==11)text+=" (FAT left / NORMAL down / AIR right)";setTooltip(text);repaint();
}
HeroEditor::HeroEditor(HeroProcessor& p,const File& settingsDirectory):AudioProcessorEditor(p),processor(p),panel(asset("faceplate_png")),panel1x(asset("faceplate_1x_png")),meterWindow(asset("meter_window_bg_png")){
    const char* colours[]={"lime","orange","red"};for(int i=0;i<3;++i){ledOff[(size_t)i]=asset((String("led_")+colours[i]+"_off_png").toRawUTF8());ledOn[(size_t)i]=asset((String("led_")+colours[i]+"_on_png").toRawUTF8());}
    lcdFace=Typeface::createSystemTypefaceFor(HeroAssets::DejaVuSansMono_ttf,HeroAssets::DejaVuSansMono_ttfSize);
    for(const auto& pos:positions){auto* c=controls.add(new HeroControl(p,pos.index,pos.red,pos.toggle,pos.steps));c->contextMenu=[this]{showMenu();};addAndMakeVisible(c);}
    PropertiesFile::Options options;options.applicationName="Suntech Audio HERO";options.folderName=settingsDirectory==File{}?"Suntech Audio":settingsDirectory.getFullPathName();options.filenameSuffix="settings";options.osxLibrarySubFolder="Application Support";preferences.setStorageParameters(options);
    setResizable(true,true);getConstrainer()->setFixedAspectRatio(1200./111.);setResizeLimits(900,83,2400,222);
    auto* settings=preferences.getUserSettings();
    // Previous releases could reopen a remembered 200% window as the apparent
    // default. Reset legacy sizing once to the native 1200x111 panel; subsequent
    // user-selected zooms remain persistent, including deliberate HiDPI sizes.
    if(settings->getIntValue("compactSizingVersion",0)<1){settings->setValue("width",jlimit(900,1200,settings->getIntValue("width",1200)));settings->setValue("compactSizingVersion",1);preferences.saveIfNeeded();}
    const int width=jlimit(900,2400,settings->getIntValue("width",1200));setSize(width,roundToInt(width*111./1200.));timerCallback();startTimerHz(30);
}
HeroEditor::~HeroEditor(){stopTimer();preferences.getUserSettings()->setValue("width",getWidth());preferences.saveIfNeeded();}
void HeroEditor::resized(){const float s=getWidth()/2400.f;for(int i=0;i<controls.size();++i){const auto& p=positions[i];controls[i]->setBounds(roundToInt((p.x-p.size*.5f)*s),roundToInt((p.y-p.size*.5f)*s),roundToInt(p.size*s),roundToInt(p.size*s));}}
void HeroEditor::paint(Graphics& g){g.setImageResamplingQuality(Graphics::highResamplingQuality);const float effective=getWidth()/1200.f*g.getInternalContext().getPhysicalPixelScaleFactor();const auto& bg=effective<=1.001f?panel1x:panel;g.addTransform(AffineTransform::scale(getWidth()/2400.f));g.drawImage(bg,0,0,2400,222,0,0,bg.getWidth(),bg.getHeight());
    // Keep the original baked TONE caption, matching MODE and METER exactly.
    g.drawImage(lcd,440,30,200,160,0,0,lcd.getWidth(),lcd.getHeight());g.drawImageAt(meterWindow,1268,34);
    const bool reduction=processor.value(10)<.5f,powered=processor.value(0)>.5f;
    // Complete the off layer before adding overlapping LED glow sprites.
    for(int pass=0;pass<2;++pass)for(int j=0;j<42;++j){const float db=-21.f+j;const int colour=db<0?0:db==0?1:2;const int x=1278+j*17;const bool lit=powered&&(reduction?(db<=0.f&&db>=-gr):(db<=Decibels::gainToDecibels(jmax(level,1e-8f))+18.f));
        if(pass==1&&!lit)continue;const auto& image=pass==0?ledOff[(size_t)colour]:ledOn[(size_t)colour];g.drawImage(image,x,35,34,30,0,0,image.getWidth(),image.getHeight());}
}
void HeroEditor::timerCallback(){bool changed=first;for(size_t i=0;i<previous.size();++i){float v=processor.value((int)i);if(v!=previous[i])changed=true;previous[i]=v;}for(auto* c:controls)c->refresh();if(changed){renderLCD();repaint();}first=false;
    const auto peaks=processor.takePeaks();gr=jmax(peaks[0],gr*float(std::exp(-1./(.3*30.))));const float decay=float(std::exp(-1./(.3*30.)));level=peaks[1]>level?level+(peaks[1]-level)*float(1.-std::exp(-1./(.01*30.))):jmax(peaks[1],level*decay);
    const int pos=roundToInt(processor.value(10)<.5f?gr:(Decibels::gainToDecibels(jmax(level,1e-8f))+18));if(pos!=meterPosition){meterPosition=pos;const float s=getWidth()/2400.f;repaint(roundToInt(1266*s),roundToInt(32*s),roundToInt(755*s),roundToInt(36*s));}
}
void HeroEditor::renderLCD(){
    // These images are sampled on the CPU below. Software backing avoids
    // reading unflushed Direct2D draws and repeated GPU readbacks on Windows.
    Image content(Image::RGB,640,512,true,SoftwareImageType{});
    {Graphics g(content);g.fillAll(Colour(0xff040402));g.addTransform(AffineTransform::scale(4.f));g.setFont(Font(FontOptions(lcdFace).withHeight(15.f)));
    const String rows[]={"T"+String(roundToInt(previous[1])).paddedLeft('0',2),"A"+String(roundToInt(previous[2])).paddedLeft('0',2),"R"+String(roundToInt(previous[3])).paddedLeft('0',2),"S"+String(roundToInt(previous[4])).paddedLeft('0',2),String(previous[8]*.5f,1)+"DB"};
    for(int i=0;i<5;++i){g.setColour(Colour(i==4?0xffbeec96:0xffd6f0a0));g.setFont(Font(FontOptions(lcdFace).withHeight(i==4?11.f:15.f)));g.drawText(rows[i],5,8+22*i,58,19,Justification::centredLeft,false);}
    const int mode=roundToInt(previous[9]);const bool active=previous[0]>.5f&&mode!=2;const String status=active?(mode==0?"LINK":"IN"):"OUT";g.setColour(Colour(!active?0xffe23020:mode==0?0xff50c8e6:0xfff08030));g.setFont(Font(FontOptions(lcdFace).withHeight(15.f)));g.drawText(status,63,1,67,19,Justification::centredRight,false);
    const std::array<float,4> values{{previous[1],previous[2],previous[3],previous[4]}};
    if(values!=curveParameters){displayCurves=hero_display::curves(values[0],values[1],values[2],values[3]);curveParameters=values;}
    auto point=[](double input,double output){return Point<float>((float)(64.+(input+35.)/60.*90.),(float)(122.-(output+35.)/60.*116.));};
    const auto lo=point(-35,-35),hi=point(25,25);g.setColour(Colour(0xffc8e050));g.drawLine({lo,hi},1.f);
    g.setColour(Colour(0xff48c4e0));const auto ref=point(4,4);g.drawLine(ref.x,ref.y,154,ref.y,1.f);
    g.setFont(Font(FontOptions(lcdFace).withHeight(10.f)));g.drawText("0VU",roundToInt(ref.x)-6,roundToInt(ref.y)-16,30,14,Justification::centredLeft,false);
    Path timing,steady;bool begun=false;
    for(const auto& db:displayCurves.timing){const auto pt=point(db.x,active?db.y:db.x);if(!begun){timing.startNewSubPath(pt);begun=true;}else timing.lineTo(pt);}
    begun=false;for(const auto& db:displayCurves.steady){const auto pt=point(db.x,active?db.y:db.x);if(!begun){steady.startNewSubPath(pt);begun=true;}else steady.lineTo(pt);}
    g.setColour(Colour(0xffeda15e).withAlpha(.70f));g.strokePath(timing,PathStrokeType(.85f));
    g.setFont(Font(FontOptions(lcdFace).withHeight(9.f)));g.drawText("1s",134,110,21,13,Justification::centredRight,false);
    g.setColour(Colour(0xffe2f2f0));g.strokePath(steady,PathStrokeType(1.f));
    } // Finish the content draw before downsampling or reading any pixels.
    lcd=hero_display::glass(content,previous[0]>.5f);
}
void HeroEditor::mouseDown(const MouseEvent& e){if(e.mods.isPopupMenu())showMenu();}
void HeroEditor::showMenu(){PopupMenu menu;menu.addSectionHeader("Suntech Audio HERO");menu.addItem(1,"4x oversampling",true,processor.value(12)<.5f);menu.addItem(2,"8x oversampling",true,processor.value(12)>.5f);
    PopupMenu stereo;stereo.addItem(30,"Linked",true,processor.preferredMode()==0);stereo.addItem(31,"Independent L/R",true,processor.preferredMode()==1);menu.addSubMenu("Stereo control",stereo);
    PopupMenu tone;const char* names[]={"FAT","NORMAL","AIR"};for(int i=0;i<3;++i)tone.addItem(40+i,names[i],true,roundToInt(processor.value(11))==i);menu.addSubMenu("Tone",tone);
    menu.addSeparator();for(int i=0;i<5;++i){const int zoom[]={75,100,125,150,200};menu.addItem(10+i,String(zoom[i])+"% interface",true,getWidth()==zoom[i]*12);}menu.addSeparator();menu.addItem(20,"About HERO 0.1.4");
    Component::SafePointer<HeroEditor> self(this);menu.showMenuAsync(PopupMenu::Options().withTargetComponent(this),[self](int choice){if(!self)return;if(choice==1||choice==2){auto* p=self->processor.state.getParameter("oversampling");p->beginChangeGesture();p->setValueNotifyingHost(choice==2?1.f:0.f);p->endChangeGesture();}else if(choice==30||choice==31)self->processor.selectPreferredMode(choice-30);else if(choice>=40&&choice<=42){auto* p=self->processor.state.getParameter("tone");p->beginChangeGesture();p->setValueNotifyingHost(p->convertTo0to1(float(choice-40)));p->endChangeGesture();}else if(choice>=10&&choice<15){const int zoom[]={75,100,125,150,200};int w=zoom[choice-10]*12;self->setSize(w,roundToInt(w*111./1200.));}else if(choice==20)AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,"Suntech Audio HERO 0.1.4","Independent Titan-inspired research model. Based on the supplied v10 DSP, not a circuit-accurate or manufacturer-endorsed emulation.\n\n4x/8x FIR oversampling; fixed compensated latency. Right-click the panel for stereo control, tone, quality and zoom settings.","OK",self);});
}
bool HeroEditor::assetsValidForTesting()const{if(!panel.isValid()||!panel1x.isValid()||!meterWindow.isValid()||!lcdFace||!lcd.isValid())return false;for(int i=0;i<3;++i)if(!ledOff[(size_t)i].isValid()||!ledOn[(size_t)i].isValid())return false;for(auto* c:controls)if(!c->assetsValid())return false;return true;}
bool HeroEditor::lcdHasReadoutsForTesting()const{
    if(!lcd.isValid())return false;int green=0;
    const Image::BitmapData data(lcd,Image::BitmapData::readOnly);
    // Left-column text must contain bright pale-green dots, not just glass.
    const float scale=lcd.getWidth()/400.f;for(int y=roundToInt(16*scale);y<roundToInt(240*scale);++y)for(int x=roundToInt(6*scale);x<roundToInt(146*scale);++x){const auto c=data.getPixelColour(x,y);if(c.getGreen()>90&&c.getGreen()>c.getBlue()*1.15f&&c.getGreen()>c.getRed()*1.02f)++green;}
    return green>80;
}
Component* HeroEditor::controlForTesting(const String& id)const{for(auto* c:controls)if(c->getComponentID()==id)return c;return nullptr;}
