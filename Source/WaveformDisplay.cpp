#include "WaveformDisplay.h"
#include "PluginProcessor.h"

WaveformDisplay::WaveformDisplay (SamplexpressAudioProcessor& proc)
    : processor (proc)
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (juce::Colour (panelBgColour));
    g.fillRect (bounds);

    g.setColour (juce::Colour (0xFF2A2A30));
    g.drawRect (bounds, 1.0f);

    const auto& buffer = processor.getSampleBuffer();
    if (buffer.getNumSamples() == 0)
    {
        g.setColour (juce::Colour (textColour));
        g.setFont (juce::Font (juce::FontOptions { 14.0f }));
        g.drawText ("No sample loaded", bounds, juce::Justification::centred, false);
        return;
    }

    auto graph  = bounds.reduced (margin);
    auto graphW = graph.getWidth();
    auto graphH = graph.getHeight();
    auto numSamples   = buffer.getNumSamples();
    auto numChannels  = buffer.getNumChannels();
    auto midY  = graph.getCentreY();
    auto scale = graphH * 0.45f;

    std::vector<float> tops, bottoms;
    tops.reserve (static_cast<size_t> (graphW));
    bottoms.reserve (static_cast<size_t> (graphW));

    for (int x = 0; x < static_cast<int> (graphW); ++x)
    {
        auto start = static_cast<int> ((x / graphW) * numSamples);
        auto end   = static_cast<int> (((x + 1.0f) / graphW) * numSamples);
        end = juce::jmin (end, numSamples);

        float minVal = 0.0f, maxVal = 0.0f;
        for (int s = start; s < end; ++s)
        {
            float sample = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
                sample += buffer.getReadPointer (ch)[s];
            sample /= numChannels;
            if (sample < minVal) minVal = sample;
            if (sample > maxVal) maxVal = sample;
        }

        tops.push_back (midY - maxVal * scale);
        bottoms.push_back (midY - minVal * scale);
    }

    juce::Path fillPath;
    fillPath.startNewSubPath (graph.getX(), tops[0]);
    for (size_t i = 1; i < tops.size(); ++i)
        fillPath.lineTo (graph.getX() + static_cast<float> (i), tops[i]);
    for (int i = static_cast<int> (bottoms.size()) - 1; i >= 0; --i)
        fillPath.lineTo (graph.getX() + static_cast<float> (i), bottoms[i]);
    fillPath.closeSubPath();

    g.setColour (juce::Colour (waveColour).withAlpha (0.2f));
    g.fillPath (fillPath);

    juce::Path tracePath;
    tracePath.startNewSubPath (graph.getX(), tops[0]);
    for (size_t i = 1; i < tops.size(); ++i)
        tracePath.lineTo (graph.getX() + static_cast<float> (i), tops[i]);

    g.setColour (juce::Colour (waveColour).withAlpha (0.9f));
    g.strokePath (tracePath, juce::PathStrokeType (1.5f));

    g.setColour (juce::Colour (textColour));
    g.setFont (juce::Font (juce::FontOptions { 10.0f }));
    g.drawText (processor.getLoadedFileName(),
                graph.getX() + 4.0f, graph.getY() + 2.0f,
                graphW - 8.0f, 14.0f,
                juce::Justification::left, false);
}

void WaveformDisplay::resized()
{
    repaint();
}
