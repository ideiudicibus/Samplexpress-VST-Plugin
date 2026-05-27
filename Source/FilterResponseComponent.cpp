#include "FilterResponseComponent.h"

FilterResponseComponent::FilterResponseComponent()
{
    setMouseCursor (juce::MouseCursor::NormalCursor);
}

void FilterResponseComponent::setValueChangedCallback (ValueChangedCallback cb)
{
    callback = std::move (cb);
}

void FilterResponseComponent::setParameters (float cutoffHz, float resonanceVal)
{
    if (dragTarget != DragTarget::none)
        return;

    cutoff    = juce::jlimit (minFreq, maxFreq, cutoffHz);
    resonance = juce::jlimit (minResonance, maxResonance, resonanceVal);
    repaint();
}

juce::Rectangle<float> FilterResponseComponent::getGraphBounds() const
{
    return getLocalBounds().toFloat().reduced (margin);
}

float FilterResponseComponent::getMagnitudeDb (float frequency, float cutoffFreq, float q) const
{
    auto omega = frequency / juce::jlimit (minFreq, maxFreq, cutoffFreq);
    auto omega2 = omega * omega;
    auto term1 = (1.0f - omega2) * (1.0f - omega2);
    auto term2 = (omega / q) * (omega / q);
    auto denom = std::sqrt (term1 + term2);
    if (denom < 0.00001f)
        denom = 0.00001f;
    auto mag = 1.0f / denom;
    return 20.0f * std::log10 (mag);
}

float FilterResponseComponent::cutoffToX (float cutoffFreq, juce::Rectangle<float> graphBounds) const
{
    auto logMin = std::log10 (minFreq);
    auto logMax = std::log10 (maxFreq);
    auto logCut = std::log10 (juce::jlimit (minFreq, maxFreq, cutoffFreq));
    auto t = (logCut - logMin) / (logMax - logMin);
    return graphBounds.getX() + t * graphBounds.getWidth();
}

float FilterResponseComponent::xToCutoff (float x, juce::Rectangle<float> graphBounds) const
{
    auto t = (x - graphBounds.getX()) / graphBounds.getWidth();
    t = juce::jlimit (0.0f, 1.0f, t);
    auto logMin = std::log10 (minFreq);
    auto logMax = std::log10 (maxFreq);
    return std::pow (10.0f, logMin + t * (logMax - logMin));
}

float FilterResponseComponent::resonanceToY (float res, juce::Rectangle<float> graphBounds) const
{
    auto t = (res - minResonance) / (maxResonance - minResonance);
    t = juce::jlimit (0.0f, 1.0f, t);
    return graphBounds.getBottom() - t * graphBounds.getHeight();
}

float FilterResponseComponent::yToResonance (float y, juce::Rectangle<float> graphBounds) const
{
    auto t = (graphBounds.getBottom() - y) / graphBounds.getHeight();
    t = juce::jlimit (0.0f, 1.0f, t);
    return minResonance + t * (maxResonance - minResonance);
}

float FilterResponseComponent::dbToY (float db, juce::Rectangle<float> graphBounds) const
{
    auto t = (db - minDb) / (maxDb - minDb);
    t = juce::jlimit (0.0f, 1.0f, t);
    return graphBounds.getBottom() - t * graphBounds.getHeight();
}

juce::Path FilterResponseComponent::computeResponsePath (float cutoffFreq, float q,
                                                         juce::Rectangle<float> graphBounds) const
{
    juce::Path path;
    bool first = true;

    for (int i = 0; i < 64; ++i)
    {
        auto t = static_cast<float> (i) / 63.0f;
        auto logMin = std::log10 (minFreq);
        auto logMax = std::log10 (maxFreq);
        auto freq = std::pow (10.0f, logMin + t * (logMax - logMin));
        auto db = getMagnitudeDb (freq, cutoffFreq, q);
        auto x = cutoffToX (freq, graphBounds);
        auto y = dbToY (db, graphBounds);

        if (first)
        {
            path.startNewSubPath (x, y);
            first = false;
        }
        else
        {
            path.lineTo (x, y);
        }
    }

    return path;
}

FilterResponseComponent::DragTarget FilterResponseComponent::hitTestHandle (juce::Point<float> pos) const
{
    auto graph = getGraphBounds();

    // Cutoff handle — on curve at cutoff frequency
    auto cutoffX = cutoffToX (cutoff, graph);
    auto cutoffDb = getMagnitudeDb (cutoff, cutoff, resonance);
    auto cutoffY = dbToY (cutoffDb, graph);
    if (juce::Point<float> { cutoffX, cutoffY }.getDistanceSquaredFrom (pos) <= hitRadius * hitRadius)
        return DragTarget::cutoff;

    // Resonance handle — fixed x at 85 % of width, y maps to resonance
    auto resX = graph.getX() + graph.getWidth() * 0.85f;
    auto resY = resonanceToY (resonance, graph);
    if (juce::Point<float> { resX, resY }.getDistanceSquaredFrom (pos) <= hitRadius * hitRadius)
        return DragTarget::resonance;

    return DragTarget::none;
}

