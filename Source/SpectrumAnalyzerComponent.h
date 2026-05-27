#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>

class SamplexpressAudioProcessor;

class SpectrumAnalyzerComponent final : public juce::Component,
                                         private juce::Timer
{
public:
    explicit SpectrumAnalyzerComponent (SamplexpressAudioProcessor& proc);
    ~SpectrumAnalyzerComponent() override;

    void prepare (double sampleRate);

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void timerCallback() override;

    SamplexpressAudioProcessor& processor;

    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;

    static constexpr int fftOrder = 9;
    static constexpr int fftSize  = 1 << fftOrder;
    static constexpr int scopeSize = 32;
    static constexpr float minDb = -96.0f;
    static constexpr float maxDb = 0.0f;

    std::vector<float> windowedSamples;
    std::vector<std::complex<float>> fftData;
    std::vector<std::complex<float>> fftOutput;
    std::vector<float> scopeData;
    std::vector<float> peakData;

    double sampleRate = 44100.0;

    static constexpr auto bgColour      = 0xFF1A1A20;
    static constexpr auto outlineColour = 0xFF3A3A45;
    static constexpr auto traceColour   = 0xFF00CED1;
    static constexpr auto gridColour    = 0xFF3A3A45;
    static constexpr auto textColour    = 0xFFCCCCCC;
    static constexpr auto margin        = 4.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzerComponent)
};
