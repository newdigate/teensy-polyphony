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

#ifndef TEENSY_AUDIO_SAMPLER_H
#define TEENSY_AUDIO_SAMPLER_H

#include "polyphonicsampler.h"
#include "playarrayresmp.h"
#include <vector>
#include "effect_envelope.h"
#include "mixer.h"

//template <unsigned MAX_NUM_POLYPHONY>
class audiosample {
public:
    audiosample(uint8_t noteNumber, int16_t *data, uint32_t sampleLength, uint16_t numChannels) : _noteNumber(noteNumber), _data(data), _sampleLength(sampleLength), _numChannels(numChannels) {
    }

    uint8_t _noteNumber;    
    int16_t *_data; 
    uint32_t _sampleLength;
    uint16_t _numChannels;
private:
};

class audiovoice {
public:
    audiovoice(AudioPlayArrayResmp *audioplayarray, AudioEffectEnvelope *audioenvelop,  AudioEffectEnvelope *audioenvelop2, AudioMixer4 *audiomixer, AudioMixer4 *audiomixer2, uint8_t mixerChannel, uint8_t mixerChannel2) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop),
        _audioenvelop2(audioenvelop2),
        _audiomixer(audiomixer),
        _audiomixer2(audiomixer2),
        _mixerChannel(mixerChannel),
        _mixerChannel2(mixerChannel2),
        _isStereo(true) {
    }

    audiovoice(AudioPlayArrayResmp *audioplayarray, AudioEffectEnvelope *audioenvelop, AudioMixer4 *audiomixer, uint8_t mixerChannel) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop),
        _audiomixer(audiomixer),
        _mixerChannel(mixerChannel)
    {
    }

    audiovoice(AudioPlayArrayResmp *audioplayarray) : 
        _audioplayarray(audioplayarray) {
    }

    audiovoice(AudioPlayArrayResmp *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel) :
        _audioplayarray(audioplayarray), 
        _audiomixer(audiomixer),
        _mixerChannel(mixerChannel) {
    }
    audiovoice(AudioPlayArrayResmp *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel, AudioMixer4 *audiomixer2, uint8_t mixerChannel2) :
        _audioplayarray(audioplayarray), 
        _audiomixer(audiomixer),
        _audiomixer2(audiomixer2),
        _mixerChannel(mixerChannel),
        _mixerChannel2(mixerChannel2) {
    }
    AudioPlayArrayResmp *_audioplayarray = nullptr;

    AudioEffectEnvelope *_audioenvelop = nullptr;
    AudioEffectEnvelope *_audioenvelop2 = nullptr;
    AudioMixer4 *_audiomixer = nullptr;
    AudioMixer4 *_audiomixer2 = nullptr;

    uint8_t _mixerChannel;
    uint8_t _mixerChannel2;
    bool _isStereo = false;
};

class sampler {
public:
    sampler() : _polysampler() {
        _polysampler.setNoteEventCallback( [&] (uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger) {
            noteEventCallback(voice, noteNumber, velocity, isNoteOn, retrigger);
        });
    }

    void noteEvent(uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger) {
        if (isNoteOn && velocity > 0)
            _polysampler.noteOn(noteNumber, velocity);
        else 
            _polysampler.noteOff(noteNumber);
    }

    void addSample(uint8_t noteNumber, int16_t *data, uint32_t sampleLength, uint16_t numChannels) {
        audiosample *newSample = new audiosample(noteNumber, data, sampleLength, numChannels);
        _audiosamples.push_back(newSample);
    }
    void addVoice(AudioPlayArrayResmp &audioplayarrayresmp, AudioMixer4 &mixer, uint8_t mixerChannel, AudioEffectEnvelope &envelope, AudioMixer4 &mixer2, uint8_t mixerChannel2, AudioEffectEnvelope &envelope2) {

        audiovoice *voice = new audiovoice(&audioplayarrayresmp, &envelope, &envelope2, &mixer, &mixer2, mixerChannel, mixerChannel2);
        addVoice(voice) ;
    }

