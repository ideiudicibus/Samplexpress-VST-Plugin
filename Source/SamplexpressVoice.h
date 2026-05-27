#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class SamplexpressSound final : public juce::SynthesiserSound
{
public:
    SamplexpressSound (const juce::AudioBuffer<float>* buffer, double rate, int rootNote)
        : sampleBuffer (buffer), sampleRate (rate), midiRootNote (rootNote) {}

    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }

    const juce::AudioBuffer<float>* sampleBuffer;
    double sampleRate = 44100.0;
    int midiRootNote = 60;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplexpressSound)
};

class SamplexpressVoice final : public juce::SynthesiserVoice
{
public:
    SamplexpressVoice (juce::AudioProcessorValueTreeState& apvts);

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SamplexpressSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int) override;
    void stopNote (float, bool allowTailOff) override;
    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    void updateAdsrParameters();
    void prepareFilter (double sampleRate);

    static float cubicInterpolate (const float* data, int numSamples, double position);

    static constexpr double PITCH_ENV_SEMITONES = 12.0;
    static constexpr float MIN_FILTER_FREQ = 20.0f;

    juce::ADSR volumeAdsr;
    juce::ADSR filterAdsr;
    juce::ADSR pitchAdsr;

    std::atomic<float>* volAttackParam  = nullptr;
    std::atomic<float>* volDecayParam   = nullptr;
    std::atomic<float>* volSustainParam = nullptr;
    std::atomic<float>* volReleaseParam = nullptr;
    std::atomic<float>* filtAttackParam  = nullptr;
    std::atomic<float>* filtDecayParam   = nullptr;
    std::atomic<float>* filtSustainParam = nullptr;
    std::atomic<float>* filtReleaseParam = nullptr;
    std::atomic<float>* pitchAttackParam  = nullptr;
    std::atomic<float>* pitchDecayParam   = nullptr;
    std::atomic<float>* pitchSustainParam = nullptr;
    std::atomic<float>* pitchReleaseParam = nullptr;
    std::atomic<float>* filtCutoffParam    = nullptr;
    std::atomic<float>* filtResonanceParam = nullptr;

    juce::dsp::StateVariableTPTFilter<float> filter;
    double filterPreparedSampleRate = 0.0;

    double sourceSamplePosition = 0.0;
    double basePitchRatio = 1.0;
    float velocityGain = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplexpressVoice)
};