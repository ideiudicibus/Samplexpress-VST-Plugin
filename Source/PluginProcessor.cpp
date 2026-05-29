#include "PluginProcessor.h"
#include "PluginEditor.h"

SamplexpressAudioProcessor::SamplexpressAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "PARAMETERS", createParameterLayouts())
{
    formatManager.registerBasicFormats();

    for (int i = 0; i < 16; ++i)
        synth.addVoice (new SamplexpressVoice (apvts));

    ringBuffer.resize (static_cast<size_t> (ringBufferSize), 0.0f);
}

SamplexpressAudioProcessor::~SamplexpressAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout SamplexpressAudioProcessor::createParameterLayouts()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto addTimeParam = [&params] (const juce::String& id, const juce::String& name,
                                    float min, float max, float centre, float defaultValue)
    {
        auto range = juce::NormalisableRange<float> (min, max, 0.001f);
        range.setSkewForCentre (centre);
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, name, range, defaultValue));
    };

    auto addLinearParam = [&params] (const juce::String& id, const juce::String& name,
                                      float min, float max, float defaultValue)
    {
        params.push_back (std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { id, 1 }, name,
            juce::NormalisableRange<float> (min, max, 0.001f), defaultValue));
    };

    // Volume ADSR
    addTimeParam ("vol_attack",  "Vol Atk", 0.0f, 5.0f,  1.0f, 0.01f);
    addTimeParam ("vol_decay",   "Vol Dec", 0.0f, 5.0f,  1.0f, 0.1f);
    addLinearParam ("vol_sustain", "Vol Sus", 0.0f, 1.0f, 0.8f);
    addTimeParam ("vol_release", "Vol Rel", 0.0f, 10.0f, 2.0f, 0.2f);

    // Filter ADSR + cutoff/resonance
    addTimeParam ("filt_attack",  "Flt Atk", 0.0f, 5.0f,  1.0f, 0.01f);
    addTimeParam ("filt_decay",   "Flt Dec", 0.0f, 5.0f,  1.0f, 0.1f);
    addLinearParam ("filt_sustain", "Flt Sus", 0.0f, 1.0f, 1.0f);
    addTimeParam ("filt_release", "Flt Rel", 0.0f, 10.0f, 2.0f, 0.1f);
    addTimeParam ("filt_cutoff",    "Flt Cutoff", 200.0f, 20000.0f, 2000.0f, 20000.0f);
    addLinearParam ("filt_resonance", "Flt Res",   0.1f,    10.0f,         0.7f);

    // Pitch ADSR
    addTimeParam ("pitch_attack",  "Pit Atk", 0.0f, 5.0f,  1.0f, 0.0f);
    addTimeParam ("pitch_decay",   "Pit Dec", 0.0f, 5.0f,  1.0f, 0.0f);
    addLinearParam ("pitch_sustain", "Pit Sus", 0.0f, 1.0f, 1.0f);
    addTimeParam ("pitch_release", "Pit Rel", 0.0f, 10.0f, 2.0f, 0.1f);

    // Loop parameters
    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "loop_enable", 1 }, "Loop Enable", false));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "loop_start", 1 }, "Loop Start",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "loop_end", 1 }, "Loop End",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "crossfade_ms", 1 }, "Crossfade",
        juce::NormalisableRange<float> (0.0f, 500.0f, 0.1f), 20.0f));

    return { params.begin(), params.end() };
}

const juce::String SamplexpressAudioProcessor::getName() const
{
    return "Samplexpress";
}

bool SamplexpressAudioProcessor::acceptsMidi() const
{
    return true;
}

bool SamplexpressAudioProcessor::producesMidi() const
{
    return false;
}

bool SamplexpressAudioProcessor::isMidiEffect() const
{
    return false;
}

double SamplexpressAudioProcessor::getTailLengthSeconds() const
{
    return 10.0;
}

int SamplexpressAudioProcessor::getNumPrograms()
{
    return 1;
}

int SamplexpressAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SamplexpressAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String SamplexpressAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void SamplexpressAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void SamplexpressAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    synth.setCurrentPlaybackSampleRate (sampleRate);
}

void SamplexpressAudioProcessor::releaseResources()
{
}

