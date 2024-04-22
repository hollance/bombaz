#pragma once

#include "Oscillator.h"
#include "Parameters.h"

class Voice
{
public:
    Voice(const Parameters& params_) : params(params_) { }

    void prepareToPlay(float sampleRate) noexcept
    {
        osc.prepareToPlay(sampleRate);
    }

    void reset() noexcept
    {
        osc.reset();
        osc.setAmplitude(1.0f);

        pitch = 0.0f;
        target = 0.0f;
        glide = 1.0f;
        glideDir = 0.0f;
    }

    void updateParameters() noexcept
    {
        osc.setPitchModulation(params.pitchModulation);
        osc.setPulseTime(params.pulseTime);
        osc.setDrive(params.drive);
    }

    float render() noexcept
    {
        if (osc.isActive()) {
            if (glide != 1.0f) {
                pitch *= glide;
                if (pitch * glideDir >= target) {
                    pitch = target * glideDir;
                    glide = 1.0f;
                }
            }
            osc.setPitch(pitch);
            return osc.nextSample();
        } else {
            return 0.0f;
        }
    }

    void startOscillator(int note, int) noexcept
    {
        updatePitchTarget(note, target != 0.0f);

        if (glide == 1.0f) {  // not playing legato
            osc.setPitch(pitch);
            osc.startPlaying();
        }
    }

    void restartOscillator(int note) noexcept
    {
        updatePitchTarget(note, true);
    }

    void stopOscillator() noexcept
    {
        pitch = 0.0f;
        target = 0.0f;
    }

private:
    float pitchForNote(int note) noexcept
    {
        return noteToHz(std::clamp(note + params.transpose, 0, 127));
    }

    void updatePitchTarget(int note, bool isPlayingLegato) noexcept
    {
        target = pitchForNote(note);

        if (isPlayingLegato && params.glideTime > 0.0f) {
            float semitones = semitonesBetweenFrequencies(pitch, target);
            if (semitones != 0.0f) {
                glide = std::exp2(semitones / (12.0f * params.glideTime));
                glideDir = (glide > 1.0f) ? 1.0f : -1.0f;
                target *= glideDir;
                return;
            }
        }

        glide = 1.0f;
        glideDir = 0.0f;
        pitch = target;
    }

    const Parameters& params;
    Oscillator osc;
    float pitch;
    float target, glide, glideDir;
};
