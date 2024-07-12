#pragma once

#include <JuceHeader.h>

class LookAndFeel : public juce::LookAndFeel_V4
{
public:
    LookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    juce::Font getSliderPopupFont(juce::Slider&) override;

    int getSliderPopupPlacement(juce::Slider&) override;

    void drawBubble(juce::Graphics&, juce::BubbleComponent&,
                    const juce::Point<float>&, const juce::Rectangle<float>&) override;

    void setComponentEffectForBubbleComponent(juce::BubbleComponent& bubbleComponent) override;

private:
    juce::DropShadowEffect bubbleShadow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LookAndFeel)
};
