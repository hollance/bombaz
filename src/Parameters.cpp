#include <JuceHeader.h>
#include "DSP.h"
#include "Parameters.h"

template<typename T>
inline void castParameter(juce::AudioProcessorValueTreeState& apvts,
                          const juce::ParameterID& id, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(destination);  // parameter does not exist or wrong type
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts)
{
    castParameter(apvts, ParameterID::width, widthParam);
    castParameter(apvts, ParameterID::drive, driveParam);
    castParameter(apvts, ParameterID::octave, octaveParam);
    castParameter(apvts, ParameterID::tuning, tuningParam);
    castParameter(apvts, ParameterID::glideTime, glideTimeParam);
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::width,
        "Width",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f, 1.0f),
        50.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::drive,
        "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.01f, 1.0f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("%")));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        ParameterID::octave,
        "Octave",
        juce::StringArray { "-2", "-1", "0" },
        1));

    auto tuningStringFromValue = [](float value, int)
    {
        return juce::String(int(value)) + " cents";
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::tuning,
        "Tune",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(tuningStringFromValue)));

    auto glideTimeStringFromValue = [](float value, int)
    {
        if (value < 10.0f) {
            return juce::String("off");
        } else if (value < 1000.0f) {
            return juce::String(int(value)) + " ms";
        } else {
            return juce::String(value / 1000.0f, 2) + " s";
        }
    };

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        ParameterID::glideTime,
        "Glide",
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.5f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(glideTimeStringFromValue)));

    return layout;
}

void Parameters::prepareToPlay(float sampleRate_) noexcept
{
    sampleRate = sampleRate_;
}

void Parameters::reset() noexcept
{
    pulseTime = 0.003f;
    transpose = 0;
    tuning = 0.0f;
    glideTime = 0.0f;
    drive = 0.0f;
    pitchBend = 1.0f;
    pitchModulation = 1.0f;
}

void Parameters::update() noexcept
{
    pulseTime = 0.015f - widthParam->get() * 0.00012f;  // 15 - 3 msec

    static int semitones[] = { -24, -12, 0 };
    transpose = semitones[octaveParam->getIndex()];

    tuning = tuningParam->get();
    drive = driveParam->get() * 0.01f;

    float glideTimeInMsec = glideTimeParam->get();
    if (glideTimeInMsec < 10.0f) {
        glideTime = 0.0f;  // no glide
    } else {
        glideTime = sampleRate * glideTimeInMsec * 0.001f;
    }

    pitchModulation = pitchBend;
    if (tuning != 0.0f) {
        pitchModulation *= std::exp2(tuning / 1200.0f);
    }
}
