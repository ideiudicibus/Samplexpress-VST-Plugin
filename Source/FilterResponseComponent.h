#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FilterResponseComponent final : public juce::Component
{
public:
    FilterResponseComponent();

    using ValueChangedCallback = std::function<void (float cutoffHz, float resonance)>;
    void setValueChangedCallback (ValueChangedCallback cb);

    void setParameters (float cutoffHz, float resonance);
    bool isDragging() const noexcept { return dragTarget != DragTarget::none; }

    void paint (juce::Graphics& g) override;
    void resized() override;

    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp   (const juce::MouseEvent& e) override;
    void mouseMove (const juce::MouseEvent& e) override;
    void mouseExit (const juce::MouseEvent& e) override;

    juce::Rectangle<float> getGraphBounds() const;
    void setPanelAlpha (float a) { panelAlpha = juce::jlimit (0.0f, 1.0f, a); repaint(); }

private:
    enum class DragTarget { none, cutoff, resonance };

    void updateFromMouse (juce::Point<float> mousePos);
    DragTarget hitTestHandle (juce::Point<float> pos) const;
    void fireCallback();

    float getMagnitudeDb (float frequency, float cutoffFreq, float q) const;

    float cutoffToX (float cutoffFreq, juce::Rectangle<float> graphBounds) const;
    float xToCutoff (float x, juce::Rectangle<float> graphBounds) const;

    float resonanceToY (float res, juce::Rectangle<float> graphBounds) const;
    float yToResonance (float y, juce::Rectangle<float> graphBounds) const;

    float dbToY (float db, juce::Rectangle<float> graphBounds) const;

    juce::Path computeResponsePath (float cutoffFreq, float q, juce::Rectangle<float> graphBounds) const;

    float cutoff  = 20000.0f;
    float resonance = 0.7f;

    DragTarget dragTarget = DragTarget::none;
    DragTarget hoverTarget = DragTarget::none;
    juce::Point<float> dragStartMouse;
    float dragStartCutoff = 0.0f;
    float dragStartResonance = 0.0f;

    ValueChangedCallback callback;

    static constexpr auto minFreq = 20.0f;
    static constexpr auto maxFreq = 20000.0f;
    static constexpr auto minResonance = 0.1f;
    static constexpr auto maxResonance = 10.0f;
    static constexpr auto minDb = -24.0f;
    static constexpr auto maxDb = 6.0f;

    static constexpr auto bgColour        = 0xFF1A1A20;
    static constexpr auto outlineColour   = 0xFF3A3A45;
    static constexpr auto curveColour     = 0xFF00CED1;
    static constexpr auto fillColour      = 0x2000CED1;
    static constexpr auto dotColour       = 0xFFE0E0E5;
    static constexpr auto activeDotColour = 0xFFFFFFFF;
    static constexpr auto hitRadius       = 10.0f;
    static constexpr auto margin          = 4.0f;

    float panelAlpha = 0.2f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FilterResponseComponent)
};
