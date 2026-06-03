#include "TabBar.h"

TabBar::TabBar() {}

void TabBar::setTabChangedCallback (TabChangedCallback cb) { callback = std::move (cb); }

void TabBar::setTabNames (const juce::StringArray& names)
{
    tabNames = names;
    repaint();
}

void TabBar::setCurrentTab (int index)
{
    if (index >= 0 && index < tabNames.size())
    {
        if (index != currentTabIndex)
        {
            currentTabIndex = index;
            repaint();
        }
        if (callback) callback (currentTabIndex);
    }
}

juce::Rectangle<int> TabBar::getTabBounds (int index) const
{
    if (tabNames.isEmpty()) return {};
    auto w = getWidth() / tabNames.size();
    return { index * w, 0, w, getHeight() };
}

void TabBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (bgColour));

    for (int i = 0; i < tabNames.size(); ++i)
    {
        auto bounds = getTabBounds (i).toFloat();
        bool active = (i == currentTabIndex);

        g.setColour (juce::Colour (active ? activeTextColour : inactiveTextColour));
        g.setFont (juce::Font (juce::FontOptions { 11.0f, juce::Font::bold }));
        g.drawText (tabNames[i], bounds, juce::Justification::centred, false);

        if (active)
        {
            auto lineY = bounds.getBottom() - 3.0f;
            g.setColour (juce::Colour (activeUnderlineColour));
            g.drawLine (bounds.getX() + bounds.getWidth() * 0.25f, lineY,
                        bounds.getRight() - bounds.getWidth() * 0.25f, lineY, 2.0f);
        }
    }
}

void TabBar::mouseDown (const juce::MouseEvent& e)
{
    for (int i = 0; i < tabNames.size(); ++i)
    {
        if (getTabBounds (i).contains (e.getPosition()))
        {
            setCurrentTab (i);
            break;
        }
    }
}

void TabBar::resized()
{
    repaint();
}
