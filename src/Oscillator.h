#pragma once

#include <cmath>
#include "DSP.h"

class Oscillator
{
public:
    Oscillator() noexcept
    {
        createWavetable();
    }

    void prepareToPlay(float sampleRate_) noexcept
    {
        sampleRate = sampleRate_;
    }

    void reset() noexcept
    {
        pulseTime = 0.0f;
        newPulseTime = 0.0f;

        amplitude = 0.0f;
        newAmplitude = 0.0f;

        pitch = 0.0f;
        newPitch = 0.0f;

        modulation = 1.0f;

        drive = 0.0f;
        newDrive = 0.0f;
        k = 0.0f;
        oneOverAtanK = 1.0f;

        period = 0.0f;
        phase = 0.0f;
        pos = 0.0f;
        inc = 0.0f;

        compensation = 1.0f;
    }

    void setPulseTime(float pulseTime_) noexcept
    {
        newPulseTime = pulseTime_;
    }

    void setAmplitude(float amplitude_) noexcept
    {
        newAmplitude = amplitude_;
    }

    void setPitch(float pitch_) noexcept
    {
        newPitch = pitch_;
    }

    void setPitchModulation(float modulation_) noexcept
    {
        modulation = modulation_;
    }

    void setDrive(float drive_) noexcept
    {
        newDrive = drive_;
    }

    void startPlaying() noexcept
    {
        // Not playing yet? Then start immediately. Otherwise, wait
        // until next cycle before changing the pitch and amplitude.
        if (pitch == 0.0f) {
            phase = 0.0f;
            pos = 0.0f;
            applyChanges();
        }
    }

    bool isActive() const noexcept
    {
        return pitch > 0.0f;
    }

    float nextSample() noexcept
    {
        float output = 0.0f;

        // Output the pulse by playing through the wavetable once,
        // followed by dead time for the remainder of the period.
        if (pos < size) {
            output = readFromWavetable(pos);

            // Use half the Blackman-Harris window as waveshaper.
            {
                float wet = readFromWavetable(output * size * 0.5f);
                output += drive * 0.8f * (wet - output);
            }

            // Soft-clipping waveshaper.
            {
                float wet = std::atan(k * output) * oneOverAtanK;
                output += drive * (wet - output);
            }

            output *= amplitude;
            pos += inc;
        }

        // Done with this cycle?
        phase += 1.0f;
        if (phase >= period) {
            phase -= period;
            pos = phase * inc;  // restart wavetable
            applyChanges();
        }

        return output;
    }

private:
    void createWavetable() noexcept
    {
        // The table is 1024 elements but only use 1023 of them. The last
        // element is the same as the first one, for easier interpolation.
        size = float(wavetable.size() - 1);

        // Create a pulse shaped like a Blackman-Harris window (asymmetric).
        for (int i = 0; i < int(wavetable.size()); ++i) {
            float arg = 6.2831853071795864f * float(i) / size;
            wavetable[i] = 0.35875f - 0.48829f * std::cos(arg)
                                    + 0.14128f * std::cos(2.0f * arg)
                                    - 0.01168f * std::cos(3.0f * arg);
        }
    }

    float readFromWavetable(float readIndex) const noexcept
    {
        // Linearly interpolate the wavetable.
        int x = int(readIndex);
        float a = readIndex - float(x);
        return a * wavetable[x + 1] + (1.0f - a) * wavetable[x];
    }

    void applyChanges() noexcept
    {
        updateLoudnessCompensation();

        pulseTime = newPulseTime;
        pitch = newPitch * modulation;
        amplitude = newAmplitude * compensation;

        drive = newDrive;
        k = drive*5.0f + 1.0f;
        oneOverAtanK = 1.0f / std::atan(k);

        updatePeriod();
        updateIncrement();
    }

    void updatePeriod() noexcept
    {
        if (pitch > 0.0f) {
            period = sampleRate / pitch;
        } else {
            period = 0.0f;
        }
    }

    void updateIncrement() noexcept
    {
        float pulseWidth = pulseTime * sampleRate;

        // If the pulse is wider than the period, use the period instead of
        // the pulse width. This does change the spectral shape somewhat but
        // doesn't introduce new harmonics.
        if (pulseWidth > period) {
            if (period > 0.0f) {
                inc = size / period;
            } else {
                inc = 0.0f;
            }
        } else {
            if (pulseWidth > 0.0f) {
                inc = size / pulseWidth;
            } else {
                inc = 0.0f;
            }
        }
    }

    void updateLoudnessCompensation() noexcept
    {
        if ((newPitch != pitch && newPitch > 0.0) || (newPulseTime != pulseTime)) {
            // Because lower notes have more deadtime, they sound more quiet,
            // so compensate by making higher notes quieter.

            // Only do this in the range 27.5 Hz - 440 Hz.
            float f = std::clamp(newPitch, 27.5f, 440.0f);

            // The multiplier at 27.5 Hz = 1.0. Every octave reduces this by 0.2,
            // so that 55 Hz = 0.8, 110 Hz = 0.6, 220 Hz = 0.4, and 440 Hz = 0.2.
            // This more-or-less gives the same RMS at all these frequencies.
            compensation = 1.0f - 0.2f * std::log2(f / 27.5f);

            // Longer pulse widths have a lower amplitude because of the DC
            // killer. Compensate for this by boosting by 3 dB at the longest
            // pulse width up to 0 dB for the shortest pulse.
            float dB = (newPulseTime - 0.003f) * 250.0f;
            compensation *= decibelsToGain(dB);
        }
    }

    float sampleRate;

    float pulseTime, newPulseTime;
    float pitch, newPitch;
    float amplitude, newAmplitude;
    float modulation;

    float drive, newDrive;
    float k, oneOverAtanK;  // for waveshaper

    float period, phase;
    float pos, inc, size;  // for wavetable playback
    float compensation;

    std::array<float, 1024> wavetable;
};
