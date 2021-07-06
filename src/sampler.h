#ifndef TEENSY_AUDIO_SAMPLER_H
#define TEENSY_AUDIO_SAMPLER_H

#include "polyphonicsampler.h"
#include "playarrayresmp.h"
#include <vector>

//template <unsigned MAX_NUM_POLYPHONY>
class sampler {
public:
    sampler() : _polysampler() {
        _polysampler.setNoteEventCallback( [&] (uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger) {
            noteEventCallback(voice, noteNumber, velocity, isNoteOn, retrigger);
        });
    }

    void begin(int16_t *array, uint32_t numSamples) {
        _data = array;
        _numSamples = numSamples;
    }

    void noteEvent(uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger) {
        if (isNoteOn && velocity > 0)
            _polysampler.noteOn(noteNumber, velocity);
        else 
            _polysampler.noteOff(noteNumber);
    }

    void addVoice(AudioPlayArrayResmp &voice){
        _voices.push_back(&voice);
        _voices.begin();
        _numVoices++;
        _polysampler.setNumVoices(_numVoices);
    }

    void addVoices(AudioPlayArrayResmp *voices, uint8_t numOfVoicesToAdd){
        for (int i = 0; i < numOfVoicesToAdd; i++){
            addVoice(voices[i]);
        }
    }
    
private:
    uint8_t _numVoices = 0;
    std::vector<AudioPlayArrayResmp*> _voices;
    polyphonicsampler _polysampler;
    int16_t *_data; 
    uint32_t _numSamples;

    static double calcFrequency(uint8_t note) {
        double result = 440.0 * pow(2.0, (note-69) / 12.0);
        return result;
    }

    void noteEventCallback(uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger)
    {
        if (voice < _numVoices) {
            _voices[voice]->setPlaybackRate(calcFrequency(noteNumber));
            _voices[voice]->play(_data, _numSamples);
        }
    }
};

#endif