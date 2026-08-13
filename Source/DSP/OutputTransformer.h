#pragma once
#include <JuceHeader.h>

class OutputTransformer
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec);
    void reset();
    void setDrive(float normalized);
    void process(juce::AudioBuffer<float>& buffer);
private:
    juce::dsp::Oversampling<float> oversampling { 2, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };
    juce::dsp::StateVariableTPTFilter<float> lowpass;
    float drive = 0.2f;
};
