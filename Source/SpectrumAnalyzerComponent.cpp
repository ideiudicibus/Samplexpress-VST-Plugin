#include "SpectrumAnalyzerComponent.h"
#include "PluginProcessor.h"

SpectrumAnalyzerComponent::SpectrumAnalyzerComponent (SamplexpressAudioProcessor& proc)
    : processor (proc),
      fft (fftOrder),
      window (static_cast<size_t> (fftSize),
              juce::dsp::WindowingFunction<float>::hann),
      windowedSamples (static_cast<size_t> (fftSize), 0.0f),
      fftData (static_cast<size_t> (fftSize)),
      fftOutput (static_cast<size_t> (fftSize)),
      scopeData (static_cast<size_t> (scopeSize), minDb),
      peakData (static_cast<size_t> (scopeSize), minDb)
{
}

SpectrumAnalyzerComponent::~SpectrumAnalyzerComponent()
{
    stopTimer();
}

void SpectrumAnalyzerComponent::prepare (double sr)
{
    sampleRate = sr;
    startTimerHz (30);
}

void SpectrumAnalyzerComponent::timerCallback()
{
    // 1. Fetch latest samples from processor
    processor.getLatestSamples (windowedSamples.data(), fftSize);

    // 2. Apply window
    window.multiplyWithWindowingTable (windowedSamples.data(),
                                        static_cast<size_t> (fftSize));

    // 3. Fill complex input buffer
    for (int i = 0; i < fftSize; ++i)
        fftData[static_cast<size_t> (i)] = windowedSamples[static_cast<size_t> (i)];

    // 4. Perform FFT (out-of-place: distinct input/output buffers)
    fft.perform (fftData.data(), fftOutput.data(), false);

    // 5. Compute magnitudes and map to log-spaced scope bins
    auto nyquistBin = fftSize / 2;
    auto binFreq = [this] (int bin) { return static_cast<float> (bin * sampleRate / fftSize); };

    auto freqToBin = [this, &binFreq, nyquistBin] (float freq)
    {
        for (int b = 0; b <= nyquistBin; ++b)
            if (binFreq (b) >= freq)
                return b;
        return nyquistBin;
    };

    auto minFreq = 20.0f;
    auto maxFreq = 20000.0f;

    for (int s = 0; s < scopeSize; ++s)
    {
        auto t1 = static_cast<float> (s) / static_cast<float> (scopeSize);
        auto t2 = static_cast<float> (s + 1) / static_cast<float> (scopeSize);

        auto lowFreq  = minFreq * std::pow (maxFreq / minFreq, t1);
        auto highFreq = minFreq * std::pow (maxFreq / minFreq, t2);

        auto lowBin  = freqToBin (lowFreq);
        auto highBin = freqToBin (highFreq);
        if (highBin <= lowBin)
            highBin = lowBin + 1;

        float sum = 0.0f;
        int count = 0;
        for (int b = lowBin; b < highBin && b <= nyquistBin; ++b)
        {
            auto mag = std::abs (fftOutput[static_cast<size_t> (b)]);
            sum += mag;
            ++count;
        }

        float avgMag = (count > 0) ? (sum / static_cast<float> (count)) : 0.0f;
        auto db = 20.0f * std::log10 (avgMag + 1e-10f);
        db = juce::jlimit (minDb, maxDb, db);

        // Exponential moving average smoothing
        scopeData[static_cast<size_t> (s)] = 0.6f * scopeData[static_cast<size_t> (s)] + 0.4f * db;

        // Peak-hold with decay
        peakData[static_cast<size_t> (s)] = std::max (scopeData[static_cast<size_t> (s)],
                                                         peakData[static_cast<size_t> (s)] * 0.92f);
    }

    repaint();
}

void SpectrumAnalyzerComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    // Background
    g.setColour (juce::Colour (bgColour));
    g.fillRect (bounds);

    // Outline
    g.setColour (juce::Colour (outlineColour));
    g.drawRect (bounds, 1.0f);

    // Margins for axes
    constexpr float leftMargin   = 40.0f;
    constexpr float bottomMargin = 24.0f;
    constexpr float topMargin    = 4.0f;
    constexpr float rightMargin  = 4.0f;

    auto graph = bounds.withTrimmedLeft (leftMargin)
                       .withTrimmedBottom (bottomMargin)
                       .withTrimmedTop (topMargin)
                       .withTrimmedRight (rightMargin);

    // --- Y-axis: dB labels and horizontal grid lines ---
    constexpr float dbLevels[] = { -96.0f, -72.0f, -48.0f, -24.0f, 0.0f };
    g.setFont (juce::Font (juce::FontOptions { 10.0f }));

    for (auto db : dbLevels)
    {
        auto t = (db - minDb) / (maxDb - minDb);
        auto y = graph.getBottom() - t * graph.getHeight();

        // Grid line
        g.setColour (juce::Colour (gridColour));
        g.drawHorizontalLine (static_cast<int> (y), graph.getX(), graph.getRight());

        // Label
        g.setColour (juce::Colour (textColour));
        auto label = juce::String (static_cast<int> (db));
        juce::Rectangle<float> labelArea (0.0f, y - 6.0f, graph.getX() - 6.0f, 12.0f);
        g.drawText (label, labelArea, juce::Justification::centredRight, false);
    }

    // --- X-axis: frequency labels and tick marks ---
    constexpr float freqValues[] = { 20.0f, 100.0f, 1000.0f, 10000.0f, 20000.0f };
    const char* freqLabels[] = { "20 Hz", "100 Hz", "1 kHz", "10 kHz", "20 kHz" };
    constexpr float minFreq = 20.0f;
    constexpr float maxFreq = 20000.0f;

    for (int i = 0; i < 5; ++i)
    {
        auto t = std::log10 (freqValues[i] / minFreq) / std::log10 (maxFreq / minFreq);
        auto x = graph.getX() + t * graph.getWidth();

        // Tick mark
        g.setColour (juce::Colour (textColour));
        g.drawVerticalLine (static_cast<int> (x),
                            graph.getBottom(),
                            graph.getBottom() + 4.0f);

        // Label
        auto label = juce::String (freqLabels[i]);
        juce::Rectangle<float> labelArea (x - 30.0f, graph.getBottom() + 4.0f, 60.0f, 14.0f);
        g.drawText (label, labelArea, juce::Justification::centred, false);
    }

    // --- Draw filled area under trace ---
    juce::Path fillPath;
    fillPath.startNewSubPath (graph.getX(), graph.getBottom());
    for (int s = 0; s < scopeSize; ++s)
    {
        auto t = static_cast<float> (s) / static_cast<float> (scopeSize - 1);
        auto x = graph.getX() + t * graph.getWidth();
        auto db = scopeData[static_cast<size_t> (s)];
        auto y = graph.getBottom() - ((db - minDb) / (maxDb - minDb)) * graph.getHeight();
        fillPath.lineTo (x, y);
    }
    fillPath.lineTo (graph.getRight(), graph.getBottom());
    fillPath.closeSubPath();

    g.setColour (juce::Colour (traceColour).withAlpha (0.25f));
    g.fillPath (fillPath);

    // --- Draw peak-hold ghost line ---
    juce::Path peakPath;
    bool peakFirst = true;
    for (int s = 0; s < scopeSize; ++s)
    {
        auto t = static_cast<float> (s) / static_cast<float> (scopeSize - 1);
        auto x = graph.getX() + t * graph.getWidth();
        auto db = peakData[static_cast<size_t> (s)];
        auto y = graph.getBottom() - ((db - minDb) / (maxDb - minDb)) * graph.getHeight();
        if (peakFirst)
        {
            peakPath.startNewSubPath (x, y);
            peakFirst = false;
        }
        else
        {
            peakPath.lineTo (x, y);
        }
    }
    g.setColour (juce::Colour (traceColour).withAlpha (0.3f));
    g.strokePath (peakPath, juce::PathStrokeType (1.0f));

    // --- Draw main trace line ---
    juce::Path tracePath;
    bool traceFirst = true;
    for (int s = 0; s < scopeSize; ++s)
    {
        auto t = static_cast<float> (s) / static_cast<float> (scopeSize - 1);
        auto x = graph.getX() + t * graph.getWidth();
        auto db = scopeData[static_cast<size_t> (s)];
        auto y = graph.getBottom() - ((db - minDb) / (maxDb - minDb)) * graph.getHeight();
        if (traceFirst)
        {
            tracePath.startNewSubPath (x, y);
            traceFirst = false;
        }
        else
        {
            tracePath.lineTo (x, y);
        }
    }
    g.setColour (juce::Colour (traceColour).withAlpha (0.9f));
    g.strokePath (tracePath, juce::PathStrokeType (2.0f));
}

void SpectrumAnalyzerComponent::resized()
{
    repaint();
}
