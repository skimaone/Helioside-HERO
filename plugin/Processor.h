#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "TitanCore.h"

class HeroProcessor final : public juce::AudioProcessor {
public:
    HeroProcessor();
    const juce::String getName() const override { return "Suntech Audio HERO"; }
    void prepareToPlay(double, int) override;
    void releaseResources() override { ready=false; }
    void reset() override;
    void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&) override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    bool hasEditor() const override {return true;}
    juce::AudioProcessorEditor* createEditor() override;
    bool acceptsMidi() const override {return false;}
    bool producesMidi() const override {return false;}
    double getTailLengthSeconds() const override {return 0.;}
    int getNumPrograms() override {return 1;}
    int getCurrentProgram() override {return 0;}
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override {return {};}
    void changeProgramName(int,const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*,int) override;
    juce::AudioProcessorValueTreeState state;
    static constexpr std::array<const char*,13> ids{{"power","threshold","attack","release","shape","vcaColour","parallelMix","dynamicColour","gain","mode","meterMode","tone","oversampling"}};
    float value(int i) const noexcept {return values[(size_t)i]->load(std::memory_order_relaxed);}
    std::array<float,2> takePeaks() noexcept {return {grPeak.exchange(0),outputPeak.exchange(0)};}
    static juce::AudioProcessorValueTreeState::ParameterLayout layout();
private:
    static constexpr int quantum=8,delayCapacity=512;
    struct Delay {
        std::array<std::array<float,delayCapacity>,2> data{};
        int position=0,length=0;
        void clear() noexcept {for(auto& c:data)c.fill(0);position=0;}
        void process(juce::AudioBuffer<float>&,int,int) noexcept;
    };
    struct Path {
        titan::Engine engine;
        std::unique_ptr<juce::dsp::Oversampling<float>> os;
        juce::AudioBuffer<float> input,output;
        Delay padding;
        void process(int,int) noexcept;
    };
    std::array<Path,2> paths;
    std::array<std::atomic<float>*,13> values{};
    std::array<juce::SmoothedValue<double>,11> controls;
    juce::SmoothedValue<float> wet,osMix;
    juce::AudioBuffer<float> dry;
    Delay dryDelay;
    int channels=2,controlLeft=0;
    bool ready=false;
    std::atomic<float> grPeak{0},outputPeak{0};
    std::array<double,11> targets() const noexcept;
    void setTargets() noexcept;
    void updateControls() noexcept;
    void process(juce::AudioBuffer<float>&,bool);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeroProcessor)
};
