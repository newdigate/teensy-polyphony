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

enum activenotestate {
    noteDown = 0,
    noteUpReceived = 1,
    noteComplete = 2
};

template <typename TVoice, typename TSample>
class activenote {
    public:
        activenote(TVoice *voice, TSample *sample) : _voice(voice), _sample(sample) {}
        TVoice *_voice;
        TSample *_sample;
        bool _noteOffRecieved = false;
};

template <typename TVoice, typename TSample>
class polyphonicsampler {
public:
    typedef std::function<bool(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t noteVelocity, bool retrigger)> NoteDownEventCallback;
    typedef std::function<bool(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel)> VoicePredicateCallback;
    typedef std::function<void(TVoice *voice, TSample *sample, uint8_t noteNumber, uint8_t noteChannel)> NoteUpEventCallback;
    typedef std::map<uint8_t, std::map<uint8_t, std::vector<activenote<TVoice, TSample>*>*>*> ActiveNotesChannelKeyMap;
    typedef std::function<TSample*(uint8_t noteNumber, uint8_t noteChannel)> FindSampleCallback;
    typedef std::function<triggertype(uint8_t noteNumber, uint8_t noteChannel)> FindTriggerTypeCallback;

    polyphonicsampler(
            polyphonic<TVoice>      &polyphony, 
            NoteDownEventCallback   noteDownEventFunction,
            NoteUpEventCallback     noteUpBeginEventFunction,
            NoteUpEventCallback     noteUpEndEventFunction,
            FindSampleCallback      findSampleFunction,
            FindTriggerTypeCallback findTriggerTypeFunction,
            VoicePredicateCallback  isVoiceStillActiveFunction,
            VoicePredicateCallback  isVoiceStillNoteDownFunction) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteDownEventFunction(noteDownEventFunction),
        _noteUpBeginEventFunction(noteUpBeginEventFunction),
        _noteUpEndEventFunction(noteUpEndEventFunction),
        _isVoiceStillActiveFunction(isVoiceStillActiveFunction),
        _isVoiceStillNoteDownFunction(isVoiceStillNoteDownFunction),
        _findSampleFunction(findSampleFunction),
        _findTriggerTypeFunction(findTriggerTypeFunction),
        _useStaticTriggerType(false),
        _staticTriggerType(triggertype::triggertype_play_while_notedown)
    {
    }

    polyphonicsampler(
            polyphonic<TVoice>      &polyphony, 
            NoteDownEventCallback   noteDownEventFunction,
            NoteUpEventCallback     noteUpBeginEventFunction,
            NoteUpEventCallback     noteUpEndEventFunction,
            FindSampleCallback      findSampleFunction,
            triggertype             trigger,
            VoicePredicateCallback  isVoiceStillActiveFunction,
            VoicePredicateCallback  isVoiceStillNoteDownFunction) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteDownEventFunction(noteDownEventFunction),
        _noteUpBeginEventFunction(noteUpBeginEventFunction),
        _noteUpEndEventFunction(noteUpEndEventFunction),
        _isVoiceStillActiveFunction(isVoiceStillActiveFunction),
        _isVoiceStillNoteDownFunction(isVoiceStillNoteDownFunction),
        _findSampleFunction(findSampleFunction),
        _findTriggerTypeFunction([] (uint8_t noteNumber, uint8_t noteChannel) -> triggertype { return triggertype::triggertype_play_while_notedown; }),
        _useStaticTriggerType(true),
        _staticTriggerType(trigger)
    {
    }

    polyphonicsampler(
            polyphonic<TVoice>      &polyphony, 
            NoteDownEventCallback   noteDownEventFunction,
            NoteUpEventCallback     noteUpBeginEventFunction,
            NoteUpEventCallback     noteUpEndEventFunction,
            FindSampleCallback      findSampleFunction,
            VoicePredicateCallback  isVoiceStillActiveFunction,
            VoicePredicateCallback  isVoiceStillNoteDownFunction) : 
        _polyphony(polyphony),
        _activeNotesPerChannel(),
        _noteDownEventFunction(noteDownEventFunction),
        _noteUpBeginEventFunction(noteUpBeginEventFunction),
        _noteUpEndEventFunction(noteUpEndEventFunction),
        _isVoiceStillActiveFunction(isVoiceStillActiveFunction),
        _isVoiceStillNoteDownFunction(isVoiceStillNoteDownFunction),
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
                TVoice* voice = _polyphony.useVoice();
                
