#include "ThemeManager.h"
juce::String ThemeManager::getName(Theme t)
{
    switch (t) { case Theme::Photoreal: return "Photoreal"; case Theme::Minimal: return "Minimal"; case Theme::Neon: return "Neon"; default: return "Aurora"; }
}
