#include "SamplexpressLookAndFeel.h"

SamplexpressLookAndFeel::SamplexpressLookAndFeel()
{
    setColour (juce::Label::textColourId, juce::Colour (textColour));
    setColour (juce::Label::textWhenEditingColourId, juce::Colour (textColour));
    setColour (juce::Label::outlineWhenEditingColourId, juce::Colour (knobAccentColour));
    setColour (juce::TextButton::buttonColourId, juce::Colour (buttonBgColour));
    setColour (juce::TextButton::buttonOnColourId, juce::Colour (knobAccentColour));
    setColour (juce::TextButton::textColourOffId, juce::Colour (textColour));
    setColour (juce::TextButton::textColourOnId, juce::Colours::black);
}

void SamplexpressLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPosProportional, float rotaryStartAngle,
                                                float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (2.0f);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f - 6.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    constexpr auto arcThickness = 0.12f;

    // Background circle
    g.setColour (juce::Colour (knobBgColour));
    g.fillEllipse (rx, ry, rw, rw);

    // Background arc (track)
    juce::Path backgroundArc;
    backgroundArc.addArc (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (juce::Colour (knobTrackColour));
    g.strokePath (backgroundArc, juce::PathStrokeType (radius * arcThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value arc
    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addArc (rx, ry, rw, rw, rotaryStartAngle, angle, true);
        g.setColour (juce::Colour (knobAccentColour));
        g.strokePath (valueArc, juce::PathStrokeType (radius * arcThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Outer ring
    g.setColour (juce::Colour (knobTrackColour).withAlpha (0.5f));
    g.drawEllipse (rx, ry, rw, rw, 1.0f);

    // Pointer
    juce::Path p;
    auto pointerThickness = radius * 0.12f;
    p.addEllipse (-pointerThickness * 0.5f, -radius + radius * 0.22f,
                  pointerThickness, pointerThickness);
    p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    g.setColour (juce::Colour (knobPointerColour));
    g.fillPath (p);

    // Inner highlight
    g.setColour (juce::Colours::white.withAlpha (0.03f));
    g.fillEllipse (rx + radius * 0.25f, ry + radius * 0.25f, rw * 0.5f, rw * 0.5f);
}

void SamplexpressLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                    const juce::Colour& /*backgroundColour*/,
                                                    bool shouldDrawButtonAsHighlighted,
                                                    bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f);
    auto cornerSize = 6.0f;
    auto baseColour = juce::Colour (buttonBgColour);

    if (button.getToggleState())
        baseColour = juce::Colour (knobAccentColour);
    else if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker (0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter (0.1f);

    g.setColour (baseColour);
    g.fillRoundedRectangle (bounds, cornerSize);

    auto borderColour = button.getToggleState()
                          ? juce::Colour (knobAccentColour).darker (0.2f)
                          : juce::Colour (knobTrackColour);
    g.setColour (borderColour);
    g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
}

void SamplexpressLookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    g.fillAll (label.findColour (juce::Label::backgroundColourId, true));

    if (! label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        auto labelTextColour = label.findColour (juce::Label::textColourId).withMultipliedAlpha (alpha);

        g.setColour (labelTextColour);
        g.setFont (getLabelFont (label));
        g.drawFittedText (label.getText(), label.getLocalBounds(),
                          label.getJustificationType(),
                          juce::jmax (1, label.getHeight() / 14));
    }
    else if (label.isEnabled())
    {
        g.setColour (juce::Colour (knobAccentColour));
        g.drawRect (label.getLocalBounds());
    }
}

juce::Font SamplexpressLookAndFeel::getLabelFont (juce::Label& /*label*/)
{
    return juce::Font (juce::FontOptions { 11.0f });
}
