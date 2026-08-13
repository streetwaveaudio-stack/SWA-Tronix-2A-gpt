#include "ThemeLookAndFeel.h"

void ThemeLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float start, float end, juce::Slider& s)
{
    auto b = juce::Rectangle<float>((float)x, (float)y, (float)w, (float)h).reduced(5.0f);
    const auto c = b.getCentre();
    const float r = juce::jmin(b.getWidth(), b.getHeight()) * 0.42f;
    if (theme == Theme::Photoreal)
    {
        g.setColour(juce::Colours::black.withAlpha(0.35f)); g.fillEllipse(c.x-r+3,c.y-r+4,r*2,r*2);
        juce::ColourGradient grad(juce::Colour(0xffd9d9d9), c.x-r, c.y-r, juce::Colour(0xff4e4e4e), c.x+r, c.y+r, true);
        g.setGradientFill(grad); g.fillEllipse(c.x-r,c.y-r,r*2,r*2);
        g.setColour(juce::Colours::white.withAlpha(0.85f)); g.drawEllipse(c.x-r,c.y-r,r*2,r*2,1.0f);
    }
    else
    {
        juce::Colour ring = theme == Theme::Neon ? juce::Colour(0xffd35cff) : (theme == Theme::Minimal ? juce::Colour(0xff20252d) : juce::Colour(0xffa7e6d4));
        g.setColour(ring.withAlpha(theme == Theme::Neon ? 0.20f : 0.35f)); g.fillEllipse(c.x-r,c.y-r,r*2,r*2);
        g.setColour(ring); g.drawEllipse(c.x-r,c.y-r,r*2,r*2,2.0f);
    }
    const float angle = start + pos * (end-start);
    juce::Path mark;
    mark.addRoundedRectangle(-1.8f, -r+6.0f, 3.6f, r*0.35f, 1.5f);
    g.setColour(theme == Theme::Neon ? juce::Colours::white : juce::Colours::black.withAlpha(0.85f));
    g.fillPath(mark, juce::AffineTransform::rotation(angle).translated(c.x, c.y));
}

void ThemeLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& b, bool highlighted, bool)
{
    auto r = b.getLocalBounds().toFloat().reduced(4.0f);
    const bool on = b.getToggleState();
    if (theme == Theme::Neon)
    {
        g.setColour(juce::Colour(0xffa24cff).withAlpha(on ? 0.9f : 0.28f)); g.fillRoundedRectangle(r, 8.0f);
        g.setColour(juce::Colours::white); g.drawRoundedRectangle(r,8.0f,1.5f);
    }
    else
    {
        g.setColour(on ? juce::Colours::white : juce::Colours::black.withAlpha(0.16f)); g.fillRoundedRectangle(r,8.0f);
        g.setColour(juce::Colours::black.withAlpha(0.55f)); g.drawRoundedRectangle(r,8.0f,1.0f);
    }
    g.setColour(on ? juce::Colours::black : juce::Colours::grey.darker(0.3f));
    g.setFont(juce::Font(13.0f, juce::Font::bold));
    g.drawText(b.getButtonText(), r.toNearestInt(), juce::Justification::centred);
    juce::ignoreUnused(highlighted);
}

void ThemeLookAndFeel::drawComboBox(juce::Graphics& g, int w, int h, bool, int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float>(0,0,(float)w,(float)h).reduced(1.0f);
    g.setColour(theme == Theme::Neon ? juce::Colour(0xff1b1028) : juce::Colours::black.withAlpha(0.12f));
    g.fillRoundedRectangle(r,6.0f);
    g.setColour(theme == Theme::Neon ? juce::Colour(0xffd35cff) : juce::Colours::black.withAlpha(0.55f));
    g.drawRoundedRectangle(r,6.0f,1.0f);
    g.setFont(13.0f); g.drawText(box.getText(), r.toNearestInt().reduced(10,0), juce::Justification::centredLeft);
    juce::Path p; p.addTriangle(w-16,h*0.35f,w-8,h*0.35f,w-12,h*0.62f); g.fillPath(p);
}

void ThemeLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float min, float max, juce::Slider::SliderStyle, juce::Slider& s)
{
    auto r = juce::Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(2.0f);
    g.setColour(juce::Colours::black.withAlpha(0.13f)); g.fillRoundedRectangle(r,5.0f);
    const float t = (pos-min)/(max-min);
    g.setColour(theme == Theme::Neon ? juce::Colour(0xffbd4fff) : juce::Colour(0xff707984));
    g.fillRoundedRectangle(0.0f, r.getY(), r.getWidth()*t, r.getHeight(), 5.0f);
}

