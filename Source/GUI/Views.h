#pragma once
#include <JuceHeader.h>
#include "ThemeLookAndFeel.h"
#include "../PluginProcessor.h"

class Knob : public juce::Slider
{
public:
    Knob() { setSliderStyle(RotaryHorizontalVerticalDrag); setTextBoxStyle(NoTextBox, false, 0, 0); }
};

class MeterView : public juce::Component, private juce::Timer
{
public:
    explicit MeterView(SWATronix2AAudioProcessor& p) : processor(p) { startTimerHz(30); }
    void paint(juce::Graphics&) override;
private:
    void timerCallback() override { repaint(); }
    SWATronix2AAudioProcessor& processor;
};

class AboutView : public juce::Component
{
public:
    AboutView();
    void paint(juce::Graphics&) override;
};
