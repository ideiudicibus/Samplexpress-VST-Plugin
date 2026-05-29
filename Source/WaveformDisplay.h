#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SamplexpressAudioProcessor;

class WaveformDisplay final : public juce::Component
{
public:
    explicit WaveformDisplay (SamplexpressAudioProcessor& proc);

    void paint (juce::Graphics& g) override;
    void resized() override;

    void setLoopEnabled (bool enabled);
    void setLoopStart (float normalised);
    void setLoopEnd (float normalised);

private:
    SamplexpressAudioProcessor& processor;

    bool loopEnabled{false};
    float loopStartNorm{0.0f};
    float loopEndNorm{1.0f};

    static constexpr auto panelBgColour = 0xFF18181A;
    static constexpr auto waveColour    = 0xFFFFAA33;
    static constexpr auto textColour    = 0xFF808088;
    static constexpr auto margin          = 4.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
