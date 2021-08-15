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

#ifndef TEENSY_AUDIO_SAMPLER_SAMPLEPLAYMIDICONTROLLER_H
#define TEENSY_AUDIO_SAMPLER_SAMPLEPLAYMIDICONTROLLER_H

#include <Arduino.h>
#include <vector>
#include <map>
#include <SD.h>
#include "sampler.h"

enum triggertype {
    triggertype_play_until_end = 0,
    triggertype_play_until_subsequent_notedown = 1,
    triggertype_play_while_notedown = 2
};

enum playlooptype {
    playlooptype_once = 0,
    playlooptype_looping = 1,
    playlooptype_pingpong = 2
};

enum playdirection {
    playdirection_begin_forward = 0,
    playdirection_begin_backward = 1
};

class sdsampleplayernote {
public:
    char * _filename = nullptr;
    byte _samplerNoteNumber = 0;    
    byte _samplerNoteChannel = 0;    
    triggertype _triggertype = triggertype::triggertype_play_until_end;
    playlooptype _playlooptype = playlooptype::playlooptype_once;
    playdirection _playdirection = playdirection::playdirection_begin_forward;
};

enum playcontrollerstate {
    playcontrollerstate_initialising = 0, // need to ascertain which midi notes and channels correspond to which control functions
    playcontrollerstate_performing = 1,
    playcontrollerstate_selecting_target = 2,
    playcontrollerstate_editing_target = 3,
};

enum triggerctrlfunction {
    triggerctrlfunction_none = 0,
    triggerctrlfunction_changetriggertype = 1,
    triggerctrlfunction_changedirection = 2,
    triggerctrlfunction_changelooptype = 3,
    triggerctrlfunction_changesample = 4,
};

class playcontrollerconfig {
public:
    const char *prompts[4] = {"Trigger type", "Direction", "Loop type", "sample"}; 
    byte midinotenum_changeTriggerType, midichannelnum_changeTriggerType,
         midinotenum_changeDirection,   midichannelnum_changeDirection,
         midinotenum_changeLoopType,    midichannelnum_changeLoopType,
         midinotenum_changeSample,      midichannelnum_changeSample;
    byte num_ctrl_fns_configured = 0;
    
    void prompt() {
        if (num_ctrl_fns_configured < sizeof(prompts)/sizeof(prompts[0]) ) {
            Serial.printf("%d: please select a key to change %s...\n", num_ctrl_fns_configured, prompts[num_ctrl_fns_configured]);
        } else 
            Serial.printf("%d: no more functions to configure...\n", num_ctrl_fns_configured);
    }

    void configure(byte noteNum, byte channel) {
        if (num_ctrl_fns_configured >= sizeof(prompts)/sizeof(prompts[0]) ) {
            Serial.printf("%d: no more functions to configure!!!\n", num_ctrl_fns_configured);
            return;
        }
        switch (num_ctrl_fns_configured) {
            case 0: {
                midinotenum_changeTriggerType = noteNum;
                midichannelnum_changeTriggerType = channel;
                num_ctrl_fns_configured++;
                break;
            }
            case 1: {
                if (!note_is_ctrl_function_key(noteNum, channel, 
                        triggerctrlfunction::triggerctrlfunction_changetriggertype)) {
                    midinotenum_changeDirection = noteNum;
                    midichannelnum_changeDirection = channel;
                    num_ctrl_fns_configured++;
                } else {
                    Serial.printf("%d: conflict: that note/channel is being used already\n", num_ctrl_fns_configured);
                }
                break;
            }
            case 2: {
                if (!note_is_ctrl_function_key(noteNum, channel, 
                        triggerctrlfunction::triggerctrlfunction_changetriggertype) 
                    && !note_is_ctrl_function_key(noteNum, channel, 
                        triggerctrlfunction::triggerctrlfunction_changedirection)) {
                    midinotenum_changeLoopType = noteNum;
                    midichannelnum_changeLoopType = channel;
                    num_ctrl_fns_configured++;
                } else {
                    Serial.printf("%d: conflict: that note/channel is being used already\n", num_ctrl_fns_configured);
                }
                break;     
            }                 
            case 3: {
                if (!note_is_ctrl_function_key(noteNum, channel, 
                        triggerctrlfunction::triggerctrlfunction_changetriggertype) 
                    && !note_is_ctrl_function_key(noteNum, channel, 
                        triggerctrlfunction::triggerctrlfunction_changedirection)
                    && !note_is_ctrl_function_key(noteNum, channel, 
                        triggerctrlfunction::triggerctrlfunction_changelooptype)) {
                    midinotenum_changeSample = noteNum;
                    midichannelnum_changeSample = channel;
                    num_ctrl_fns_configured++;
                } else {
                    Serial.printf("%d: conflict: that note/channel is being used already\n", num_ctrl_fns_configured);
                }
                break;     
            }          
        }
        prompt();
    }

