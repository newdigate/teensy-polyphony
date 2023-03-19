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
#include "loopsamplerenums.h"

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
    typedef std::function<triggertype(uint8_t noteNumber, uint8_t noteChannel)> FindTriggerTypeCallback;

    polyphonicsampler(
            polyphonic<TVoice> &polyphony, 
            NoteEventCallback noteEventFunction,
            FindSampleCallback findSampleFunction,
            FindTriggerTypeCallback findTriggerTypeFunction) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteEventFunction(noteEventFunction),
        _findSampleFunction(findSampleFunction),
        _findTriggerTypeFunction(findTriggerTypeFunction),
        _useStaticTriggerType(false),
        _staticTriggerType(triggertype::triggertype_play_while_notedown)
    {
    }

    polyphonicsampler(
            polyphonic<TVoice> &polyphony, 
            NoteEventCallback noteEventFunction,
            FindSampleCallback findSampleFunction,
            triggertype trigger) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteEventFunction(noteEventFunction),
        _findSampleFunction(findSampleFunction),
        _findTriggerTypeFunction([] (uint8_t noteNumber, uint8_t noteChannel) -> triggertype { return triggertype::triggertype_play_while_notedown; }),
        _useStaticTriggerType(true),
        _staticTriggerType(trigger)
    {
    }

    polyphonicsampler(
            polyphonic<TVoice> &polyphony, 
            NoteEventCallback noteEventFunction,
            FindSampleCallback findSampleFunction) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteEventFunction(noteEventFunction),
        _findSampleFunction(nullptr),
        _useStaticTriggerType(true),
        _staticTriggerType(triggertype::triggertype_play_while_notedown)
    {
    }

    polyphonicsampler(const polyphonicsampler&) = delete;

    void preprocessNoteOn(uint8_t noteNumber, uint8_t velocity, uint8_t noteChannel) {
        triggertype trigger = _staticTriggerType;
        if (!_useStaticTriggerType) {
            trigger = _findTriggerTypeFunction(noteNumber, noteChannel);
        } 
        noteDown(noteNumber, velocity, noteChannel, trigger);
    }

    void preprocessNoteOff(uint8_t noteNumber, uint8_t noteChannel) {
        triggertype trigger = _staticTriggerType;
        if (!_useStaticTriggerType) {
            trigger = _findTriggerTypeFunction(noteNumber, noteChannel);
        }
        noteUp(noteNumber, noteChannel, trigger);
    }

    void noteDown(uint8_t noteNumber, uint8_t velocity, uint8_t noteChannel, triggertype trigger) {
        switch (trigger) {
            case triggertype_play_until_end : {
                TVoice* voice = getVoice(noteNumber, noteChannel);
                if (voice != nullptr) {    
                    TSample *sample = _findSampleFunction(noteNumber, noteChannel);
                    activenote<TVoice, TSample> *activeNote = allocateActiveNote(voice, sample, noteNumber, noteChannel);
                    noteOn(noteNumber, velocity, noteChannel, voice, sample);     
                } 
            }           
            break;

            case triggertype_play_while_notedown : {
                TVoice *voice = nullptr;
                TSample *sample = nullptr;
                bool isretrigger = false;
                activenote<TVoice, TSample> *activeNote = nullptr;

                std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
                if (activeNotesForChannel != nullptr) {
                    activeNote = (*activeNotesForChannel)[noteNumber];
                }

                if (activeNote == nullptr) {
                    // note is not active, allocate a voice if possible
                    voice = getVoice(noteNumber, noteChannel);
                    if (voice != nullptr) {
                        TSample *sample = _findSampleFunction(noteNumber, noteChannel);
                        activenote<TVoice, TSample> *activeNote = allocateActiveNote(voice, sample, noteNumber, noteChannel);
                    }
                } else {
                    // note is already active, just re-trigger it...
                    voice = activeNote->_voice;
                    sample = activeNote->_sample;
                    isretrigger = true;
                }

                noteOn(noteNumber, velocity, noteChannel, voice, sample, isretrigger); 
            }
            break;

            case triggertype_play_until_subsequent_notedown: {
                activenote<TVoice, TSample>* activenote = isNoteActive(noteNumber, noteChannel);
                if (activenote) {
                    noteOff(noteNumber, noteChannel, activenote);  
                } else {
                    TVoice *voice = getVoice(noteNumber, noteChannel);
                    if (voice != nullptr) {
                        TSample *sample = _findSampleFunction(noteNumber, noteChannel);
                        activenote = allocateActiveNote(voice, sample, noteNumber, noteChannel);
                        noteOn(noteNumber, velocity, noteChannel, voice, sample);
                    }
                }
            }
            break;

            default: break;
        }
    }

    void noteUp(uint8_t noteNumber, uint8_t noteChannel, triggertype trigger) {
        switch (trigger) {
            case triggertype_play_until_end :
            case triggertype_play_until_subsequent_notedown :
            break;
            
            case triggertype_play_while_notedown :
            {   
                 
                break;
            }

            default: break;
        }
    }

    void noteOn(uint8_t noteNumber, uint8_t velocity, uint8_t noteChannel, TVoice* voice, TSample *sample, bool isretrigger = false) {
        if (voice != nullptr) {            
            _noteEventFunction(voice, sample, noteNumber, noteChannel, velocity, true, isretrigger);
        }
    }

    void noteOff(uint8_t noteNumber, uint8_t noteChannel) {
        std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel == nullptr)
            return;
        
        activenote<TVoice, TSample>* activeNote = (*activeNotesForChannel)[noteNumber];
        if (activeNote == nullptr)
            return;

        noteOff(noteNumber, noteChannel, activeNote);
    }

    void noteOff(uint8_t noteNumber, uint8_t noteChannel, activenote<TVoice, TSample>* activeNote) {
        if (activeNote == nullptr)
            return;

        _noteEventFunction(activeNote->_voice, activeNote->_sample, noteNumber, noteChannel, 0, false, false);
        std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel != nullptr)
            activeNotesForChannel->erase(noteNumber);
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
    polyphonic<TVoice>      &_polyphony;
    
    std::map<uint8_t, std::map<uint8_t, activenote<TVoice, TSample>*>*> _activeNotesPerChannel;

    FindTriggerTypeCallback _findTriggerTypeFunction;
    bool _useStaticTriggerType;
    triggertype _staticTriggerType = triggertype_play_while_notedown;

    activenote<TVoice, TSample>* isNoteActive(uint8_t noteNumber, uint8_t noteChannel) {
        activenote<TVoice, TSample> *activeNote = nullptr;

        std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel != nullptr) {
            activeNote = (*activeNotesForChannel)[noteNumber];
        }
        return activeNote;
    }
    
    TVoice* getVoice(uint8_t noteNumber, uint8_t noteChannel ) {
        TVoice* voice = _polyphony.useVoice(); // getFirstFreeVoice();
        if (voice != nullptr) {
            std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
            if (activeNotesForChannel == nullptr) {
                activeNotesForChannel = new std::map<uint8_t, activenote<TVoice, TSample>*>();
                _activeNotesPerChannel[noteChannel] = activeNotesForChannel;
            }

        } else
        {
            // note dropped: insufficient polyphony to play this note
            //indexOfVoice = 255;
            Serial.printf("Note dropped: %i \n", noteNumber);
        }
        return voice;
    }

    activenote<TVoice, TSample>* allocateActiveNote(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel) {
        std::map<uint8_t, activenote<TVoice, TSample>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        activenote<TVoice, TSample> *activeNote = new activenote<TVoice, TSample>(voice, sample);
        (*activeNotesForChannel)[noteNumber] = activeNote;
        return activeNote;
    }
};


#endif //TEENSYSEQUENCER_POLYPHONICSAMPLER_H
