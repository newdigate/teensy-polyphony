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

    bool isPlaying() {
        return true;
    }
};

class stringnote {
public:
    float _freq;
};

class stringsampler : polyphonicsampler<stringvoice, stringnote> {
public:
    stringsampler(polyphonic<stringvoice> polyphony) : 
        polyphonicsampler<stringvoice, stringnote>(polyphony)
    {
    }

    stringsampler(const stringsampler&) = delete;
    
    virtual ~stringsampler() {}

    virtual bool noteDownEventCallback(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t noteVelocity, bool retrigger){
        if (voice == nullptr)
            return false;

        voice->_strings->noteOn(sample->_freq, noteVelocity/255.0);
        if (voice->_envelop != nullptr)
            voice->_envelop->noteOn();

        return true;
    }

    virtual void noteUpBeginEventCallback(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        if (voice->_envelop != nullptr)
            voice->_envelop->noteOff();
    }

    virtual void noteEndBeginEventCallback(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        voice->_strings->noteOff(0);
    }
    
    virtual bool isVoiceStillActive(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel)
    {
       return true;
    }

    virtual bool isVoiceStillNoteDown(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        return true;
    }

    virtual stringnote* findSampleCallback(uint8_t noteNumber, uint8_t noteChannel) {
        stringnote *result = new stringnote();
        result->_freq = calcFrequency(noteNumber);
        return result;
    }

    virtual triggertype findTriggerType(uint8_t noteNumber, uint8_t noteChannel) {
        return _staticTriggerType;
    }

    void noteUpBeginEvent(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        voice->_envelop->noteOff();
    }

    void noteUpEndEvent(stringvoice *voice, stringnote *sample, uint8_t noteNumber, uint8_t noteChannel) {
        voice->_strings->noteOff(0);
    }


private:
    
    static float calcFrequency(uint8_t note) {
        float result = 440.0 * powf(2.0, (note-69) / 12.0);
        return result;
    }
};

#endif