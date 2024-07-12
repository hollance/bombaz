#include "LookAndFeel.h"

LookAndFeel::LookAndFeel()
{
    setColour(juce::Slider::rotarySliderFillColourId, { 234, 17, 134 });

    setColour(juce::TooltipWindow::textColourId, juce::Colours::lightgrey);
    setColour(juce::BubbleComponent::backgroundColourId, juce::Colours::black);
    setColour(juce::BubbleComponent::outlineColourId, juce::Colours::lightgrey);

    auto dropShadow = juce::DropShadow(juce::Colours::black.withAlpha(0.5f), 2, {0, 1});
    bubbleShadow.setShadowProperties(dropShadow);
}

void LookAndFeel::drawRotarySlider(
     juce::Graphics& g,
     int x,
     int y,
     int width,
     [[maybe_unused]] int height,
     float sliderPos,
     float rotaryStartAngle,
     float rotaryEndAngle,
     [[maybe_unused]] juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, width).toFloat();
    auto lineWidth = 7.0f;
    auto arcRadius = (bounds.getWidth() - lineWidth) / 2.0f;

    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    juce::Path arc;
    arc.addCentredArc(
        bounds.getCentreX(),
        bounds.getCentreY(),
        arcRadius,
        arcRadius,
        0.0f,
        angle - 0.15f,
        angle + 0.15f,
        true);

    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    g.strokePath(arc, juce::PathStrokeType(lineWidth));
}

juce::Font LookAndFeel::getSliderPopupFont(juce::Slider&)
{
    return juce::Font(juce::FontOptions(12.0f));
}

int LookAndFeel::getSliderPopupPlacement(juce::Slider&)
{
    return juce::BubbleComponent::above;
}

void LookAndFeel::drawBubble(
    juce::Graphics& g,
    juce::BubbleComponent& comp,
    const juce::Point<float>& tip,
    const juce::Rectangle<float>& body)
{
    // Draw tip above or below the bubble?
    float adjustedY = tip.y + ((tip.y > body.getBottom()) ? -6.0f : 6.0f);

    juce::Path path;
    path.addBubble(
        body.reduced(0.5f),
        body.getUnion(juce::Rectangle<float>(tip.x, tip.y, 1.0f, 1.0f)),
        {tip.x, adjustedY},
        2.0f,
        juce::jmin(15.0f, body.getWidth() * 0.2f, body.getHeight() * 0.2f));

    g.setColour(comp.findColour(juce::BubbleComponent::backgroundColourId));
    g.fillPath(path);

    g.setColour(comp.findColour(juce::BubbleComponent::outlineColourId));
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void LookAndFeel::setComponentEffectForBubbleComponent(juce::BubbleComponent& bubbleComponent)
{
    bubbleComponent.setComponentEffect(&bubbleShadow);
}
