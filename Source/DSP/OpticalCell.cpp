#include "OpticalCell.h"

float OpticalCell::coeff(double ms, double sampleRate) noexcept
{
    return static_cast<float>(std::exp(-1.0 / (0.001 * ms * sampleRate)));
}

void OpticalCell::prepare(double sampleRate)
{
    sr = sampleRate;
    reset();
}

void OpticalCell::reset()
{
    envelope = 0.0f;
    opticalState = 0.0f;
    grDb = 0.0f;
}

float OpticalCell::process(float detectorPeak, float peakReduction01, bool limitMode)
{
    const float driveDb = juce::jmap(peakReduction01, 0.0f, 1.0f, -20.0f, +24.0f);
    const float driven = juce::Decibels::decibelsToGain(driveDb) * detectorPeak;

    // The light-cell attack is fast-ish (~10 ms), while release is program/memory dependent.
    const float attack = coeff(10.0, sr);
    if (driven > envelope)
        envelope = attack * envelope + (1.0f - attack) * driven;
    else
    {
        const float held = juce::jlimit(0.0f, 1.0f, opticalState);
        const float slowMs = juce::jmap(held, 450.0f, 4200.0f);
        const float rel = coeff(slowMs, sr);
        envelope = rel * envelope + (1.0f - rel) * driven;
    }

    // T4-style memory: optical state decays in two stages and remembers sustained illumination.
    const float fastR = coeff(65.0, sr);
    const float slowR = coeff(1800.0, sr);
    const float target = juce::jlimit(0.0f, 1.0f, envelope * 0.65f + driven * 0.35f);
    const float recoverFast = fastR * opticalState + (1.0f - fastR) * target;
    const float recoverSlow = slowR * opticalState + (1.0f - slowR) * target;
    const float levelDuration = juce::jlimit(0.0f, 1.0f, envelope);
    opticalState = juce::jlimit(0.0f, 2.0f, juce::jmap(levelDuration, recoverFast, recoverSlow));

    const float excited = juce::jlimit(0.0f, 2.5f, opticalState);
    const float reductionStrength = juce::jmap(peakReduction01, 0.0f, 1.0f, 0.0f, limitMode ? 22.0f : 14.0f);
    const float knee = juce::jmap(peakReduction01, 0.0f, 1.0f, 10.0f, 3.0f);

    // Smooth knee and level-dependent ratio. Limit mode approaches a high ratio for peaks.
    const float xDb = juce::Decibels::gainToDecibels(juce::jmax(1.0e-6f, excited));
    const float soft = 1.0f / (1.0f + std::exp(-(xDb + knee) / 3.0f));
    const float ratio = limitMode ? juce::jmap(soft, 3.5f, 12.0f) : juce::jmap(soft, 2.4f, 3.6f);
    const float compressed = (xDb > 0.0f) ? xDb - (xDb * (1.0f - 1.0f / ratio) * soft) : 0.0f;
    const float rawGr = juce::jlimit(0.0f, reductionStrength, reductionStrength * juce::jlimit(0.0f, 1.0f, compressed / 18.0f));

    grDb = 0.996f * grDb + 0.004f * (-rawGr);
    return juce::Decibels::decibelsToGain(grDb);
}

