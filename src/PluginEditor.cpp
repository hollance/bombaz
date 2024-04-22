#include "PluginProcessor.h"
#include "PluginEditor.h"

BassSynthAudioProcessorEditor::BassSynthAudioProcessorEditor(BassSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(400, 300);
}

BassSynthAudioProcessorEditor::~BassSynthAudioProcessorEditor()
{
}

void BassSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void BassSynthAudioProcessorEditor::resized()
{
}
