#pragma once
#include <JuceHeader.h>

struct FactoryPreset
{
    juce::String name;
    float gainDb = 0.0f;
    float peakReduction = 0.25f;
    int mode = 0;
    int theme = 0;
};

class PresetManager
{
public:
    PresetManager();
    const std::vector<FactoryPreset>& getFactoryPresets() const noexcept { return presets; }
    std::optional<juce::ValueTree> getFactoryPreset(int index) const;
    juce::StringArray getNames() const;
    juce::String toXmlString(const FactoryPreset&) const;

private:
    std::vector<FactoryPreset> presets;
};
