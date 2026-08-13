#include "OutputTransformer.h"

void OutputTransformer::prepare(const juce::dsp::ProcessSpec& spec)
{
    oversampling.initProcessing(static_cast<size_t>(spec.maximumBlockSize));
    lowpass.prepare(spec);
    lowpass.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    lowpass.setCutoffFrequency(19000.0f);
    lowpass.setResonance(0.15f);
    reset();
}

void OutputTransformer::reset()
{
    oversampling.reset();
    lowpass.reset();
}

void OutputTransformer::setDrive(float normalized) { drive = juce::jlimit(0.0f, 1.0f, normalized); }

void OutputTransformer::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    auto up = oversampling.processSamplesUp(block);
    for (size_t c = 0; c < up.getNumChannels(); ++c)
        for (size_t n = 0; n < up.getNumSamples(); ++n)
        {
            auto* p = up.getChannelPointer(c);
            const float x = p[n];
            // Mild transformer-like saturation plus asymmetric low-frequency memory cue.
            const float harmonic = x + drive * (0.10f * x * x - 0.015f * x * x * x);
            p[n] = std::tanh(harmonic * (1.0f + drive * 0.75f)) * 0.985f;
        }
    oversampling.processSamplesDown(block);

    // Gentle HF smoothing after the non-linearity.
    lowpass.process(juce::dsp::ProcessContextReplacing<float>(block));
}
