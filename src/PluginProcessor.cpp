#include "PluginProcessor.h"
#include "PluginEditor.h"

BassSynthAudioProcessor::BassSynthAudioProcessor() :
    AudioProcessor(
        BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ),
    apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout()),
    params(apvts),
    synth(params)
{
}

bool BassSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono()
        || layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void BassSynthAudioProcessor::prepareToPlay(double newSampleRate, int maximumBlockSize)
{
    float sampleRate = float(newSampleRate);

    params.prepareToPlay(sampleRate);
    params.reset();

    synth.prepareToPlay(sampleRate, maximumBlockSize);
    synth.reset();
}

void BassSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear any output channels that don't contain input data.
    for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i) {
        buffer.clear(i, 0, buffer.getNumSamples());
    }

    params.update();

    float* bufferPtr = buffer.getWritePointer(0);
    int bufferOffset = 0;

    for (const auto metadata : midiMessages) {
        // Render the audio that happens before this MIDI event.
        int samplesThisSegment = metadata.samplePosition - bufferOffset;
        if (samplesThisSegment > 0) {
            synth.render(bufferPtr + bufferOffset, samplesThisSegment);
            bufferOffset += samplesThisSegment;
        }

        // Handle the MIDI event. Ignore messages such as sysex.
        if (metadata.numBytes <= 3) {
            uint8_t data1 = (metadata.numBytes >= 2) ? metadata.data[1] : 0;
            uint8_t data2 = (metadata.numBytes == 3) ? metadata.data[2] : 0;
            synth.midiMessage(metadata.data[0], data1, data2);
        }
    }

    // Render the audio after the last MIDI event. If there were no
    // MIDI events at all, this renders the entire buffer.
    int samplesLastSegment = buffer.getNumSamples() - bufferOffset;
    if (samplesLastSegment > 0) {
        synth.render(bufferPtr + bufferOffset, samplesLastSegment);
    }

    midiMessages.clear();

    // Stereo? Then copy left channel into right channel.
    if (getTotalNumOutputChannels() > 1) {
        buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
    }
}

void BassSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void BassSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessorEditor* BassSynthAudioProcessor::createEditor()
{
    return new BassSynthAudioProcessorEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassSynthAudioProcessor();
}
