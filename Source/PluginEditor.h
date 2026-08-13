#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/ThemeLookAndFeel.h"
#include "GUI/Views.h"

class SWATronix2AAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit SWATronix2AAudioProcessorEditor(SWATronix2AAudioProcessor&);
    ~SWATronix2AAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override { repaint(); }
    void rebuildTheme();
    void showAbout();
    void applyPreset(int index);

    SWATronix2AAudioProcessor& processor;
    ThemeLookAndFeel photoreal { Theme::Photoreal };
    ThemeLookAndFeel minimal { Theme::Minimal };
    ThemeLookAndFeel neon { Theme::Neon };
    ThemeLookAndFeel aurora { Theme::Aurora };
    ThemeLookAndFeel* activeLF = &photoreal;

    MeterView meter;
    Knob gain, peak;
    juce::ComboBox mode, meterChoice, preset, theme;
    juce::ToggleButton power { "POWER" };
    juce::TextButton info { "i" };
    juce::Label gainLabel, peakLabel, modeLabel, meterLabel, presetLabel, themeLabel, brand, model;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> meterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> themeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SWATronix2AAudioProcessorEditor)
};
