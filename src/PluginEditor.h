#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class BassSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    BassSynthAudioProcessorEditor(BassSynthAudioProcessor&);
    ~BassSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BassSynthAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BassSynthAudioProcessorEditor)
};
