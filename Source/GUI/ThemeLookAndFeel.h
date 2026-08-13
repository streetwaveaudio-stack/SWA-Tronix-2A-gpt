#pragma once
#include <JuceHeader.h>
#include "ThemeManager.h"

class ThemeLookAndFeel : public juce::LookAndFeel_V4
{
public:
    explicit ThemeLookAndFeel(Theme t) : theme(t) {}
    void drawRotarySlider(juce::Graphics&, int,int,int,int,float,float,float,juce::Slider&) override;
    void drawToggleButton(juce::Graphics&, juce::ToggleButton&, bool, bool) override;
    void drawComboBox(juce::Graphics&, int,int,bool,int,int,int,int,juce::ComboBox&) override;
    void drawLinearSlider(juce::Graphics&, int,int,int,int,float,float,float,juce::Slider::SliderStyle,juce::Slider&) override;
    Theme getTheme() const noexcept { return theme; }
private:
    Theme theme;
};
