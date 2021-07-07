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
    audiosample(uint8_t noteNumber, int16_t *data, uint32_t sampleLength) : _noteNumber(noteNumber), _data(data), _sampleLength(sampleLength) {
    }

    uint8_t _noteNumber;    
    int16_t *_data; 
    uint32_t _sampleLength;
private:
};

class audiovoice {
public:
    audiovoice(AudioPlayArrayResmp *audioplayarray, AudioEffectEnvelope *audioenvelop, AudioMixer4 *audiomixer, uint8_t mixerChannel) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(audioenvelop),
        _audiomixer(audiomixer),
        _mixerChannel(mixerChannel) {
    }

    audiovoice(AudioPlayArrayResmp *audioplayarray) : 
        _audioplayarray(audioplayarray), 
        _audioenvelop(nullptr),
        _audiomixer(nullptr),
        _mixerChannel(0) {
    }

    audiovoice(AudioPlayArrayResmp *audioplayarray, AudioMixer4 *audiomixer, uint8_t mixerChannel) :
        _audioplayarray(audioplayarray), 
        _audioenvelop(nullptr),
        _audiomixer(audiomixer),
        _mixerChannel(mixerChannel) {
    }

    AudioPlayArrayResmp *_audioplayarray;
    AudioEffectEnvelope *_audioenvelop;
    AudioMixer4 *_audiomixer;
    uint8_t _mixerChannel;
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

    void addSample(uint8_t noteNumber, int16_t *data, uint32_t sampleLength) {
        audiosample *newSample = new audiosample(noteNumber, data, sampleLength);
        _audiosamples.push_back(newSample);
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

    static double calcFrequency(uint8_t note) {
        double result = 440.0 * pow(2.0, (note-69) / 12.0);
        return result;
    }

    static double calcPitchFactor(uint8_t note, uint8_t rootNoteNumber) {
        double result = pow(2.0, (note-rootNoteNumber) / 12.0);
        return result;
    }
    void noteEventCallback(uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger)
    {
        if (voice < _numVoices) {
            if (isNoteOn) {
                audiosample *nearestSample = findNearestSampleForKey(noteNumber);
                if (nearestSample != nullptr) {
                    double factor = calcPitchFactor(noteNumber, nearestSample->_noteNumber);
                    _voices[voice]->_audioplayarray->setPlaybackRate(factor);
                    _voices[voice]->_audioplayarray->play(nearestSample->_data, nearestSample->_sampleLength);
                    if (_voices[voice]->_audiomixer != nullptr) {                        
                        _voices[voice]->_audiomixer->gain(_voices[voice]->_mixerChannel, velocity / 255.0);
                    }
                    if (_voices[voice]->_audioenvelop != nullptr) {
                        _voices[voice]->_audioenvelop->noteOn();
                    }
                }
            } else {
                // Note off event
                if (_voices[voice]->_audioenvelop != nullptr) {
                    _voices[voice]->_audioenvelop->noteOff();
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