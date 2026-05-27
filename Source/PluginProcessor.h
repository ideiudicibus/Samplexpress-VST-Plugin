#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include "SamplexpressVoice.h"

class SamplexpressAudioProcessor final : public juce::AudioProcessor
{
public:
    SamplexpressAudioProcessor();
    ~SamplexpressAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>& buffer,
                       juce::MidiBuffer& midiMessages) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    bool loadFile (const juce::File& audioFile);
    juce::AudioFormatManager& getFormatManager() { return formatManager; }
    const juce::String& getLoadedFileName() const { return loadedFileName; }
    const juce::AudioBuffer<float>& getSampleBuffer() const { return sampleBuffer; }
    double getSampleSampleRate() const { return sampleSampleRate; }
    int getMidiRootNote() const { return midiRootNote; }
    void setMidiRootNote (int note) { midiRootNote = note; }
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    std::atomic<bool> sampleChanged{false};

    std::atomic<bool> previewPlayRequested{false};

    // Spectrum analyzer audio capture
    static constexpr int fftOrder = 9;
    static constexpr int fftSize = 1 << fftOrder;
    static constexpr int ringBufferSize = fftSize * 4;

    std::vector<float> ringBuffer;
    std::atomic<size_t> ringWritePos{0};
    juce::dsp::WindowingFunction<float> window { static_cast<size_t> (fftSize),
                                                     juce::dsp::WindowingFunction<float>::hann };

public:
    void getLatestSamples (float* dest, int numSamples) const;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayouts();

    juce::AudioFormatManager formatManager;
    juce::AudioBuffer<float> sampleBuffer;
    double sampleSampleRate = 0.0;
    juce::String loadedFileName;
    juce::Synthesiser synth;
    juce::SpinLock soundLock;
    int midiRootNote = 60;
    static constexpr int previewChannel = 1;
    static constexpr int previewNote = 60;
    juce::AudioProcessorValueTreeState apvts;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplexpressAudioProcessor)
};