    void addVoice(AudioPlayArrayResmp &audioplayarrayresmp, AudioMixer4 &mixer, uint8_t mixerChannel, AudioEffectEnvelope &envelope) {

        audiovoice *voice = new audiovoice(&audioplayarrayresmp, &envelope, &mixer, mixerChannel);
        addVoice(voice) ;
    }

    void addVoice(AudioPlayArrayResmp &audioplayarrayresmp, AudioMixer4 &mixer, uint8_t mixerChannel) {

        audiovoice *voice = new audiovoice(&audioplayarrayresmp, nullptr, &mixer, mixerChannel);
        addVoice(voice) ;
    }

    void addVoice(AudioPlayArrayResmp &audioplayarrayresmp) {
        audiovoice *voice = new audiovoice(&audioplayarrayresmp, nullptr, nullptr, 0);
        addVoice(voice) ;
    }

    void addVoices(AudioPlayArrayResmp **voices, uint8_t numOfVoicesToAdd){
        for (int i = 0; i < numOfVoicesToAdd; i++){
            addVoice(*voices[i]);
        }
    }
    
    void addVoices(AudioPlayArrayResmp *voices, uint8_t numOfVoicesToAdd){
        for (int i = 0; i < numOfVoicesToAdd; i++){
            addVoice(voices[i]);
        }
    }

private:
    uint8_t _numVoices = 0;
    std::vector<audiovoice*> _voices;
    polyphonicsampler _polysampler;
    
    std::vector<audiosample*> _audiosamples;

    static float calcFrequency(uint8_t note) {
        float result = 440.0 * powf(2.0, (note-69) / 12.0);
        return result;
    }

    static float calcPitchFactor(uint8_t note, uint8_t rootNoteNumber) {
        float result = powf(2.0, (note-rootNoteNumber) / 12.0);
        return result;
    }

    void noteEventCallback(uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger)
    {
        if (voice < _numVoices) {
            if (isNoteOn) {
                audiosample *nearestSample = findNearestSampleForKey(noteNumber);
                if (nearestSample != nullptr) {
                    float factor = calcPitchFactor(noteNumber, nearestSample->_noteNumber);
                    _voices[voice]->_audioplayarray->setPlaybackRate(factor);
                    
                    if (_voices[voice]->_audiomixer != nullptr) {                        
                        _voices[voice]->_audiomixer->gain(_voices[voice]->_mixerChannel, velocity / 255.0);
                    }
                    if (_voices[voice]->_audiomixer2 != nullptr) {                        
                        _voices[voice]->_audiomixer2->gain(_voices[voice]->_mixerChannel, velocity / 255.0);
                    }
                    if (_voices[voice]->_audioenvelop != nullptr) {
                        _voices[voice]->_audioenvelop->noteOn();
                    }
                    if (_voices[voice]->_audioenvelop2 != nullptr) {
                        _voices[voice]->_audioenvelop2->noteOn();
                    }
                    _voices[voice]->_audioplayarray->playRaw(nearestSample->_data, nearestSample->_sampleLength, nearestSample->_numChannels);
                }
            } else {
                // Note off event
                if (_voices[voice]->_audioenvelop != nullptr) {
                    _voices[voice]->_audioenvelop->noteOff();
                }
                
                if (_voices[voice]->_audioenvelop2 != nullptr) {
                    _voices[voice]->_audioenvelop2->noteOff();
                }

            }
        }
    }

    audiosample* findNearestSampleForKey(uint8_t noteNumber) {
        uint8_t smallestDiff = 255;
        audiosample *candidate = nullptr;
        for (auto &&x : _audiosamples) {
            uint8_t diff = abs(x->_noteNumber - noteNumber);
            if (diff < smallestDiff) {
                smallestDiff = diff;
                candidate = x;
            }
        }
        return candidate;
    }
    
    void addVoice(audiovoice *voice){

        _voices.push_back(voice);
        _voices.begin();
        _numVoices++;
        _polysampler.setNumVoices(_numVoices);
    }
};

#endif