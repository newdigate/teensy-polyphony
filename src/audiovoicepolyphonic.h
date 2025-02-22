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

#ifndef TEENSYAUDIO_LAUNCHCTRL_AUDIOVOICEPOLYPHONIC_H
#define TEENSYAUDIO_LAUNCHCTRL_AUDIOVOICEPOLYPHONIC_H

#include <vector>
#include <stdint.h>
#include <Arduino.h>
#include "sampler.h"

template <typename TVoice>
class audiovoicepolyphonic : public polyphonic<audiovoice<TVoice>>{
public:
    audiovoicepolyphonic() : polyphonic<audiovoice<TVoice>>() {
    }

    audiovoicepolyphonic(const audiovoicepolyphonic&) = delete;

    virtual ~audiovoicepolyphonic() {
    }

    void addVoice(TVoice *audioplayarray, AudioEffectEnvelope *audioenvelop,  AudioEffectEnvelope *audioenvelop2, AudioMixer4 *audiomixer, AudioMixer4 *audiomixer2, uint8_t mixerChannel, uint8_t mixerChannel2) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray, audioenvelop,  audioenvelop2, audiomixer,  audiomixer2, mixerChannel, mixerChannel2);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }

    void addVoice(TVoice *audioplayarray, AudioEffectEnvelope *audioenvelop, AudioMixer4 *audiomixer, uint8_t mixerChannel) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray,  audioenvelop, audiomixer, mixerChannel);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }

    void addVoice(TVoice *audioplayarray, AudioEffectEnvelope *audioenvelop) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray, audioenvelop);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }

    void addVoice(TVoice *audioplayarray, AudioEffectEnvelope *audioenvelop1, AudioEffectEnvelope *audioenvelop2) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray, audioenvelop1, audioenvelop2);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }

    void addVoice(TVoice *audioplayarray) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }

    void addVoice(TVoice *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray, audiomixer, mixerChannel);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }

    void addVoice(TVoice *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel, AudioMixer4 *audiomixer2, uint8_t mixerChannel2) {
        audiovoice<TVoice> *voice = new audiovoice<TVoice>(audioplayarray, audiomixer, mixerChannel, audiomixer2, mixerChannel2);
        polyphonic<audiovoice<TVoice>>::addVoice(*voice);
    }
 
private:
};

#endif //TEENSYAUDIO_LAUNCHCTRL_POLYPHONIC_H
