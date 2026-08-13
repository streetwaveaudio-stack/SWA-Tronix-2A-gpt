#include "TubeStage.h"

void TubeStage::prepare(const juce::dsp::ProcessSpec& spec)
{
    oversampling.initProcessing(static_cast<size_t>(spec.maximumBlockSize));
    reset();
}

void TubeStage::reset() { oversampling.reset(); }

void TubeStage::setDrive(float normalized) { drive = juce::jlimit(0.0f, 1.0f, normalized); }

void TubeStage::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    auto up = oversampling.processSamplesUp(block);
    for (size_t c = 0; c < up.getNumChannels(); ++c)
        for (size_t n = 0; n < up.getNumSamples(); ++n)
        {
            float x = up.getChannelPointer(c)[n];
            const float g = 1.0f + drive * 4.0f;
            // Asymmetric soft clipping creates gentle even-order content.
            const float biased = x * g + drive * 0.04f * x * x;
            up.getChannelPointer(c)[n] = 0.93f * std::tanh(biased) + 0.07f * x;
        }
    oversampling.processSamplesDown(block);
}
