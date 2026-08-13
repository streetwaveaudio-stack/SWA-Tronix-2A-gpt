#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;
    p.push_back(std::make_unique<AudioParameterFloat>("gain", "Gain", NormalisableRange<float>(-10.0f, 20.0f, 0.01f), 0.0f, "dB"));
    p.push_back(std::make_unique<AudioParameterFloat>("peakReduction", "Peak Reduction", NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.28f));
    p.push_back(std::make_unique<AudioParameterChoice>("mode", "Mode", StringArray { "Compress", "Limit" }, 0));
    p.push_back(std::make_unique<AudioParameterChoice>("meter", "Meter", StringArray { "Output +4", "Gain Reduction" }, 0));
    p.push_back(std::make_unique<AudioParameterBool>("power", "Power", true));
    p.push_back(std::make_unique<AudioParameterChoice>("theme", "Theme", StringArray { "Photoreal", "Minimal", "Neon", "Aurora" }, 0));
    return { p.begin(), p.end() };
}

SWATronix2AAudioProcessor::SWATronix2AAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", AudioChannelSet::stereo(), true)
                                      .withOutput("Output", AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

void SWATronix2AAudioProcessor::prepareToPlay(double sr, int blockSize)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec { sr, static_cast<juce::uint32>(blockSize), static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    optical.prepare(sr);
    tube.prepare(spec);
    transformer.prepare(spec);
    tube.setDrive(0.17f);
    transformer.setDrive(0.12f);
    gainSmoothed.reset(sr, 0.03);
    gainSmoothed.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(parameters.getRawParameterValue(gainParam)->load()));
    lastVu = 0.0f;
    powerWarmup = 1.0f;
    previousPower = true;
}

void SWATronix2AAudioProcessor::releaseResources()
{
    optical.reset();
    tube.reset();
    transformer.reset();
}

bool SWATronix2AAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto out = layouts.getMainOutputChannelSet();
    return out == AudioChannelSet::mono() || out == AudioChannelSet::stereo();
}

void SWATronix2AAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
    const bool powered = parameters.getRawParameterValue(powerParam)->load() > 0.5f;
    const float gainDb = parameters.getRawParameterValue(gainParam)->load();
    const float peakReduction = parameters.getRawParameterValue(peakParam)->load();
    const bool limitMode = parameters.getRawParameterValue(modeParam)->load() > 0.5f;

    if (powered && ! previousPower)
        powerWarmup = 0.0f;
    previousPower = powered;
    if (powered)
        powerWarmup = juce::jmin(1.0f, powerWarmup + static_cast<float>(buffer.getNumSamples() / (0.8 * sampleRate)));

    gainSmoothed.setTargetValue(powered ? Decibels::decibelsToGain(gainDb) : 1.0f);
    tube.setDrive(powered ? juce::jmap(powerWarmup, 0.10f, 0.18f) : 0.0f);
    transformer.setDrive(powered ? juce::jmap(powerWarmup, 0.07f, 0.13f) : 0.0f);

    if (!powered)
    {
        // True bypass behavior with a tiny anti-click smoothing ramp.
        for (int s = 0; s < buffer.getNumSamples(); ++s)
        {
            const float g = gainSmoothed.getNextValue();
            for (int c = 0; c < buffer.getNumChannels(); ++c)
                buffer.setSample(c, s, buffer.getSample(c, s) * g);
        }
        meterGainReductionDb.store(0.0f);
        return;
    }

    float maxOut = 0.0f;
    for (int s = 0; s < buffer.getNumSamples(); ++s)
    {
        float peak = 0.0f;
        for (int c = 0; c < buffer.getNumChannels(); ++c)
            peak = juce::jmax(peak, std::abs(buffer.getSample(c, s)));

        const float gr = optical.process(peak, peakReduction, limitMode);
        const float g = gainSmoothed.getNextValue() * gr;
        for (int c = 0; c < buffer.getNumChannels(); ++c)
        {
            auto* x = buffer.getWritePointer(c);
            x[s] *= g;
            maxOut = juce::jmax(maxOut, std::abs(x[s]));
        }
    }

    tube.process(buffer);
    transformer.process(buffer);

    const float outDb = Decibels::gainToDecibels(juce::jmax(1.0e-7f, maxOut));
    const float grDb = optical.getGainReductionDb();
    meterOutputDb.store(outDb);
    meterGainReductionDb.store(grDb);

    // 300 ms VU integration + damped needle approximation.
    const float target = juce::jmap(juce::jlimit(-24.0f, 12.0f, outDb), -24.0f, 12.0f, 0.0f, 1.0f);
    const float alpha = static_cast<float>(1.0 - std::exp(-static_cast<double>(buffer.getNumSamples()) / (0.300 * sampleRate)));
    lastVu += alpha * (target - lastVu);
    vuNeedle.store(lastVu);
}

void SWATronix2AAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    juce::XmlElement wrapper("SWA_TRONIX_2A_STATE");
    wrapper.setAttribute("version", 1);
    wrapper.addChildElement(xml.release());
    copyXmlToBinary(wrapper, destData);
}

void SWATronix2AAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml == nullptr || !xml->hasTagName("SWA_TRONIX_2A_STATE"))
        return;
    auto* paramsXml = xml->getChildByName(parameters.state.getType());
    if (paramsXml != nullptr)
        parameters.replaceState(juce::ValueTree::fromXml(*paramsXml));
}

void SWATronix2AAudioProcessor::loadFactoryPreset(int index)
{
    if (auto v = presets.getFactoryPreset(index))
    {
        auto set = [this](const char* id, float plainValue)
        {
            if (auto* param = parameters.getParameter(id))
                param->setValueNotifyingHost(param->getNormalisableRange().convertTo0to1(plainValue));
        };
        set(gainParam, static_cast<float>(v->getProperty("gain")));
        set(peakParam, static_cast<float>(v->getProperty("peakReduction")));
        set(modeParam, static_cast<float>(v->getProperty("mode")));
        set(meterParam, static_cast<float>(v->getProperty("meter")));
        set(themeParam, static_cast<float>(v->getProperty("theme")));
        if (auto* power = parameters.getParameter(powerParam))
            power->setValueNotifyingHost(power->getNormalisableRange().convertTo0to1(1.0f));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SWATronix2AAudioProcessor();
}
