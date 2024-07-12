#pragma once

namespace ParameterID
{
    const juce::ParameterID width { "width", 1 };
    const juce::ParameterID drive { "drive", 1 };
    const juce::ParameterID octave { "octave", 1 };
    const juce::ParameterID tuning { "tuning", 1 };
    const juce::ParameterID glideTime { "glideTime", 1 };
}

struct Parameters
{
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    void prepareToPlay(float sampleRate) noexcept;
    void reset() noexcept;
    void update() noexcept;

    // *** Plug-in parameters ***

    juce::AudioParameterFloat* widthParam;
    juce::AudioParameterFloat* driveParam;
    juce::AudioParameterChoice* octaveParam;
    juce::AudioParameterFloat* tuningParam;
    juce::AudioParameterFloat* glideTimeParam;

    // *** Parameter values ***

    float sampleRate;
    float pulseTime;  // seconds
    int transpose;    // semitones
    float tuning;     // cents
    float glideTime;  // in samples
    float drive;      // 0 - 1

    // *** MIDI CC ***

    float pitchBend;

    // *** Derived values ***

    float pitchModulation;
};
