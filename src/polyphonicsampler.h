#ifndef TEENSYPOLYPHONY_POLYPHONICSAMPLER_H
#define TEENSYPOLYPHONY_POLYPHONICSAMPLER_H

#include <cstdint>
#include <Arduino.h>
#include <functional>
#define MAX_VOICES 16

//typedef void (*noteEventCallback)();

class polyphonicsampler {
public:
    polyphonicsampler() : _numVoices(0) {
        for (int i=0; i < 128; i++)
          activeNotes[i] = 255;
    }

    void setNumVoices(uint8_t numVoices) {
        if (numVoices > _numVoices)
            for (int i=_numVoices; i < numVoices; i++)
              activeVoices[i] = 255;
        else if (numVoices < _numVoices) {
            for (int i=_numVoices; i > numVoices; i--)
              activeVoices[i] = 255;
        }
        _numVoices = numVoices;
    }

    void noteOn(uint8_t noteNumber, uint8_t velocity) {
        int indexOfVoice = 255;
        bool isretrigger = false;
        if (activeNotes[noteNumber] == 255) {
            // note is not active, allocate a voice if possible
            indexOfVoice = getFirstFreeVoice();
            if (indexOfVoice < _numVoices) {
                activeVoices[indexOfVoice] = noteNumber;
                activeNotes[noteNumber] = indexOfVoice;                
            } else
            {
                // note dropped: insufficient polyphony to play this note
                indexOfVoice = 255;
                Serial.printf("Note dropped: %i \n", noteNumber);
            }
        } else {
            // note is already active, just re-trigger it...
            indexOfVoice = activeNotes[noteNumber];
            isretrigger = true;
        }
        if (indexOfVoice != 255) {            
            _noteEventFunction(indexOfVoice, noteNumber, velocity, true, isretrigger);
            Serial.printf("Voice %i plays note %i (%i, %i)\n", indexOfVoice, noteNumber, velocity, isretrigger);
        }
    }

    void noteOff(uint8_t noteNumber) {
        uint8_t index = activeNotes[noteNumber];
        if (index == 255) {
            // note is not active, ignore
            return;
        }

        _noteEventFunction(index, noteNumber, 0, false, false);
        activeNotes[noteNumber] = 255;
        activeVoices[index] = 255; // free the voice
    }

    void setNoteEventCallback (std::function<void(uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger)> noteEventFunction) {
        _noteEventFunction =  noteEventFunction;
    }

    void turnOffAllNotesStillPlaying() {
        Serial.printf("turn off all notes: (%d)\n", _numVoices);
        for (int i=0; i<_numVoices; i++) {
            if (activeVoices[i] != 255) {
                Serial.printf("turn off %d\n",activeVoices[i]);
                noteOff(activeVoices[i]);
            }
        }
    }

private:
    std::function<void(uint8_t voice, uint8_t noteNumber, uint8_t velocity, bool isNoteOn, bool retrigger)>  _noteEventFunction;

    uint8_t activeNotes[128];
    uint8_t activeVoices[MAX_VOICES];
    uint8_t _numVoices;

    uint8_t getFirstFreeVoice() {
        for (int i=0; i < _numVoices; i++) {
            if (activeVoices[i] == 255) {
                return i;
            }
        }
        return 255;
    }
};


#endif //TEENSYSEQUENCER_POLYPHONICSAMPLER_H
