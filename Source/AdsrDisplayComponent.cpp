#include "AdsrDisplayComponent.h"

AdsrDisplayComponent::AdsrDisplayComponent()
{
    setMouseCursor (juce::MouseCursor::NormalCursor);
}

void AdsrDisplayComponent::setValueChangedCallback (ValueChangedCallback cb)
{
    callback = std::move (cb);
}

void AdsrDisplayComponent::setParameters (float attackSec, float decaySec, float sustainLevel, float releaseSec)
{
    if (dragTarget != DragTarget::none)
        return; // ignore external updates while user is dragging

    attack  = juce::jlimit (0.0f, 5.0f, attackSec);
    decay   = juce::jlimit (0.0f, 5.0f, decaySec);
    sustain = juce::jlimit (0.0f, 1.0f, sustainLevel);
    release = juce::jlimit (0.0f, 10.0f, releaseSec);
    repaint();
}

juce::Rectangle<float> AdsrDisplayComponent::getGraphBounds() const
{
    return getLocalBounds().toFloat().reduced (margin);
}

std::array<juce::Point<float>, 5> computeNodePositions (float attack, float decay, float sustain, float release,
                                                            juce::Rectangle<float> graphBounds)
{
    auto x = graphBounds.getX();
    auto y = graphBounds.getY();
    auto w = graphBounds.getWidth();
    auto h = graphBounds.getHeight();
    auto bottom = y + h;
    auto top    = y;

    auto totalTime = attack + decay + release + 0.3f;
    auto aW = w * (attack  / totalTime);
    auto dW = w * (decay   / totalTime);
    auto sW = w * (0.3f    / totalTime);
    auto rW = w * (release / totalTime);

    constexpr auto minW = 4.0f;
    if (aW < minW && attack > 0.0f) aW = minW;
    if (dW < minW && decay  > 0.0f) dW = minW;
    if (rW < minW && release > 0.0f) rW = minW;

    auto susY = bottom - sustain * h;

    return std::array<juce::Point<float>, 5> {
        juce::Point<float> { x,                    bottom },
        juce::Point<float> { x + aW,               top    },
        juce::Point<float> { x + aW + dW,          susY   },
        juce::Point<float> { x + aW + dW + sW,     susY   },
        juce::Point<float> { x + aW + dW + sW + rW, bottom }
    };
}

juce::Point<float> AdsrDisplayComponent::getNodePosition (DragTarget target) const
{
    auto nodes = computeNodePositions (attack, decay, sustain, release, getGraphBounds());
    switch (target)
    {
        case DragTarget::attackPeak:    return nodes[1];
        case DragTarget::decaySustain:  return nodes[2];
        case DragTarget::releaseStart:  return nodes[3];
        default:                        return {};
    }
}

AdsrDisplayComponent::DragTarget AdsrDisplayComponent::hitTestNode (juce::Point<float> pos) const
{
    auto nodes = computeNodePositions (attack, decay, sustain, release, getGraphBounds());

    for (auto t : { DragTarget::attackPeak, DragTarget::decaySustain, DragTarget::releaseStart })
    {
        auto nodePos = getNodePosition (t);
        if (nodePos.getDistanceSquaredFrom (pos) <= hitRadius * hitRadius)
            return t;
    }

    return DragTarget::none;
}

void AdsrDisplayComponent::updateFromMouse (juce::Point<float> mousePos)
{
    auto graph = getGraphBounds();
    auto gx = graph.getX();
    auto gy = graph.getY();
    auto w  = graph.getWidth();
    auto h  = graph.getHeight();
    auto bottom = gy + h;

    auto mx = juce::jlimit (gx + 1.0f, gx + w - 1.0f, mousePos.x) - gx;
    auto my = juce::jlimit (gy,        gy + h,         mousePos.y);

    switch (dragTarget)
    {
        case DragTarget::attackPeak:
        {
            auto denom = w - mx;
            if (denom < 0.001f) denom = 0.001f;
            attack = mx * (dragStartDecay + dragStartRelease + 0.3f) / denom;
            attack = juce::jlimit (0.0f, 5.0f, attack);
            break;
        }

        case DragTarget::decaySustain:
        {
            auto denom = w - mx;
            if (denom < 0.001f) denom = 0.001f;
            auto sumAD = mx * (dragStartRelease + 0.3f) / denom;
            decay = sumAD - dragStartAttack;
            decay = juce::jlimit (0.0f, 5.0f, decay);

            sustain = (bottom - my) / h;
            sustain = juce::jlimit (0.0f, 1.0f, sustain);
            break;
        }

        case DragTarget::releaseStart:
        {
            auto denom = mx;
            if (denom < 0.001f) denom = 0.001f;
            auto sumADH = dragStartAttack + dragStartDecay + 0.3f;
            release = sumADH * (w - mx) / denom;
            release = juce::jlimit (0.0f, 10.0f, release);
            break;
        }

        default:
            break;
    }

    repaint();
    fireCallback();
}

void AdsrDisplayComponent::fireCallback()
{
    if (callback)
        callback (attack, decay, sustain, release);
}

juce::String AdsrDisplayComponent::formatNodeValue (DragTarget target) const
{
    switch (target)
    {
        case DragTarget::attackPeak:
            return "Atk " + juce::String (attack, 2) + " s";
        case DragTarget::decaySustain:
            return "Dec " + juce::String (decay, 2) + " s / Sus "
                   + juce::String (sustain, 2);
        case DragTarget::releaseStart:
            return "Rel " + juce::String (release, 2) + " s";
        default:
            return {};
    }
}

