#include "SamplexpressVoice.h"

float SamplexpressVoice::cubicInterpolate (const float* data, int numSamples, double position)
{
    int pos = static_cast<int> (position);
    float frac = static_cast<float> (position - pos);

    int i0 = juce::jlimit (0, numSamples - 1, pos - 1);
    int i1 = juce::jlimit (0, numSamples - 1, pos);
    int i2 = juce::jlimit (0, numSamples - 1, pos + 1);
    int i3 = juce::jlimit (0, numSamples - 1, pos + 2);

    float y0 = data[i0], y1 = data[i1], y2 = data[i2], y3 = data[i3];

    float c0 = y1;
    float c1 = -0.5f * y0 + 0.5f * y2;
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = -0.5f * y0 + 1.5f * y1 - 1.5f * y2 + 0.5f * y3;

    return ((c3 * frac + c2) * frac + c1) * frac + c0;
}

SamplexpressVoice::SamplexpressVoice (juce::AudioProcessorValueTreeState& apvts)
{
    volAttackParam  = apvts.getRawParameterValue ("vol_attack");
    volDecayParam   = apvts.getRawParameterValue ("vol_decay");
    volSustainParam = apvts.getRawParameterValue ("vol_sustain");
    volReleaseParam = apvts.getRawParameterValue ("vol_release");
    filtAttackParam  = apvts.getRawParameterValue ("filt_attack");
    filtDecayParam   = apvts.getRawParameterValue ("filt_decay");
    filtSustainParam = apvts.getRawParameterValue ("filt_sustain");
    filtReleaseParam = apvts.getRawParameterValue ("filt_release");
    pitchAttackParam  = apvts.getRawParameterValue ("pitch_attack");
    pitchDecayParam   = apvts.getRawParameterValue ("pitch_decay");
    pitchSustainParam = apvts.getRawParameterValue ("pitch_sustain");
    pitchReleaseParam = apvts.getRawParameterValue ("pitch_release");
    filtCutoffParam    = apvts.getRawParameterValue ("filt_cutoff");
    filtResonanceParam = apvts.getRawParameterValue ("filt_resonance");

    loopEnableParam  = apvts.getRawParameterValue ("loop_enable");
    loopStartParam   = apvts.getRawParameterValue ("loop_start");
    loopEndParam     = apvts.getRawParameterValue ("loop_end");
    crossfadeMsParam = apvts.getRawParameterValue ("crossfade_ms");
}

void SamplexpressVoice::updateAdsrParameters()
{
    volumeAdsr.setParameters (juce::ADSR::Parameters {
        volAttackParam->load(),
        volDecayParam->load(),
        volSustainParam->load(),
        volReleaseParam->load()
    });

    filterAdsr.setParameters (juce::ADSR::Parameters {
        filtAttackParam->load(),
        filtDecayParam->load(),
        filtSustainParam->load(),
        filtReleaseParam->load()
    });

    pitchAdsr.setParameters (juce::ADSR::Parameters {
        pitchAttackParam->load(),
        pitchDecayParam->load(),
        pitchSustainParam->load(),
        pitchReleaseParam->load()
    });
}

void SamplexpressVoice::prepareFilter (double sampleRate)
{
    if (sampleRate != filterPreparedSampleRate)
    {
        filter.prepare ({ sampleRate, 512, 2 });
        filter.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        filterPreparedSampleRate = sampleRate;
    }
}

void SamplexpressVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int)
{
    auto* s = dynamic_cast<SamplexpressSound*> (sound);
    if (s == nullptr)
    {
        clearCurrentNote();
        return;
    }

    auto noteFrequency = juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber);
    auto rootFrequency = juce::MidiMessage::getMidiNoteInHertz (s->midiRootNote);
    basePitchRatio = (noteFrequency / rootFrequency) * (s->sampleRate / getSampleRate());

    velocityGain = velocity * 0.7f;
    sourceSamplePosition = 0.0;

    // Cache loop parameters
    loopEnabled = loopEnableParam != nullptr && loopEnableParam->load() > 0.5f;
    if (loopEnabled && s->sampleBuffer != nullptr)
    {
        int totalSamples = s->sampleBuffer->getNumSamples();
        float startNorm = loopStartParam != nullptr ? loopStartParam->load() : 0.0f;
        float endNorm   = loopEndParam   != nullptr ? loopEndParam->load()   : 1.0f;
        loopStartSample = juce::jlimit (0, totalSamples - 1, static_cast<int> (startNorm * totalSamples));
        loopEndSample   = juce::jlimit (0, totalSamples - 1, static_cast<int> (endNorm   * totalSamples));
        if (loopEndSample <= loopStartSample)
            loopEndSample = totalSamples - 1;
        float cfMs = crossfadeMsParam != nullptr ? crossfadeMsParam->load() : 20.0f;
        crossfadeSamples = juce::jlimit (0, (loopEndSample - loopStartSample) / 2,
                                          static_cast<int> (cfMs * 0.001f * getSampleRate()));
        inCrossfade = false;
        crossfadePosition = 0.0;
    }

    volumeAdsr.setSampleRate (getSampleRate());
    filterAdsr.setSampleRate (getSampleRate());
    pitchAdsr.setSampleRate (getSampleRate());

    updateAdsrParameters();

    volumeAdsr.noteOn();
    filterAdsr.noteOn();
    pitchAdsr.noteOn();

    prepareFilter (getSampleRate());
    filter.reset();
}

