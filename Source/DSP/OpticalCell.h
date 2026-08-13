#pragma once
#include <JuceHeader.h>

class OpticalCell
{
public:
    void prepare(double sampleRate);
    void reset();
    float process(float detectorPeak, float peakReduction01, bool limitMode);
    float getGainReductionDb() const noexcept { return grDb; }

private:
    double sr = 44100.0;
    float envelope = 0.0f;
    float opticalState = 0.0f;
    float grDb = 0.0f;

    static float coeff(double ms, double sampleRate) noexcept;
};
