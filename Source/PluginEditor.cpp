#include "PluginEditor.h"

SWATronix2AAudioProcessorEditor::SWATronix2AAudioProcessorEditor(SWATronix2AAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p), meter(p)
{
    setOpaque(true);
    setResizable(true, true);
    setResizeLimits(640, 420, 1500, 1000);
    setSize(920, 610);

    mode.addItem("Compress", 1); mode.addItem("Limit", 2);
    meterChoice.addItem("Output +4", 1); meterChoice.addItem("Gain Reduction", 2);
    preset.addItemList(processor.presets.getNames(), 1);
    theme.addItem("Photoreal", 1); theme.addItem("Minimal", 2); theme.addItem("Neon", 3); theme.addItem("Aurora", 4);

    gain.setRange(-10.0, 20.0, 0.01); gain.setTextValueSuffix(" dB");
    peak.setRange(0.0, 1.0, 0.001); peak.setNumDecimalPlacesToDisplay(2);

    addAndMakeVisible(meter); addAndMakeVisible(gain); addAndMakeVisible(peak);
    addAndMakeVisible(mode); addAndMakeVisible(meterChoice); addAndMakeVisible(preset); addAndMakeVisible(theme);
    addAndMakeVisible(power); addAndMakeVisible(info);

    auto setupLabel = [&](juce::Label& l, const juce::String& text, bool big=false) { l.setText(text, juce::dontSendNotification); l.setJustificationType(juce::Justification::centred); l.setFont(juce::Font(big ? 24.0f : 13.0f, big ? juce::Font::bold : juce::Font::plain)); addAndMakeVisible(l); };
    setupLabel(brand, "StreetWave Audio", true);
    setupLabel(model, "SWA TRONIX 2A");
    setupLabel(gainLabel, "GAIN"); setupLabel(peakLabel, "PEAK REDUCTION"); setupLabel(modeLabel, "MODE"); setupLabel(meterLabel, "METER"); setupLabel(presetLabel, "PRESET"); setupLabel(themeLabel, "THEME");

    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, SWATronix2AAudioProcessor::gainParam, gain);
    peakAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processor.parameters, SWATronix2AAudioProcessor::peakParam, peak);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, SWATronix2AAudioProcessor::modeParam, mode);
    meterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, SWATronix2AAudioProcessor::meterParam, meterChoice);
    powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(processor.parameters, SWATronix2AAudioProcessor::powerParam, power);
    themeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(processor.parameters, SWATronix2AAudioProcessor::themeParam, theme);

    preset.onChange = [this] { applyPreset(preset.getSelectedItemIndex()); };
    theme.onChange = [this] { rebuildTheme(); };
    info.onClick = [this] { showAbout(); };

    rebuildTheme();
    startTimerHz(12);
}

void SWATronix2AAudioProcessorEditor::paint(juce::Graphics& g)
{
    const auto t = static_cast<Theme>((int) processor.parameters.getRawParameterValue(SWATronix2AAudioProcessor::themeParam)->load());
    if (t == Theme::Photoreal)
    {
        juce::ColourGradient metal(juce::Colour(0xffc8c4bb), 0, 0, juce::Colour(0xfff1eee8), 0, (float)getHeight(), false);
        g.setGradientFill(metal); g.fillAll();
        g.setColour(juce::Colours::black.withAlpha(0.10f));
        for (int y=0; y<getHeight(); y+=4) g.drawHorizontalLine(y, 0, (float)getWidth());
    }
    else if (t == Theme::Minimal)
    {
        g.fillAll(juce::Colour(0xfff3f5f7));
    }
    else if (t == Theme::Neon)
    {
        g.fillAll(juce::Colour(0xff0b0811));
        g.setColour(juce::Colour(0xff6c1dd1).withAlpha(0.20f));
        g.fillEllipse(getWidth()-320.0f, -80.0f, 380.0f, 380.0f);
        g.setColour(juce::Colour(0xffe13dff).withAlpha(0.11f));
        g.fillEllipse(-100.0f, getHeight()-250.0f, 350.0f, 350.0f);
    }
    else
    {
        const float phase = static_cast<float>(juce::Time::getMillisecondCounterHiRes() * 0.00008);
        juce::ColourGradient bg(juce::Colour::fromHSV(std::fmod(0.55f+phase,1.0f),0.48f,0.18f,1), 0, 0,
                                 juce::Colour::fromHSV(std::fmod(0.88f+phase*0.6f,1.0f),0.58f,0.24f,1), (float)getWidth(), (float)getHeight(), true);
        g.setGradientFill(bg); g.fillAll();
        g.setColour(juce::Colours::white.withAlpha(0.07f));
        g.fillEllipse(getWidth()*0.54f, -80.0f, 360.0f, 300.0f);
        g.fillEllipse(-100.0f, getHeight()*0.48f, 330.0f, 330.0f);
    }

    const auto card = getLocalBounds().toFloat().reduced(18.0f);
    g.setColour(juce::Colours::black.withAlpha(t == Theme::Photoreal ? 0.08f : 0.20f));
    g.drawRoundedRectangle(card, 14.0f, 1.0f);
}

