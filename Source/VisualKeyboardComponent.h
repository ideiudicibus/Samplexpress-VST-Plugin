#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>

class VisualKeyboardComponent final : public juce::Component,
                                      private juce::Timer
{
public:
    explicit VisualKeyboardComponent (juce::MidiKeyboardState& keyboardState);
    ~VisualKeyboardComponent() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    struct WhiteKey
    {
        int midiNote;
        juce::Rectangle<float> bounds;
        juce::String label;
    };

    struct BlackKey
    {
        int midiNote;
        juce::Rectangle<float> bounds;
    };

    void timerCallback() override;

    juce::MidiKeyboardState& keyboardState;
    std::vector<WhiteKey> whiteKeys;
    std::vector<BlackKey> blackKeys;

    float whiteKeyWidth = 0.0f;
    float blackKeyWidth = 0.0f;
    float keyHeight = 0.0f;
    float blackKeyHeight = 0.0f;

    static constexpr auto whiteKeyFill    = 0xFF2A2A2E;
    static constexpr auto whiteKeyBorder  = 0xFF404048;
    static constexpr auto blackKeyFill    = 0xFF151518;
    static constexpr auto blackKeyBorder  = 0xFF303038;
    static constexpr auto pressedFill     = 0xFFFFAA33;
    static constexpr auto pressedFillDark = 0xFFCC8822;
    static constexpr auto labelColour     = 0xFF808088;
    static constexpr auto bgColour        = 0xFF1E1E22;

    static constexpr auto cornerRadius    = 2.0f;
    static constexpr int  firstNote       = 48; // C3
    static constexpr int  numWhiteKeys    = 14;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualKeyboardComponent)
};
