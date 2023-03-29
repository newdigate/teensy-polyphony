/* Audio Library for Teensy
 * Copyright (c) 2021, Nic Newdigate
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef TEENSY_AUDIO_STRINGSAMPLER_H
#define TEENSY_AUDIO_STRINGSAMPLER_H

#include <vector>
#include "polyphonicsampler.h"
#include "synth_karplusstrong.h"
#include "effect_envelope.h"

class stringvoice {
public:
    stringvoice(AudioSynthKarplusStrong *strings) :
        _envelop(nullptr),
        _strings(strings) {
    }

    stringvoice(AudioSynthKarplusStrong *strings, AudioEffectEnvelope *envelop) :
        _envelop(envelop), 
        _strings(strings) {
    }

    AudioEffectEnvelope *_envelop = nullptr;
    AudioSynthKarplusStrong *_strings = nullptr;
};

class stringnote {
public:
    float _freq;
};

class stringsampler {
public:
    stringsampler(polyphonic<stringvoice> polyphony) : 
            _polysampler(
                polyphony, 
                // noteDownEventFunction
                [&] (stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t noteVelocity, bool retrigger) -> bool {
                    return noteDownEvent(voice, sample, noteNumber, noteChannel, noteVelocity, retrigger);
                },
                // noteUpBeginEventFunction
                [&] (stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
                    noteUpBeginEvent(voice, sample, noteNumber, noteChannel);
                },
                // noteUpEndEventFunction
                [&] (stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
                    noteUpEndEvent(voice, sample, noteNumber, noteChannel);
                },
                [&] (uint8_t noteNumber, uint8_t noteChannel) -> stringnote* { 
                    stringnote *result = new stringnote();
                    result->_freq = calcFrequency(noteNumber);
                    return result;
                },
                // isVoiceStillActiveFunction
                [&] (stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) -> bool {
                    return false;
                },
                // isVoiceStillNoteDownFunction
                [&] (stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) -> bool {
                    return true;
                })
    {
    }

    stringsampler(const stringsampler&) = delete;
    virtual ~stringsampler() {
    }

    bool noteDownEvent(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool retrigger) {
        if (voice == nullptr)
            return false;

        voice->_strings->noteOn(sample->_freq, velocity/255.0);
        voice->_envelop->noteOn();
        return true;
    }

    void noteUpBeginEvent(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        voice->_envelop->noteOff();
    }

    void noteUpEndEvent(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        voice->_strings->noteOff(0);
    }

    void turnOffAllNotesStillPlaying() {
        _polysampler.turnOffAllNotesStillPlaying();
    }

private:
    polyphonicsampler<stringvoice, stringnote> _polysampler;
    
    static float calcFrequency(uint8_t note) {
        float result = 440.0 * powf(2.0, (note-69) / 12.0);
        return result;
    }

    void noteEventCallback(stringvoice *voice, stringnote *note, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool isNoteOn, bool retrigger)
    {
        if (voice != nullptr) {
            if (isNoteOn) {
                float freq = calcFrequency(noteNumber);

                if (voice->_envelop != nullptr) {
                    voice->_envelop->noteOn();
                }
                voice->_strings->noteOn(freq, velocity / 255.0);
            } else {
                // Note off event
                if (voice->_envelop != nullptr) {
                    voice->_envelop->noteOff();
                }
            }
        }
    }
};

#endif