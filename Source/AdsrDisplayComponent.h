#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class AdsrDisplayComponent final : public juce::Component
{
public:
    AdsrDisplayComponent();

    using ValueChangedCallback = std::function<void (float attack, float decay, float sustain, float release)>;
    void setValueChangedCallback (ValueChangedCallback cb);

    void setParameters (float attackSec, float decaySec, float sustainLevel, float releaseSec);
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
    enum class DragTarget { none, attackPeak, decaySustain, releaseStart };

    void updateFromMouse (juce::Point<float> mousePos);
    juce::Point<float> getNodePosition (DragTarget target) const;
    DragTarget hitTestNode (juce::Point<float> pos) const;
    void fireCallback();

    float attack  = 0.01f;
    float decay   = 0.1f;
    float sustain = 0.8f;
    float release = 0.2f;

    DragTarget dragTarget = DragTarget::none;
    DragTarget hoverTarget = DragTarget::none;
    juce::Point<float> dragStartMouse;
    float dragStartAttack = 0.0f;
    float dragStartDecay  = 0.0f;
    float dragStartSustain = 0.0f;
    float dragStartRelease = 0.0f;

    ValueChangedCallback callback;

    static constexpr auto bgColour      = 0xFF1A1A20;
    static constexpr auto outlineColour = 0xFF3A3A45;
    static constexpr auto curveColour   = 0xFF00CED1;
    static constexpr auto fillColour    = 0x2000CED1;
    static constexpr auto dotColour     = 0xFFE0E0E5;
    static constexpr auto activeDotColour = 0xFFFFFFFF;
    static constexpr auto hitRadius     = 10.0f;
    static constexpr auto margin        = 4.0f;

    float panelAlpha = 0.2f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrDisplayComponent)
};
