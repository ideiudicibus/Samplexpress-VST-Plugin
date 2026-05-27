#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class TabBar final : public juce::Component
{
public:
    TabBar();

    using TabChangedCallback = std::function<void (int index)>;
    void setTabChangedCallback (TabChangedCallback cb);

    void setTabNames (const juce::StringArray& names);
    void setCurrentTab (int index);

    void paint (juce::Graphics& g) override;
    void mouseDown (const juce::MouseEvent& e) override;
    void resized() override;

private:
    juce::Rectangle<int> getTabBounds (int index) const;

    juce::StringArray tabNames;
    int currentTabIndex = 0;
    TabChangedCallback callback;

    static constexpr auto inactiveTextColour   = 0xFF808088;
    static constexpr auto activeTextColour     = 0xFFFFFFFF;
    static constexpr auto activeUnderlineColour = 0xFFFFAA33;
    static constexpr auto bgColour             = 0xFF121214;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TabBar)
};
