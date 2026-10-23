#pragma once
#include "Processor.h"
#include <juce_gui_extra/juce_gui_extra.h>

class HeroControl final : public juce::Component,public juce::SettableTooltipClient {
public:
    HeroControl(HeroProcessor&,int index,bool red,bool toggle,int positions);
    void paint(juce::Graphics&)override;
    void mouseDown(const juce::MouseEvent&)override;
    void mouseDrag(const juce::MouseEvent&)override;
    void mouseUp(const juce::MouseEvent&)override;
    void mouseDoubleClick(const juce::MouseEvent&)override;
    void mouseWheelMove(const juce::MouseEvent&,const juce::MouseWheelDetails&)override;
    bool keyPressed(const juce::KeyPress&)override;
    void refresh();
    bool assetsValid()const;
    std::function<void()> contextMenu;
private:
    HeroProcessor& processor;
    juce::RangedAudioParameter* parameter;
    int index,count;
    bool red,toggle,dragging=false;
    float start=0,last=-999,dragSteps=0,wheelSteps=0;
    int lastDragY=0;
    juce::uint32 lastWheelTime=0;
    juce::Image strip;
    void set(float);
    void setTogglePosition(bool right);
    float current()const{return processor.value(index);}
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroControl)
};

class HeroEditor final : public juce::AudioProcessorEditor,private juce::Timer {
public:
    explicit HeroEditor(HeroProcessor&);
    ~HeroEditor()override;
    void paint(juce::Graphics&)override;
    void resized()override;
    void mouseDown(const juce::MouseEvent&)override;
    void showMenu();
    void refreshForTesting(){timerCallback();}
    bool assetsValidForTesting()const;
    bool lcdHasReadoutsForTesting()const;
    const juce::Image& lcdForTesting()const{return lcd;}
    juce::Component* controlForTesting(const juce::String&)const;
private:
    HeroProcessor& processor;
    juce::OwnedArray<HeroControl> controls;
    juce::Image panel,panel1x,meterWindow,lcd;
    std::array<juce::Image,3> ledOff,ledOn;
    juce::Typeface::Ptr lcdFace;
    juce::TooltipWindow tips{this,450};
    juce::ApplicationProperties preferences;
    std::array<float,13> previous{};
    float gr=0,level=0;
    int meterPosition=-999;
    bool first=true;
    void timerCallback()override;
    void renderLCD();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroEditor)
};