juce::String AdsrDisplayComponent::getTooltip()
{
    switch (hoverTarget)
    {
        case DragTarget::attackPeak:   return "Attack";
        case DragTarget::decaySustain: return "Decay / Sustain";
        case DragTarget::releaseStart: return "Release";
        default:                       return {};
    }
}

void AdsrDisplayComponent::mouseDown (const juce::MouseEvent& e)
{
    dragTarget = hitTestNode (e.position);
    if (dragTarget != DragTarget::none)
    {
        dragStartMouse = e.position;
        dragStartAttack  = attack;
        dragStartDecay   = decay;
        dragStartSustain = sustain;
        dragStartRelease = release;
        updateFromMouse (e.position);
    }
}

void AdsrDisplayComponent::mouseDrag (const juce::MouseEvent& e)
{
    if (dragTarget != DragTarget::none)
        updateFromMouse (e.position);
}

void AdsrDisplayComponent::mouseUp (const juce::MouseEvent&)
{
    dragTarget = DragTarget::none;
    repaint();
}

void AdsrDisplayComponent::mouseMove (const juce::MouseEvent& e)
{
    auto prev = hoverTarget;
    hoverTarget = hitTestNode (e.position);
    if (hoverTarget != prev)
    {
        switch (hoverTarget)
        {
            case DragTarget::attackPeak:
            case DragTarget::releaseStart:
                setMouseCursor (juce::MouseCursor::LeftRightResizeCursor);
                break;
            case DragTarget::decaySustain:
                setMouseCursor (juce::MouseCursor::UpDownLeftRightResizeCursor);
                break;
            default:
                setMouseCursor (juce::MouseCursor::NormalCursor);
                break;
        }
        repaint();
    }
}

void AdsrDisplayComponent::mouseExit (const juce::MouseEvent&)
{
    hoverTarget = DragTarget::none;
    setMouseCursor (juce::MouseCursor::NormalCursor);
    repaint();
}

void AdsrDisplayComponent::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);

    g.setColour (juce::Colour (bgColour).withAlpha (panelAlpha));
    g.fillRect (bounds);

    g.setColour (juce::Colour (outlineColour).withAlpha (panelAlpha + 0.3f));
    g.drawRect (bounds, 1.0f);

    auto graph = getGraphBounds();
    auto nodes = computeNodePositions (attack, decay, sustain, release, graph);

    // Envelope path
    juce::Path env;
    env.startNewSubPath (nodes[0]);
    for (size_t i = 1; i < nodes.size(); ++i)
        env.lineTo (nodes[i]);

    // Fill under curve
    juce::Path fill = env;
    fill.lineTo (nodes.back().x, graph.getBottom());
    fill.lineTo (nodes[0].x, graph.getBottom());
    fill.closeSubPath();
    g.setColour (juce::Colour (fillColour));
    g.fillPath (fill);

    // Curve line
    g.setColour (juce::Colour (curveColour));
    g.strokePath (env, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Draw nodes
    auto drawNode = [&g, this] (juce::Point<float> p, DragTarget t)
    {
        bool active = (dragTarget == t) || (hoverTarget == t);
        auto radius = active ? activeDotRadius : restDotRadius;
        g.setColour (active ? juce::Colour (activeDotColour) : juce::Colour (dotColour));
        g.fillEllipse (p.x - radius, p.y - radius, radius * 2.0f, radius * 2.0f);

        if (active)
        {
            g.setColour (juce::Colour (curveColour).withAlpha (0.5f));
            g.drawEllipse (p.x - radius - 2.0f, p.y - radius - 2.0f,
                           radius * 2.0f + 4.0f, radius * 2.0f + 4.0f, 1.0f);
        }
    };

    drawNode (nodes[0], DragTarget::none);
    drawNode (nodes[1], DragTarget::attackPeak);
    drawNode (nodes[2], DragTarget::decaySustain);
    drawNode (nodes[3], DragTarget::releaseStart);
    drawNode (nodes[4], DragTarget::none);

    // Guide lines for active drag
    if (dragTarget == DragTarget::decaySustain)
    {
        g.setColour (juce::Colour (curveColour).withAlpha (0.3f));
        g.drawHorizontalLine (static_cast<int> (nodes[2].y), graph.getX(), graph.getRight());
        g.drawVerticalLine   (static_cast<int> (nodes[2].x), graph.getY(), graph.getBottom());
    }

    // Live numeric readout while dragging
    if (dragTarget != DragTarget::none)
    {
        auto readout = formatNodeValue (dragTarget);
        if (readout.isNotEmpty())
        {
            auto nodePos = getNodePosition (dragTarget);
            g.setFont (juce::Font (juce::FontOptions { readoutFontSize, juce::Font::bold }));
            auto textW = g.getCurrentFont().getStringWidth (readout) + 8.0f;
            auto textH = readoutFontSize + 4.0f;
            juce::Rectangle<float> box (nodePos.x + 12.0f, nodePos.y - textH - 6.0f,
                                        textW, textH);
            box.setX (juce::jlimit (graph.getX(), graph.getRight() - textW, box.getX()));
            box.setY (juce::jlimit (graph.getY(), graph.getBottom() - textH, box.getY()));
            g.setColour (juce::Colour (bgColour).withAlpha (0.85f));
            g.fillRoundedRectangle (box, 3.0f);
            g.setColour (juce::Colour (activeDotColour));
            g.drawText (readout, box, juce::Justification::centred, false);
        }
    }
}

void AdsrDisplayComponent::resized()
{
    repaint();
}