                if (voice != nullptr) {    
                    TSample *sample = _findSampleFunction(noteNumber, noteChannel);
                    activenote<TVoice, TSample> *activeNote = new activenote<TVoice, TSample>(voice, sample);                 
                    if (noteOn(noteNumber, velocity, noteChannel, voice, sample))
                        addActiveNote(activeNote, noteNumber, noteChannel);
                    else delete activeNote;
                } 
            }           
            break;

            case triggertype_play_while_notedown : {
                TVoice *voice = nullptr;
                TSample *sample = nullptr;
                bool isretrigger = false;
                activenote<TVoice, TSample> *activeNote = isNoteActive(noteNumber, noteChannel);

                if (activeNote == nullptr) {
                    // note is not active, allocate a voice if possible
                    voice = _polyphony.useVoice();
                    if (voice != nullptr) {
                        sample = _findSampleFunction(noteNumber, noteChannel);
                        activenote<TVoice, TSample> *activeNote = new activenote<TVoice, TSample>(voice, sample);
                        addActiveNote(activeNote, noteNumber, noteChannel);
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
                activenote<TVoice, TSample>* activeNote = isNoteActive(noteNumber, noteChannel);
                if (activeNote) {
                    noteOff(noteNumber, noteChannel, activeNote);  
                } else {
                    TVoice *voice = _polyphony.useVoice();
                    if (voice != nullptr) {
                        TSample *sample = _findSampleFunction(noteNumber, noteChannel);
                        activeNote = new activenote<TVoice, TSample>(voice, sample);
                        addActiveNote(activeNote, noteNumber, noteChannel);
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
                activenote<TVoice, TSample>* activenote = isNoteActive(noteNumber, noteChannel);
                noteOff(noteNumber, noteChannel, activenote);
                break;
            }

            default: break;
        }
    }

    bool noteOn(uint8_t noteNumber, uint8_t velocity, uint8_t noteChannel, TVoice* voice, TSample *sample, bool isretrigger = false) {
        if (voice != nullptr) {            
            return _noteDownEventFunction(voice, sample, noteNumber, noteChannel, velocity, isretrigger);
        }
        return false;
    }

    void noteOff(uint8_t noteNumber, uint8_t noteChannel) {
        activenote<TVoice, TSample>* activeNote = isNoteActive(noteNumber, noteChannel);
        if (activeNote == nullptr)
            return;

        noteOff(noteNumber, noteChannel, activeNote);
    }

    void noteOff(uint8_t noteNumber, uint8_t noteChannel, activenote<TVoice, TSample>* activeNote) {
        if (activeNote == nullptr)
            return;

        _noteUpBeginEventFunction(activeNote->_voice, activeNote->_sample, noteNumber, noteChannel);
    }

    void turnOffAllNotesStillPlaying() {
        for (auto && activeNotesForChannel : _activeNotesPerChannel){
            for (auto && activeNote : (*(activeNotesForChannel.second))) {
                noteOff(activeNote.first, activeNotesForChannel.first);
            }
        }
    }

    virtual void update() {
        for (auto && activenotesForChannel : _activeNotesPerChannel){
            for (auto && activenotesForChannelAndKey : *(activenotesForChannel.second)) {
                for (auto && activeNote : *(activenotesForChannelAndKey.second)) {
                    if (!activeNote->_noteOffRecieved) {
                        if (!_isVoiceStillNoteDownFunction(activeNote->_voice, activeNote->_sample, activenotesForChannelAndKey.first, activenotesForChannel.first)){
                            _noteUpBeginEventFunction(activeNote->_voice, activeNote->_sample, activenotesForChannel.first, activenotesForChannelAndKey.first);
                            activeNote->_noteOffRecieved = true;
                        }
                    } else
                    {                        
                        if (!_isVoiceStillActiveFunction(activeNote->_voice, activeNote->_sample, activenotesForChannelAndKey.first, activenotesForChannel.first)){
                            _noteUpEndEventFunction(activeNote->_voice, activeNote->_sample, activenotesForChannel.first, activenotesForChannelAndKey.first);
                            activenotesForChannelAndKey.second->erase(std::remove(activenotesForChannelAndKey.second->begin(), activenotesForChannelAndKey.second->end(), activeNote), activenotesForChannelAndKey.second->end());
                            delete activeNote;
                        }
                    }
                }
            }
        }
    }

protected:
    NoteDownEventCallback       _noteDownEventFunction;
    NoteUpEventCallback         _noteUpBeginEventFunction;
    NoteUpEventCallback         _noteUpEndEventFunction;
    VoicePredicateCallback      _isVoiceStillActiveFunction;
    VoicePredicateCallback      _isVoiceStillNoteDownFunction;
    FindSampleCallback          _findSampleFunction;
    polyphonic<TVoice>          &_polyphony;
    ActiveNotesChannelKeyMap     _activeNotesPerChannel;
    FindTriggerTypeCallback     _findTriggerTypeFunction;
    bool                        _useStaticTriggerType;
    triggertype                 _staticTriggerType = triggertype_play_while_notedown;

    activenote<TVoice, TSample>* isNoteActive(uint8_t noteNumber, uint8_t noteChannel) {
        std::map<uint8_t, std::vector<activenote<TVoice, TSample>*>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel != nullptr) {
            std::vector<activenote<TVoice, TSample>*> *activeNotes = (*activeNotesForChannel)[noteNumber];
            if (activeNotes->size() > 0)
                return (*activeNotes)[0];
        }
        return nullptr;
    }
    
    void addActiveNote(activenote<TVoice, TSample> *activeNote, uint8_t noteNumber, uint8_t noteChannel ) {
        if (activeNote == nullptr)
            return;

        std::map<uint8_t, std::vector<activenote<TVoice, TSample>*>*> *activeNotesForChannel = _activeNotesPerChannel[noteChannel];
        if (activeNotesForChannel == nullptr) {
            activeNotesForChannel = new std::map<uint8_t, std::vector<activenote<TVoice, TSample>*>*>();
            _activeNotesPerChannel[noteChannel] = activeNotesForChannel;
        }

        std::vector<activenote<TVoice, TSample>*> *activeNotesForChannelAndKey = (*activeNotesForChannel)[noteNumber];
        if (activeNotesForChannelAndKey == nullptr)  {
            activeNotesForChannelAndKey = new std::vector<activenote<TVoice, TSample>*>();
            (*activeNotesForChannel)[noteNumber] = activeNotesForChannelAndKey;
        }
        activeNotesForChannelAndKey->push_back(activeNote);
    }
};


#endif //TEENSYSEQUENCER_POLYPHONICSAMPLER_H
