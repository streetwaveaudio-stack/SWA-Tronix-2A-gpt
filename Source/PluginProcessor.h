#pragma once
#include <JuceHeader.h>
#include "DSP/OpticalCell.h"
#include "DSP/TubeStage.h"
#include "DSP/OutputTransformer.h"
#include "Presets/PresetManager.h"

class SWATronix2AAudioProcessor : public juce::AudioProcessor
{
public:
    SWATronix2AAudioProcessor();
    ~SWATronix2AAudioProcessor() override = default;

    void prepareToPlay(double, int) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout&) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "SWA Tronix 2A"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;
    OpticalCell optical;
    TubeStage tube;
    OutputTransformer transformer;
    PresetManager presets;

    std::atomic<float> meterGainReductionDb { 0.0f };
    std::atomic<float> meterOutputDb { -60.0f };
    std::atomic<float> vuNeedle { 0.0f };

    void loadFactoryPreset(int index);

    static constexpr const char* gainParam = "gain";
    static constexpr const char* peakParam = "peakReduction";
    static constexpr const char* modeParam = "mode";
    static constexpr const char* meterParam = "meter";
    static constexpr const char* powerParam = "power";
    static constexpr const char* themeParam = "theme";

private:
    double sampleRate = 44100.0;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> gainSmoothed;
    float lastVu = 0.0f;
    float powerWarmup = 1.0f;
    bool previousPower = true;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SWATronix2AAudioProcessor)
};

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
