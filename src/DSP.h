#pragma once

inline float gainToDecibels(float value) noexcept
{
    return 8.6858896381f * std::log(value);  // 20 * log10(x)
}

inline float decibelsToGain(float value) noexcept
{
    return std::exp(0.1151292546f * value);  // 10^(x/20)
}

/**
  Returns the pitch of a note using twelve-tone equal temperament (12-TET).
 */
inline float noteToHz(int midiNoteNumber) noexcept
{
    return 440.0f * std::exp2(float(midiNoteNumber - 69) / 12.0f);
}

/**
  How much to multiply the current pitch by, in order to transpose up or down
  by the specified number of semitones.
*/
inline float pitchMultiplierForSemitones(float semitones) noexcept
{
    if (semitones == 0.0f) {
        return 1.0f;
    } else {
        return std::exp2(semitones / 12.0f);
    }
}

/**
  Returns the number of semitones between two frequencies. Useful for portamento
  and modulating a filter's cutoff frequency.
 */
inline float semitonesBetweenFrequencies(float fromFrequency, float toFrequency) noexcept
{
    return std::log2(toFrequency / fromFrequency) * 12.0f;
}