void SamplexpressVoice::stopNote (float, bool allowTailOff)
{
    if (allowTailOff)
    {
        volumeAdsr.noteOff();
        filterAdsr.noteOff();
        pitchAdsr.noteOff();
    }
    else
    {
        volumeAdsr.reset();
        filterAdsr.reset();
        pitchAdsr.reset();
        clearCurrentNote();
    }
}

void SamplexpressVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    auto* sound = dynamic_cast<SamplexpressSound*> (getCurrentlyPlayingSound().get());
    if (sound == nullptr || sound->sampleBuffer == nullptr || sound->sampleBuffer->getNumSamples() == 0)
    {
        for (int i = startSample; i < startSample + numSamples; ++i)
        {
            outputBuffer.addSample (0, i, 0.0f);
            outputBuffer.addSample (1, i, 0.0f);
        }
        clearCurrentNote();
        return;
    }

    updateAdsrParameters();

    const auto* leftData = sound->sampleBuffer->getReadPointer (0);
    const auto* rightData = sound->sampleBuffer->getNumChannels() > 1
                              ? sound->sampleBuffer->getReadPointer (1)
                              : nullptr;
    const int numSamplesInBuffer = sound->sampleBuffer->getNumSamples();
    const float nyquist = static_cast<float> (getSampleRate() * 0.49);
    const float baseCutoff = filtCutoffParam->load();

    for (int i = startSample; i < startSample + numSamples; ++i)
    {
        if (!volumeAdsr.isActive())
        {
            clearCurrentNote();
            return;
        }

        float volEnv = volumeAdsr.getNextSample();
        float pitchEnv = pitchAdsr.getNextSample();
        float filtEnv = filterAdsr.getNextSample();

        float logMin = std::log (MIN_FILTER_FREQ);
        float logMax = std::log (baseCutoff);
        float cutoffHz = std::exp (logMin + filtEnv * (logMax - logMin));
        cutoffHz = juce::jlimit (MIN_FILTER_FREQ, nyquist, cutoffHz);
        filter.setCutoffFrequency (cutoffHz);
        filter.setResonance (filtResonanceParam->load());

        float pitchSustain = pitchSustainParam->load();
        float pitchOffset = (pitchEnv - pitchSustain) * static_cast<float> (PITCH_ENV_SEMITONES);
        double effectivePitchRatio = basePitchRatio * std::pow (2.0, pitchOffset / 12.0);

        // Loop crossfade logic
        float leftSample, rightSample;

        if (loopEnabled && sourceSamplePosition >= static_cast<double> (loopEndSample - crossfadeSamples))
        {
            double loopBPos = static_cast<double> (loopStartSample)
                              + (sourceSamplePosition - static_cast<double> (loopEndSample - crossfadeSamples));

            float progress = 0.0f;
            if (crossfadeSamples > 0)
                progress = static_cast<float> ((sourceSamplePosition - (loopEndSample - crossfadeSamples)) / crossfadeSamples);
            progress = juce::jlimit (0.0f, 1.0f, progress);

            float fadeOut = std::cos (progress * juce::MathConstants<float>::halfPi);
            float fadeIn  = std::sin (progress * juce::MathConstants<float>::halfPi);

            float leftA  = cubicInterpolate (leftData, numSamplesInBuffer, sourceSamplePosition);
            float rightA = rightData != nullptr
                               ? cubicInterpolate (rightData, numSamplesInBuffer, sourceSamplePosition)
                               : leftA;

            float leftB  = cubicInterpolate (leftData, numSamplesInBuffer, loopBPos);
            float rightB = rightData != nullptr
                               ? cubicInterpolate (rightData, numSamplesInBuffer, loopBPos)
                               : leftB;

            leftSample  = leftA  * fadeOut + leftB  * fadeIn;
            rightSample = rightA * fadeOut + rightB * fadeIn;

            if (sourceSamplePosition >= loopEndSample)
                sourceSamplePosition = loopBPos;
        }
        else
        {
            leftSample  = cubicInterpolate (leftData, numSamplesInBuffer, sourceSamplePosition);
            rightSample = rightData != nullptr
                              ? cubicInterpolate (rightData, numSamplesInBuffer, sourceSamplePosition)
                              : leftSample;
        }

        float leftFiltered  = filter.processSample (0, leftSample * velocityGain);
        float rightFiltered = filter.processSample (1, rightSample * velocityGain);

        outputBuffer.addSample (0, i, leftFiltered * volEnv);
        outputBuffer.addSample (1, i, rightFiltered * volEnv);

        sourceSamplePosition += effectivePitchRatio;
    }

    filter.snapToZero();
}