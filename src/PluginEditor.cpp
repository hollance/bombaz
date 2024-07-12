#include "PluginProcessor.h"
#include "PluginEditor.h"

BassSynthAudioProcessorEditor::BassSynthAudioProcessorEditor(BassSynthAudioProcessor& p) :
    AudioProcessorEditor(&p),
    audioProcessor(p),
    octaveAttachment(*p.params.octaveParam, [this](float f){ setOctaveValue(f); })
{
    audioProcessor.apvts.state.addListener(this);

    float pi = juce::MathConstants<float>::pi;
    tuningSlider.setRotaryParameters(1.25f * pi, 2.75f * pi, true);
    glideSlider.setRotaryParameters(tuningSlider.getRotaryParameters());

    tuningSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    tuningSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    tuningSlider.setPopupDisplayEnabled(true, true, this, -1);
    tuningSlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(tuningSlider);

    glideSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    glideSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    glideSlider.setPopupDisplayEnabled(true, true, this, -1);
    glideSlider.setLookAndFeel(&lookAndFeel);
    addAndMakeVisible(glideSlider);

    xyPad.xParameter = audioProcessor.params.widthParam;
    xyPad.yParameter = audioProcessor.params.driveParam;
    xyPad.updatePoint();
    addAndMakeVisible(xyPad);

    onImages[0] = juce::ImageCache::getFromMemory(BinaryData::On2_png, BinaryData::On2_pngSize);
    onImages[1] = juce::ImageCache::getFromMemory(BinaryData::On1_png, BinaryData::On1_pngSize);
    onImages[2] = juce::ImageCache::getFromMemory(BinaryData::On0_png, BinaryData::On0_pngSize);

    offImages[0] = juce::ImageCache::getFromMemory(BinaryData::Off2_png, BinaryData::Off2_pngSize);
    offImages[1] = juce::ImageCache::getFromMemory(BinaryData::Off1_png, BinaryData::Off1_pngSize);
    offImages[2] = juce::ImageCache::getFromMemory(BinaryData::Off0_png, BinaryData::Off0_pngSize);

    octaveButton.setClickingTogglesState(false);
    octaveButton.addListener(this);
    addAndMakeVisible(octaveButton);

    octaveAttachment.sendInitialUpdate();

    setOpaque(true);
    setSize(420, 290);
}

BassSynthAudioProcessorEditor::~BassSynthAudioProcessorEditor()
{
    octaveButton.removeListener(this);
    audioProcessor.apvts.state.removeListener(this);
}

void BassSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImage(backgroundImage, 0, 0, 420, 290, 0, 0, 840, 580);
}

void BassSynthAudioProcessorEditor::resized()
{
    xyPad.setBounds(20, 20, 250, 250);
    octaveButton.setBounds(328, 19, 37, 37);
    tuningSlider.setBounds(316, 92, 59, 59);
    glideSlider.setBounds(316, 194, 59, 59);
}

void BassSynthAudioProcessorEditor::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier&)
{
    xyPad.updatePoint();
    xyPad.repaint();
}

void BassSynthAudioProcessorEditor::setOctaveValue(float newValue)
{
    const juce::ScopedValueSetter<bool> svs(ignoreCallbacks, true);

    int newIndex = int(newValue);
    octaveButton.setImages(
        false, true, true,
        offImages[newIndex], 1.0f, juce::Colours::transparentBlack,
        juce::Image(), 1.0f, juce::Colours::transparentBlack,
        onImages[newIndex], 1.0f, juce::Colours::transparentBlack,
        0.0f);
}

void BassSynthAudioProcessorEditor::buttonClicked(juce::Button*)
{
    if (ignoreCallbacks) { return; }

    int currentIndex = audioProcessor.params.octaveParam->getIndex();
    int newIndex = (currentIndex + 1) % 3;
    octaveAttachment.setValueAsCompleteGesture(float(newIndex));
}
