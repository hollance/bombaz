#pragma once

#include <JuceHeader.h>

class XYPad : public juce::Component
{
public:
    XYPad();

    void paint(juce::Graphics&) override;

    void updatePoint();

    juce::AudioParameterFloat* xParameter = nullptr;
    juce::AudioParameterFloat* yParameter = nullptr;

private:
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    juce::Image pointerImage = juce::ImageCache::getFromMemory(
                    BinaryData::Pointer_png, BinaryData::Pointer_pngSize);
    juce::Image lineHorzImage = juce::ImageCache::getFromMemory(
                    BinaryData::LineHorz_png, BinaryData::LineHorz_pngSize);
    juce::Image lineVertImage = juce::ImageCache::getFromMemory(
                    BinaryData::LineVert_png, BinaryData::LineVert_pngSize);

    juce::Point<float> point;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPad)
};
