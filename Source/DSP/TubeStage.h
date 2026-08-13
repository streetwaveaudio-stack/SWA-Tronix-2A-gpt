#pragma once
#include <JuceHeader.h>

class TubeStage
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setDrive(float normalized);
    void process(juce::AudioBuffer<float>& buffer);
private:
    juce::dsp::Oversampling<float> oversampling { 2, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };
    float drive = 0.25f;
};