    bool complete() {
        return num_ctrl_fns_configured >= 4;
    }
    
    bool iscontrolkey(byte noteNum, byte channel) {
        return 
            (getfunctionkey(noteNum, channel) == triggerctrlfunction_none);
    }

    triggerctrlfunction getfunctionkey(byte noteNum, byte channel) {
        if (note_is_ctrl_function_key(noteNum, channel,  triggerctrlfunction_changedirection))
            return triggerctrlfunction_changedirection;

        if (note_is_ctrl_function_key(noteNum, channel, triggerctrlfunction_changelooptype ))
            return triggerctrlfunction_changelooptype;

        if (note_is_ctrl_function_key(noteNum, channel, triggerctrlfunction_changesample ))
            return triggerctrlfunction_changesample;

        if (note_is_ctrl_function_key(noteNum, channel, triggerctrlfunction_changetriggertype ))
            return triggerctrlfunction_changetriggertype;

        return triggerctrlfunction_none;
    }

private:
    bool note_is_ctrl_function_key(byte note, byte channel, triggerctrlfunction funct) {
        switch (funct) {
            case triggerctrlfunction::triggerctrlfunction_changetriggertype:
                return (note == midinotenum_changeTriggerType && channel == midichannelnum_changeTriggerType);
            case triggerctrlfunction::triggerctrlfunction_changedirection:
                return (note == midinotenum_changeDirection && channel == midichannelnum_changeDirection);
            case triggerctrlfunction::triggerctrlfunction_changelooptype:
                return (note == midinotenum_changeLoopType && channel == midichannelnum_changeLoopType);
            case triggerctrlfunction::triggerctrlfunction_changesample:
                return (note == midinotenum_changeSample && channel == midichannelnum_changeSample);
            default:{
                Serial.printf("Not sure about ctrl function #%d\n", (int)funct);
                return false;
            }
        }
    }
};

class sdsampleplayermidicontroller {
public:
    playcontrollerconfig _config;
    playcontrollerstate _state = playcontrollerstate::playcontrollerstate_initialising;
    sdsampleplayernote *_selected_target = nullptr;
    triggerctrlfunction _selected_ctrl_function = triggerctrlfunction::triggerctrlfunction_changetriggertype;

    std::vector<sdsampleplayernote*> _samples;
    
    // needed the first time we use the firmware, to prompt user to select first function key configuration 
    void initialisation_prompt() {
        if (_state == playcontrollerstate::playcontrollerstate_initialising) {
            _config.prompt();
        }
    }

    std::vector<char *> _filenames;

    void begin(const char* directoryname = nullptr) {
        populateFilenames(directoryname);
               
        
    }

    void unloadFilenames() {
        for (auto && filename : _filenames) {
            delete [] filename;
        }
        _filenames.clear();
    }
    
    void unloadSamples() {
        for (auto && sample : _samples) {
            if (sample->_filename)
                delete sample->_filename;
            delete sample;
        }
        _samples.clear();
    }
    
    void populateFilenames(const char *directory) {
        unloadFilenames();
        File dir = directory? SD.open(directory) : SD.open(".");
        while (true) { 

            File files =  dir.openNextFile();
            if (!files) {
                //If no more files, break out.
                break;
            }

            String curfile = files.name(); //put file in string
            int m = curfile.lastIndexOf(".WAV");
            int a = curfile.lastIndexOf(".wav");
            int underscore = curfile.indexOf("_");

            // if returned results is more then 0 add them to the list.
            if ((m > 0 || a > 0) && (underscore != 0)) {  
                char *filename = new char[curfile.length()+1] {0};
                memcpy(filename, curfile.c_str(), curfile.length());
                _filenames.push_back(filename);
            } 
            files.close();
        }
        // close 
        dir.close();
    }

