#pragma once
#include <JuceHeader.h>

enum class Theme { Photoreal = 0, Minimal, Neon, Aurora };

class ThemeManager
{
public:
    static juce::String getName(Theme t);
};