bool SamplexpressAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SamplexpressAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                                juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    if (previewPlayRequested.exchange (false))
    {
        midiMessages.addEvent (juce::MidiMessage::noteOff (previewChannel, previewNote), 0);
        midiMessages.addEvent (juce::MidiMessage::noteOn (previewChannel, previewNote, 0.7f), 1);
    }

    synth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());

    // Capture mixed mono output for spectrum analyzer
    auto numSamples = buffer.getNumSamples();
    auto* left = buffer.getReadPointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getReadPointer (1) : left;

    for (int i = 0; i < numSamples; ++i)
    {
        auto pos = ringWritePos.load (std::memory_order_relaxed);
        ringBuffer[pos] = (left[i] + right[i]) * 0.5f;
        ringWritePos.store ((pos + 1) % static_cast<size_t> (ringBufferSize),
                            std::memory_order_release);
    }
}

void SamplexpressAudioProcessor::getLatestSamples (float* dest, int numSamples) const
{
    auto writePos = ringWritePos.load (std::memory_order_acquire);
    for (int i = 0; i < numSamples; ++i)
    {
        auto idx = (writePos + static_cast<size_t> (ringBufferSize)
                    - static_cast<size_t> (numSamples) + static_cast<size_t> (i))
                   % static_cast<size_t> (ringBufferSize);
        dest[i] = ringBuffer[idx];
    }
}

bool SamplexpressAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* SamplexpressAudioProcessor::createEditor()
{
    return new SamplexpressAudioProcessorEditor (*this);
}

bool SamplexpressAudioProcessor::loadFile (const juce::File& audioFile)
{
    std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (audioFile));

    if (reader == nullptr)
        return false;

    auto numChannels = static_cast<int> (reader->numChannels);
    auto numSamples  = static_cast<int> (reader->lengthInSamples);

    sampleBuffer.setSize (numChannels, numSamples);
    reader->read (&sampleBuffer, 0, numSamples, 0, true, true);

    sampleSampleRate = reader->sampleRate;
    loadedFileName = audioFile.getFileName();
    sampleChanged.store (true);

    {
        juce::SpinLock::ScopedLockType lock (soundLock);
        synth.clearSounds();
        synth.addSound (new SamplexpressSound (&sampleBuffer, sampleSampleRate, midiRootNote));
    }

    return true;
}

void SamplexpressAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree state ("SAMPLEXPRESS");

    state.setProperty ("sampleRate", sampleSampleRate, nullptr);
    state.setProperty ("numChannels", sampleBuffer.getNumChannels(), nullptr);
    state.setProperty ("numSamples", sampleBuffer.getNumSamples(), nullptr);
    state.setProperty ("loadedFileName", loadedFileName, nullptr);

    if (sampleBuffer.getNumSamples() > 0)
    {
        juce::MemoryBlock audioData;
        for (int ch = 0; ch < sampleBuffer.getNumChannels(); ++ch)
        {
            auto* channelData = sampleBuffer.getReadPointer (ch);
            auto numBytes = static_cast<size_t> (sampleBuffer.getNumSamples()) * sizeof (float);
            audioData.append (channelData, numBytes);
        }
        state.setProperty ("audioData", audioData.toBase64Encoding(), nullptr);
    }

    auto apvtsState = apvts.copyState();
    state.addChild (apvtsState, -1, nullptr);

    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void SamplexpressAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml == nullptr)
        return;

    auto state = juce::ValueTree::fromXml (*xml);

    if (! state.isValid() || state.getType() != juce::Identifier ("SAMPLEXPRESS"))
        return;

    sampleSampleRate = static_cast<double> (state.getProperty ("sampleRate", 0.0));
    auto numChannels = static_cast<int> (state.getProperty ("numChannels", 0));
    auto numSamples  = static_cast<int> (state.getProperty ("numSamples", 0));
    loadedFileName = state.getProperty ("loadedFileName", "").toString();

    if (numChannels > 0 && numSamples > 0)
    {
        juce::MemoryBlock audioData;
        audioData.fromBase64Encoding (state.getProperty ("audioData", "").toString());

        auto bytesPerChannel = static_cast<size_t> (numSamples) * sizeof (float);

        sampleBuffer.setSize (numChannels, numSamples);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto* destPtr = sampleBuffer.getWritePointer (ch);
            const auto* srcPtr = static_cast<const float*> (audioData.getData()) + ch * numSamples;
            std::memcpy (destPtr, srcPtr, bytesPerChannel);
        }

        sampleChanged.store (true);

        {
            juce::SpinLock::ScopedLockType lock (soundLock);
            synth.clearSounds();
            synth.addSound (new SamplexpressSound (&sampleBuffer, sampleSampleRate, midiRootNote));
        }
    }

    auto apvtsState = state.getChildWithName (juce::Identifier ("PARAMETERS"));
    if (apvtsState.isValid())
        apvts.replaceState (apvtsState);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplexpressAudioProcessor();
}