    void midiChannleVoiceMessage(byte status, byte data1, byte data2, byte channel) {
        switch (_state) {

            case playcontrollerstate_initialising : {
                // we are learning the ctrl function keys 
                if ((status & 0xF0) == 0x90) // midi key-down status nibble
                    _config.configure(data1, channel);
                    if (_config.complete()) {
                        _state = playcontrollerstate::playcontrollerstate_performing;
                    }
                break;
            }
            
            case playcontrollerstate_performing: {
                triggerctrlfunction potentialFnKey = _config.getfunctionkey(status, channel);
                switch (potentialFnKey) {
                    case triggerctrlfunction_none: {
                        // not a function key - regular performance note...
                        // try find a sample mapping for this fellow

                        break;
                    }
                    case triggerctrlfunction_changesample: {
                        _selected_ctrl_function = triggerctrlfunction_changesample;
                        _state = playcontrollerstate::playcontrollerstate_selecting_target;
                        break;
                    }
                    case triggerctrlfunction_changedirection: {
                        _selected_ctrl_function = triggerctrlfunction_changedirection;
                        _state = playcontrollerstate::playcontrollerstate_selecting_target;
                        break;
                    }
                    case triggerctrlfunction_changelooptype: {
                        _selected_ctrl_function = triggerctrlfunction_changelooptype;
                        _state = playcontrollerstate::playcontrollerstate_selecting_target;
                        break;
                    }
                    case triggerctrlfunction_changetriggertype: {
                        _selected_ctrl_function = triggerctrlfunction_changetriggertype;
                        _state = playcontrollerstate::playcontrollerstate_selecting_target;
                        break;
                    }                                        
                }
                break;
            }
            case playcontrollerstate_selecting_target: {
                if ((status & 0xf0) == 0x90) {
                    // midi key-down status
                    _selected_target = getSamplerNoteForNoteNum(data1, channel);
                    if (_selected_target == nullptr) {
                        _selected_target = new sdsampleplayernote();
                        _selected_target->_samplerNoteNumber = data1;
                        _selected_target->_samplerNoteChannel = channel;                        
                        _samples.push_back(_selected_target);
                    }
                    _state = playcontrollerstate::playcontrollerstate_selecting_target;
                }
                break;
            }
            case playcontrollerstate_editing_target: {

                if (_config.getfunctionkey(data1, channel) == _selected_ctrl_function) {
                    // the note pressed was the select ctrl function already selecected, go back to performance mode
                    _state = playcontrollerstate_performing;
                    _selected_ctrl_function = triggerctrlfunction_none;
                    break;
                }
                if (_selected_target != nullptr) {  
                    switch (_selected_ctrl_function) {
                        case triggerctrlfunction_none: {
                            // shouldn't ever reach here...
                            break;
                        }

                        case triggerctrlfunction_changetriggertype: {
                            triggertype triggerType = (triggertype)(data1 * 5 / 127); 
                            if (triggerType != _selected_target->_triggertype){
                                _selected_target->_triggertype = triggerType;
                                Serial.printf("sample %d, %d changed trigger type to %s\n", data1, channel, getTriggerTypeName(triggerType));
                            }
                            break;
                        }

                        case triggerctrlfunction_changesample: {


                            break;
                        }

                        case triggerctrlfunction_changedirection:{
                            playdirection playDirection = (playdirection)(data1 * 2 / 127); 
                            if (playDirection != _selected_target->_playdirection){
                                _selected_target->_playdirection = playDirection;
                                Serial.printf("sample %d, %d changed play dir to %s\n", data1, channel, getPlayDirectionName(playDirection));
                            }
                            break;
                        }

                        case triggerctrlfunction_changelooptype: {
                            playlooptype playLoopType = (playlooptype)(data1 * 3 / 127); 
                            if (playLoopType != _selected_target->_playlooptype){
                                _selected_target->_playlooptype = playLoopType;
                                Serial.printf("sample %d, %d changed loop type to %s\n", data1, channel, getLoopTypeName(playLoopType));
                            }
                            break;
                        }
                    }
                }
                break;
            }


            default:
                break;
        }
    }

private:
    sdsampleplayernote* getSamplerNoteForNoteNum(byte noteNum, byte channel) {
        for (auto && note : _samples) {
            if (note->_samplerNoteNumber == noteNum && note->_samplerNoteChannel == channel)
                return note;
        }
        return nullptr;
    }
    static const char* getTriggerTypeName(triggertype tt){
        switch (tt) {
            case triggertype_play_until_end: 
                return "play until end";
            case triggertype_play_until_subsequent_notedown:
                return "toggle";
            case triggertype_play_while_notedown: 
                return "play while notedown";
            default:
                return "not sure?";
        }
    }

    
    static const char* getPlayDirectionName(playdirection pd){
        switch (pd) {
            case playdirection_begin_forward: 
                return "begin forwards";
            case playdirection_begin_backward:
                return "begin backwards";
            default:
                return "not sure?";
        }
    }

};

#endif // TEENSY_AUDIO_SAMPLER_SAMPLEPLAYMIDICONTROLLER_H