void FilterResponseComponent::updateFromMouse (juce::Point<float> mousePos)
{
    auto graph = getGraphBounds();

    switch (dragTarget)
    {
        case DragTarget::cutoff:
        {
            auto mx = juce::jlimit (graph.getX(), graph.getRight(), mousePos.x);
            cutoff = xToCutoff (mx, graph);
            cutoff = juce::jlimit (200.0f, maxFreq, cutoff);
            break;
        }

        case DragTarget::resonance:
        {
            auto my = juce::jlimit (graph.getY(), graph.getBottom(), mousePos.y);
            resonance = yToResonance (my, graph);
            break;
        }

        default:
            break;
    }

    repaint();
    fireCallback();
}

void FilterResponseComponent::fireCallback()
{
    if (callback)
        callback (cutoff, resonance);
}

void FilterResponseComponent::mouseDown (const juce::MouseEvent& e)
{
    dragTarget = hitTestHandle (e.position);
    if (dragTarget != DragTarget::none)
    {
        dragStartMouse = e.position;
        dragStartCutoff    = cutoff;
        dragStartResonance = resonance;
        updateFromMouse (e.position);
    }
}

void FilterResponseComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (dragTarget != DragTarget::none)
        updateFromMouse (e.position);
}

void FilterResponseComponent::mouseUp (const juce::MouseEvent&)
{
    dragTarget = DragTarget::none;
    repaint();
}

void FilterResponseComponent::mouseMove (const juce::MouseEvent& e)
{
    auto prev = hoverTarget;
    hoverTarget = hitTestHandle (e.position);
    if (hoverTarget != prev)
    {
        switch (hoverTarget)
        {
            case DragTarget::cutoff:
                setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
                break;
            case DragTarget::resonance:
                setMouseCursor (juce::MouseCursor::UpDownLeftRightResizeCursor);
                break;
            default:
                setMouseCursor (juce::MouseCursor::NormalCursor);
                break;
        }
        repaint();
    }
}

void FilterResponseComponent::mouseExit (const juce::MouseEvent&)
{
    hoverTarget = DragTarget::none;
    setMouseCursor (juce::MouseCursor::NormalCursor);
    repaint();
}

void FilterResponseComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (juce::Colour (bgColour));
    g.fillRect (bounds);

    g.setColour (juce::Colour (outlineColour));
    g.drawRect (bounds, 1.0f);

    auto graph = getGraphBounds();

    // Draw response curve
    auto path = computeResponsePath (cutoff, resonance, graph);

    // Fill under curve
    juce::Path fill = path;
    fill.lineTo (graph.getRight(), graph.getBottom());
    fill.lineTo (graph.getX(),     graph.getBottom());
    fill.closeSubPath();
    g.setColour (juce::Colour (fillColour));
    g.fillPath (fill);

    // Curve line
    g.setColour (juce::Colour (curveColour));
    g.strokePath (path, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));

    auto drawDot = [&g, this] (juce::Point<float> p, bool active)
    {
        auto radius = active ? 4.0f : 3.0f;
        g.setColour (active ? juce::Colour (activeDotColour) : juce::Colour (dotColour));
        g.fillEllipse (p.x - radius, p.y - radius, radius * 2.0f, radius * 2.0f);

        if (active)
        {
            g.setColour (juce::Colour (curveColour).withAlpha (0.5f));
            g.drawEllipse (p.x - radius - 2.0f, p.y - radius - 2.0f,
                           radius * 2.0f + 4.0f, radius * 2.0f + 4.0f, 1.0f);
        }
    };

    // Cutoff handle
    auto cutoffX = cutoffToX (cutoff, graph);
    auto cutoffDb = getMagnitudeDb (cutoff, cutoff, resonance);
    auto cutoffY = dbToY (cutoffDb, graph);
    bool cutoffActive = (dragTarget == DragTarget::cutoff) || (hoverTarget == DragTarget::cutoff);

    if (cutoffActive)
    {
        g.setColour (juce::Colour (curveColour).withAlpha (0.3f));
        for (float y = graph.getY(); y < graph.getBottom(); y += 6.0f)
            g.drawLine (cutoffX, y, cutoffX, juce::jmin (y + 3.0f, graph.getBottom()), 1.0f);
    }
    drawDot ({ cutoffX, cutoffY }, cutoffActive);

    // Resonance handle
    auto resX = graph.getX() + graph.getWidth() * 0.85f;
    auto resY = resonanceToY (resonance, graph);
    bool resActive = (dragTarget == DragTarget::resonance) || (hoverTarget == DragTarget::resonance);

    if (resActive)
    {
        g.setColour (juce::Colour (curveColour).withAlpha (0.3f));
        for (float x = graph.getX(); x < graph.getRight(); x += 6.0f)
            g.drawLine (x, resY, juce::jmin (x + 3.0f, graph.getRight()), resY, 1.0f);
    }
    drawDot ({ resX, resY }, resActive);
}

void FilterResponseComponent::resized()
{
    repaint();
}
