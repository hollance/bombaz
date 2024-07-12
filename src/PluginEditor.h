#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "XYPad.h"

class BassSynthAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      private juce::ValueTree::Listener,
                                      private juce::Button::Listener
{
public:
    BassSynthAudioProcessorEditor(BassSynthAudioProcessor&);
    ~BassSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setOctaveValue(float newValue);
    void buttonClicked(juce::Button* button) override;
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&) override;

    BassSynthAudioProcessor& audioProcessor;

    LookAndFeel lookAndFeel;

    XYPad xyPad;
    juce::Slider tuningSlider;
    juce::Slider glideSlider;

    juce::ImageButton octaveButton;
    juce::Image offImages[3];
    juce::Image onImages[3];

    juce::AudioProcessorValueTreeState::SliderAttachment tuningAttachment {
        audioProcessor.apvts, ParameterID::tuning.getParamID(), tuningSlider
    };
    juce::AudioProcessorValueTreeState::SliderAttachment glideAttachment {
        audioProcessor.apvts, ParameterID::glideTime.getParamID(), glideSlider
    };

    juce::ParameterAttachment octaveAttachment;
    bool ignoreCallbacks = false;

    juce::Image backgroundImage = juce::ImageCache::getFromMemory(
                BinaryData::Background_png, BinaryData::Background_pngSize);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BassSynthAudioProcessorEditor)
};
