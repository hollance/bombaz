#pragma once

#include <JuceHeader.h>
#include "DCBlockFilter.h"
#include "Voice.h"

class Synth
{
public:
    Synth(Parameters& params);

    void prepareToPlay(float sampleRate, int maximumBlockSize) noexcept;
    void reset() noexcept;
    void render(float* outputBuffer, int sampleCount) noexcept;
    void midiMessage(uint8_t data0, uint8_t data1, uint8_t data2) noexcept;

private:
    void noteOn(int note, int velocity) noexcept;
    void noteOff(int note) noexcept;
    void pitchWheel(uint8_t data1, uint8_t data2) noexcept;
    void controlChange(uint8_t data1, uint8_t data2) noexcept;

    void insertNote(int note) noexcept;
    void removeNote(int note) noexcept;

    Parameters& params;

    Voice voice;
    int activeNote;   // -1 if not playing
    bool sustainPedalPressed;
    std::array<int, 10> queue;

    DCBlockFilter dcKiller;
};