void SWATronix2AAudioProcessorEditor::resized()
{
    const auto b = getLocalBounds().reduced(20);
    const int top = 82;
    const int bottom = 88;
    brand.setBounds(b.removeFromTop(34));
    model.setBounds(brand.getBottom(), 0, getWidth(), 24);

    auto footer = getLocalBounds().removeFromBottom(bottom).reduced(22, 10);
    const int cell = footer.getWidth() / 4;
    presetLabel.setBounds(footer.removeFromLeft(cell)); preset.setBounds(presetLabel.getX(), presetLabel.getBottom(), cell-8, 28);
    themeLabel.setBounds(footer.removeFromLeft(cell)); theme.setBounds(themeLabel.getX(), themeLabel.getBottom(), cell-8, 28);
    info.setBounds(footer.removeFromRight(42)); power.setBounds(footer.removeFromRight(110));

    auto content = getLocalBounds().reduced(24);
    content.removeFromTop(top); content.removeFromBottom(bottom);
    auto left = content.removeFromLeft(245);
    meter.setBounds(left.reduced(8));

    auto controls = content.reduced(5);
    auto knobRow = controls.removeFromTop(210);
    gain.setBounds(knobRow.removeFromLeft(knobRow.getWidth()/2).reduced(18));
    peak.setBounds(knobRow.reduced(18));
    gainLabel.setBounds(gain.getX(), gain.getBottom()-5, gain.getWidth(), 22);
    peakLabel.setBounds(peak.getX(), peak.getBottom()-5, peak.getWidth(), 22);

    auto selector = controls.removeFromTop(95);
    modeLabel.setBounds(selector.removeFromLeft(selector.getWidth()/2));
    mode.setBounds(modeLabel.getX(), modeLabel.getY()+26, modeLabel.getWidth()-12, 32);
    meterLabel.setBounds(selector.removeFromLeft(selector.getWidth()));
    meterChoice.setBounds(meterLabel.getX(), meterLabel.getY()+26, meterLabel.getWidth()-12, 32);
}

void SWATronix2AAudioProcessorEditor::rebuildTheme()
{
    const auto t = static_cast<Theme>(juce::jlimit(0,3,(int)processor.parameters.getRawParameterValue(SWATronix2AAudioProcessor::themeParam)->load()));
    activeLF = t == Theme::Photoreal ? &photoreal : t == Theme::Minimal ? &minimal : t == Theme::Neon ? &neon : &aurora;
    setLookAndFeel(activeLF);
    repaint();
}

void SWATronix2AAudioProcessorEditor::applyPreset(int index)
{
    processor.loadFactoryPreset(index);
    theme.setSelectedItem(static_cast<int>(processor.parameters.getRawParameterValue(SWATronix2AAudioProcessor::themeParam)->load()) + 1, juce::dontSendNotification);
    rebuildTheme();
}

void SWATronix2AAudioProcessorEditor::showAbout()
{
    juce::DialogWindow::LaunchOptions o;
    o.content.setOwned(new AboutView());
    o.dialogTitle = "About SWA Tronix 2A";
    o.dialogBackgroundColour = juce::Colour(0xff15171b);
    o.escapeKeyTriggersCloseButton = true;
    o.resizable = false;
    o.useNativeTitleBar = true;
    o.content->setSize(480, 220);
    o.runModal();
}
