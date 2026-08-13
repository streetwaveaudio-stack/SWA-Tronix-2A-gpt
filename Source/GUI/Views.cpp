#include "Views.h"

void MeterView::paint(juce::Graphics& g)
{
    auto r = getLocalBounds().toFloat().reduced(6.0f);
    g.setColour(juce::Colour(0xff202226)); g.fillRoundedRectangle(r, 12.0f);
    g.setColour(juce::Colour(0xffdfd7c8)); g.fillRoundedRectangle(r.reduced(10.0f), 7.0f);
    auto face = r.reduced(22.0f);
    g.setColour(juce::Colours::black.withAlpha(0.65f));
    g.setFont(juce::Font(18.0f, juce::Font::bold));
    g.drawText("SWA", face.removeFromTop(24).toNearestInt(), juce::Justification::centred);

    const bool grMode = processor.parameters.getRawParameterValue(SWATronix2AAudioProcessor::meterParam)->load() > 0.5f;
    const float n = grMode ? juce::jmap(processor.meterGainReductionDb.load(), -30.0f, 0.0f, 0.0f, 1.0f)
                           : juce::jlimit(0.0f, 1.0f, processor.vuNeedle.load());
    const float centerX = r.getCentreX(), centerY = r.getBottom() - r.getHeight()*0.24f;
    const float radius = r.getWidth()*0.36f;
    juce::Path arc;
    arc.addCentredArc(centerX, centerY, radius, radius, 0.0f, juce::MathConstants<float>::pi*1.18f, juce::MathConstants<float>::pi*1.82f, true);
    g.setColour(juce::Colours::black.withAlpha(0.25f)); g.strokePath(arc, juce::PathStrokeType(4.0f));

    const float a = juce::MathConstants<float>::pi*1.18f + n * juce::MathConstants<float>::pi*0.64f;
    juce::Path needle; needle.addTriangle(-2.0f,0.0f,2.0f,0.0f,0.0f,-radius*0.82f);
    g.setColour(juce::Colours::black.withAlpha(0.9f)); g.fillPath(needle, juce::AffineTransform::rotation(a).translated(centerX,centerY));
    g.setColour(juce::Colour(0xffb72f2f)); g.fillEllipse(centerX-5, centerY-5, 10, 10);
    g.setColour(juce::Colours::black.withAlpha(0.7f)); g.setFont(11.0f);
    g.drawText("-20", (int)(centerX-radius), (int)(centerY-radius*0.62f), 32, 18, juce::Justification::centred);
    g.drawText("0", (int)(centerX-12), (int)(centerY-radius*0.86f), 24, 18, juce::Justification::centred);
    g.drawText("+4", (int)(centerX+radius*0.62f), (int)(centerY-radius*0.62f), 32, 18, juce::Justification::centred);
}

AboutView::AboutView()
{
    setOpaque(true);
}

void AboutView::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff15171b));
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(26.0f, juce::Font::bold));
    g.drawText("StreetWave Audio", 20, 20, getWidth()-40, 35, juce::Justification::centred);
    g.setFont(juce::Font(20.0f));
    g.drawText("SWA Tronix 2A", 20, 65, getWidth()-40, 30, juce::Justification::centred);
    g.setFont(juce::Font(14.0f));
    g.setColour(juce::Colours::lightgrey);
    g.drawText("Version 1.0.0  •  Original DSP / procedural UI", 20, 108, getWidth()-40, 24, juce::Justification::centred);
    g.drawText("Credits / contact: replace this line with your website or support email.", 20, 145, getWidth()-40, 24, juce::Justification::centred);
}
