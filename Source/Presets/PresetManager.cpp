#include "PresetManager.h"
#include "../PluginProcessor.h"

PresetManager::PresetManager()
{
    auto xml = juce::parseXML(juce::String::fromUTF8(BinaryData::FactoryPresets_xml, BinaryData::FactoryPresets_xmlSize));
    if (xml == nullptr)
        return;

    forEachXmlChildElement(*xml, child)
    {
        if (! child->hasTagName("PRESET"))
            continue;

        FactoryPreset p;
        p.name = child->getStringAttribute("name");
        p.gainDb = (float) child->getDoubleAttribute("gain");
        p.peakReduction = (float) child->getDoubleAttribute("peakReduction");
        p.mode = child->getIntAttribute("mode");
        p.theme = child->getIntAttribute("theme");
        presets.push_back(p);
    }
}

std::optional<juce::ValueTree> PresetManager::getFactoryPreset(int index) const
{
    if (index < 0 || index >= static_cast<int>(presets.size()))
        return std::nullopt;

    const auto& p = presets[static_cast<size_t>(index)];
    juce::ValueTree t("PARAMETERS");
    t.setProperty("gain", p.gainDb, nullptr);
    t.setProperty("peakReduction", p.peakReduction, nullptr);
    t.setProperty("mode", p.mode, nullptr);
    t.setProperty("theme", p.theme, nullptr);
    t.setProperty("meter", 0, nullptr);
    t.setProperty("power", true, nullptr);
    return t;
}

juce::StringArray PresetManager::getNames() const
{
    juce::StringArray names;
    for (const auto& p : presets)
        names.add(p.name);
    return names;
}

juce::String PresetManager::toXmlString(const FactoryPreset& p) const
{
    juce::XmlElement xml("PRESET");
    xml.setAttribute("name", p.name);
    xml.setAttribute("gain", p.gainDb);
    xml.setAttribute("peakReduction", p.peakReduction);
    xml.setAttribute("mode", p.mode);
    xml.setAttribute("theme", p.theme);
    return xml.toString();
}

