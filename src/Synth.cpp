#include "Synth.h"
#include "DSP.h"

Synth::Synth(Parameters& params_) : params(params_), voice(params) { }

void Synth::prepareToPlay(float sampleRate, int) noexcept
{
    voice.prepareToPlay(sampleRate);
    dcKiller.setCutoff(sampleRate, 27.5f);
}

void Synth::reset() noexcept
{
    voice.reset();
    dcKiller.reset();

    for (size_t i = 0; i < queue.size(); ++i) {
        queue[i] = -1;
    }

    activeNote = -1;
    sustainPedalPressed = false;
}

void Synth::render(float* outputBuffer, int sampleCount) noexcept
{
    voice.updateParameters();

    for (int sample = 0; sample < sampleCount; ++sample) {
        float output = voice.render();
        output = dcKiller.processSample(output);
        outputBuffer[sample] = output;
    }
}

void Synth::midiMessage(uint8_t data0, uint8_t data1, uint8_t data2) noexcept
{
    switch (data0 & 0xF0) {  // all channels
        case 0x80:
            noteOff(data1);
            break;

        case 0x90:
            if (data2 > 0) {
                noteOn(data1, data2);
            } else {
                noteOff(data1);
            }
            break;

        case 0xB0:
            controlChange(data1, data2);
            break;

        case 0xE0:
            pitchWheel(data1, data2);
            break;
    }
}

void Synth::noteOn(int note, int velocity) noexcept
{
    activeNote = note;

    removeNote(note);  // each note only once
    insertNote(note);

    voice.startOscillator(note, velocity);
}

void Synth::noteOff(int note) noexcept
{
    removeNote(note);

    if (activeNote == note) {
        activeNote = queue[0];
        if (activeNote != -1) {
            voice.restartOscillator(activeNote);
        } else if (!sustainPedalPressed) {
            voice.stopOscillator();
        }
    }
}

void Synth::pitchWheel(uint8_t data1, uint8_t data2) noexcept
{
    int amount = int(data1) + 128*int(data2) - 8192;

    // The pitch bend multiplier is 2^(semitones * (amount / 8192) / 12),
    // which is exp(x * amount) where x = ln(2^(semitones / (8192 * 12))).
    const float multiplier = 0.0000493574f;  // 7 semitones

    params.pitchBend = std::exp(multiplier * float(amount));
}

void Synth::controlChange(uint8_t data1, uint8_t data2) noexcept
{
    switch (data1) {
        case 0x40:
            sustainPedalPressed = (data2 >= 64);

            if (!sustainPedalPressed && activeNote == -1) {
                voice.stopOscillator();
            }
            break;

        default:
            if (data1 >= 0x78) {  // all notes off
                reset();
            }
            break;
    }
}

void Synth::insertNote(int note) noexcept
{
    for (size_t i = queue.size() - 1; i > 0; --i) {
        queue[i] = queue[i - 1];
    }
    queue[0] = note;
}

void Synth::removeNote(int note) noexcept
{
    for (size_t i = 0; i < queue.size(); ++i) {
        if (queue[i] == note) {
            for (; i < queue.size() - 1; ++i) {
                queue[i] = queue[i + 1];
            }
            queue[i] = -1;
            return;
        }
    }
}
