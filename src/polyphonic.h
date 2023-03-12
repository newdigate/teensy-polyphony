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

#ifndef TEENSYAUDIO_LAUNCHCTRL_POLYPHONIC_H
#define TEENSYAUDIO_LAUNCHCTRL_POLYPHONIC_H

#include <vector>
#include <stdint.h>
#include <Arduino.h>

template <typename TVoice>
class voice_usage {
public:
    voice_usage(TVoice &voice) : _voice(voice), _isActive(false) {
    }
    virtual ~voice_usage() {
    }
    TVoice &_voice;
    bool _isActive;
};

template <typename TVoice>
class polyphonic {
public:
    polyphonic() : _voices(0) {
    }

    virtual ~polyphonic() {
        for (auto v : _voices) {
            delete v;
        }
        _voices.clear();
    }

    TVoice* useVoice() {
        //Serial.printf("use voice %d\n", _voicesInUse);
        for (auto v : _voices) {
            if (!v->_isActive) {
                v->_isActive = true;
                _voicesInUse++;
                return &v->_voice;
            }
        } 

        for (auto v : _voices) {
            if ( v->_isActive) {
                if( ! (&v->_voice)->isPlaying() ) {
                    return &v->_voice;
                }
            } 
        }

        return nullptr;
    }

    void freeVoice(TVoice* voice) {
        Serial.printf("free voice %d\n", _voicesInUse);

        for (auto v : _voices) {
            if ( v->_isActive) {
                if( ! (&v->_voice)->isPlaying() ) {
                    v->_isActive = false;
                    _voicesInUse--;
                }
            } 
        }
        for (auto v : _voices) {
            if (&v->_voice == voice && v->_isActive) {
                v->_isActive = false;
                _voicesInUse--;
                return;
            }
        } 
        

    }

    void addVoice(TVoice &voice) {   
        voice_usage<TVoice> *vu = new voice_usage<TVoice>(voice);
        _voices.push_back(vu);
    }

    void addVoices(TVoice **voices, uint8_t numOfVoicesToAdd){
        for (int i = 0; i < numOfVoicesToAdd; i++){
            addVoice(*voices[i]);
        }
    }
    void addVoices(TVoice *voices, uint8_t numOfVoicesToAdd){
        for (int i = 0; i < numOfVoicesToAdd; i++){
            addVoice(voices[i]);
        }
    }

private:
    std::vector<voice_usage<TVoice> *> _voices;
    volatile int _voicesInUse = 0;
};

#endif //TEENSYAUDIO_LAUNCHCTRL_POLYPHONIC_H
