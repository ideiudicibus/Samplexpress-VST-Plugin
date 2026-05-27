#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SamplexpressLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    SamplexpressLookAndFeel();

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPosProportional, float rotaryStartAngle,
                           float rotaryEndAngle, juce::Slider& slider) override;

    void drawButtonBackground (juce::Graphics& g, juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override;

    void drawLabel (juce::Graphics& g, juce::Label& label) override;

    juce::Font getLabelFont (juce::Label& label) override;

private:
    static constexpr auto knobBgColour     = 0xFF2A2A30;
    static constexpr auto knobTrackColour  = 0xFF3A3A45;
    static constexpr auto knobAccentColour = 0xFF00CED1;
    static constexpr auto knobPointerColour= 0xFFE0E0E5;
    static constexpr auto textColour       = 0xFFC0C0C8;
    static constexpr auto buttonBgColour   = 0xFF222228;
    static constexpr auto accentOrangeColour = 0xFFFFAA33;
    static constexpr auto accentOrangeBright = 0xFFFFCC66;
};
