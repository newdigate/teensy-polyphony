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

#ifndef TEENSYPOLYPHONY_POLYPHONICSAMPLER_H
#define TEENSYPOLYPHONY_POLYPHONICSAMPLER_H

#include <cstdint>
#include <Arduino.h>
#include <functional>
#include <map>
#include "polyphonic.h"

#define MAX_VOICES 16

template <typename TVoice, typename TSample>
class activenote {
    public:
        activenote(TVoice *voice, TSample *sample) : _voice(voice), _sample(sample) {}
        TVoice *_voice;
        TSample *_sample;
};

template <typename TVoice, typename TSample>
class polyphonicsampler {
public:
    typedef std::function<void(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t noteVelocity, bool isNoteOn, bool retrigger)> NoteEventCallback;
    typedef std::function<TSample*(uint8_t noteNumber, uint8_t noteChannel)> FindSampleCallback;

    polyphonicsampler(
            polyphonic<TVoice> &polyphony, 
            NoteEventCallback noteEventFunction,
            FindSampleCallback findSampleFunction) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteEventFunction(noteEventFunction),
        _findSampleFunction(findSampleFunction) 
    {
    }

    void noteOn(uint8_t noteNumber, uint8_t velocity, uint8_t noteChannel) {
        TVoice *voice = nullptr;
        bool isretrigger = false;
        activenote<TVoice, TSample> *activeNote = nullptr;

        std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel != nullptr) {
            activeNote = (*activeNotesForChannel)[noteNumber];
        }

        if (activeNote == nullptr) {
            // note is not active, allocate a voice if possible
            voice = _polyphony.useVoice(); // getFirstFreeVoice();
            if (voice != nullptr) {
                if (activeNotesForChannel == nullptr) {
                    activeNotesForChannel = new std::map<uint8_t, activenote<TVoice, TSample>*>();
                    _activeNotesPerChannel[noteChannel] = activeNotesForChannel;
                }
                TSample *newSample = _findSampleFunction(noteNumber, noteChannel);
                activeNote = new activenote<TVoice, TSample>(voice, newSample);
                (*activeNotesForChannel)[noteNumber] = activeNote;
            } else
            {
                // note dropped: insufficient polyphony to play this note
                //indexOfVoice = 255;
                Serial.printf("Note dropped: %i \n", noteNumber);
            }
        } else {
            // note is already active, just re-trigger it...
            voice = activeNote->_voice;
            isretrigger = true;
        }
        if (voice != nullptr) {            
            _noteEventFunction(voice, activeNote->_sample, noteNumber, noteChannel, velocity, true, isretrigger);
        }
    }

    void noteOff(uint8_t noteNumber, uint8_t noteChannel) {
        std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel == nullptr)
            return;
        
        activenote<TVoice, TSample>* activeNote = (*activeNotesForChannel)[noteNumber];
        if (activeNote == nullptr)
            return;

        _noteEventFunction(activeNote->_voice, activeNote->_sample, noteNumber, noteChannel, 0, false, false);
        (*activeNotesForChannel)[noteNumber] = nullptr;
        delete activeNote;
    }

    void turnOffAllNotesStillPlaying() {
        for (auto && activeNotesForChannel : _activeNotesPerChannel){
            for (auto && activeNote : (*(activeNotesForChannel.second))) {
                noteOff(activeNote.first, activeNotesForChannel.first);
            }
        }
    }

protected:
    NoteEventCallback   _noteEventFunction;
    FindSampleCallback  _findSampleFunction;

    polyphonic<TVoice> &_polyphony;
    
    std::map<uint8_t, std::map<uint8_t, activenote<TVoice, TSample>*>*> _activeNotesPerChannel;

    //uint8_t _numVoices;
    //unsigned long voice_noteOff[MAX_VOICES] {0};
    //unsigned long voice_noteOn[MAX_VOICES] {0};
    /**
    uint8_t getFirstFreeVoice() {
        unsigned long leastRecentNoteOffEvent = UINT32_MAX;
        uint8_t indexOfVoiceWithLeastRecentNoteOff = 255;
        for (int i=0; i < _numVoices; i++) {
            if (activeVoices[i] == 255) {
                if (voice_noteOff[i] < leastRecentNoteOffEvent) {
                    leastRecentNoteOffEvent = voice_noteOff[i];
                    indexOfVoiceWithLeastRecentNoteOff = i;
                }
            }
        }
        if (indexOfVoiceWithLeastRecentNoteOff == 0xff) {
            // all voices are in use... return the voice with least recent note off
            unsigned long leastRecentNoteOnEvent = UINT32_MAX;
            for (int i=0; i < _numVoices; i++) {
                if (voice_noteOn[i] < leastRecentNoteOffEvent) {
                    leastRecentNoteOffEvent = voice_noteOn[i];
                    indexOfVoiceWithLeastRecentNoteOff = i;
                }
            }
        }
        return indexOfVoiceWithLeastRecentNoteOff;
    }
    */
};


#endif //TEENSYSEQUENCER_POLYPHONICSAMPLER_H
