#include "VisualKeyboardComponent.h"

VisualKeyboardComponent::VisualKeyboardComponent (juce::MidiKeyboardState& state)
    : keyboardState (state)
{
    startTimerHz (30);
}

VisualKeyboardComponent::~VisualKeyboardComponent()
{
    stopTimer();
}

void VisualKeyboardComponent::resized()
{
    whiteKeys.clear();
    blackKeys.clear();

    whiteKeyWidth  = static_cast<float> (getWidth()) / numWhiteKeys;
    keyHeight      = static_cast<float> (getHeight());
    blackKeyWidth  = whiteKeyWidth * 0.6f;
    blackKeyHeight = keyHeight * 0.62f;

    // Build white keys (2 octaves, C3–B4)
    const int whiteNoteIndices[] = { 0, 2, 4, 5, 7, 9, 11 }; // C D E F G A B offsets within octave

    for (int octave = 0; octave < 2; ++octave)
    {
        for (int i = 0; i < 7; ++i)
        {
            int midiNote = firstNote + octave * 12 + whiteNoteIndices[i];
            int index = octave * 7 + i;

            auto x = static_cast<float> (index) * whiteKeyWidth;
            juce::String label;
            switch (i)
            {
                case 0: label = "C"  + juce::String (3 + octave); break;
                case 1: label = "D"  + juce::String (3 + octave); break;
                case 2: label = "E"  + juce::String (3 + octave); break;
                case 3: label = "F"  + juce::String (3 + octave); break;
                case 4: label = "G"  + juce::String (3 + octave); break;
                case 5: label = "A"  + juce::String (3 + octave); break;
                case 6: label = "B"  + juce::String (3 + octave); break;
            }

            whiteKeys.push_back ({ midiNote, { x, 0.0f, whiteKeyWidth, keyHeight }, label });
        }
    }

    // Build black keys — offset from the preceding white key
    // Pattern per octave: after C(0), D(1), F(3), G(4), A(5)
    const int blackKeyPredecessors[] = { 0, 1, 3, 4, 5 };

    for (int octave = 0; octave < 2; ++octave)
    {
        for (int i = 0; i < 5; ++i)
        {
            int whiteIndex = octave * 7 + blackKeyPredecessors[i];
            int midiNote = whiteKeys[static_cast<size_t> (whiteIndex)].midiNote + 1;

            auto x = static_cast<float> (whiteIndex + 1) * whiteKeyWidth - blackKeyWidth * 0.5f;
            blackKeys.push_back ({ midiNote, { x, 0.0f, blackKeyWidth, blackKeyHeight } });
        }
    }
}

void VisualKeyboardComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (bgColour));

    // Draw white keys (back layer)
    for (const auto& key : whiteKeys)
    {
        bool pressed = keyboardState.isNoteOnForChannels (0xffff, key.midiNote);
        auto fillColour = pressed ? juce::Colour (pressedFill) : juce::Colour (whiteKeyFill);
        auto borderColour = pressed
            ? juce::Colour (pressedFill).darker (0.2f)
            : juce::Colour (whiteKeyBorder);

        g.setColour (fillColour);
        g.fillRoundedRectangle (key.bounds, cornerRadius);
        g.setColour (borderColour);
        g.drawRoundedRectangle (key.bounds, cornerRadius, 1.0f);

        // Label
        g.setColour (juce::Colour (labelColour));
        g.setFont (juce::Font (juce::FontOptions { 9.0f }));
        g.drawText (key.label,
                    key.bounds.reduced (2.0f).withTrimmedTop (key.bounds.getHeight() * 0.65f),
                    juce::Justification::centredBottom,
                    false);
    }

    // Draw black keys (front layer)
    for (const auto& key : blackKeys)
    {
        bool pressed = keyboardState.isNoteOnForChannels (0xffff, key.midiNote);
        auto fillColour = pressed ? juce::Colour (pressedFillDark) : juce::Colour (blackKeyFill);
        auto borderColour = pressed
            ? juce::Colour (pressedFillDark).darker (0.2f)
            : juce::Colour (blackKeyBorder);

        g.setColour (fillColour);
        g.fillRoundedRectangle (key.bounds, cornerRadius);
        g.setColour (borderColour);
        g.drawRoundedRectangle (key.bounds, cornerRadius, 1.0f);
    }
}

void VisualKeyboardComponent::timerCallback()
{
    repaint();
}
