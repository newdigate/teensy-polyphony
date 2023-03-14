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

#ifndef TEENSY_AUDIO_LOOPSAMPLER_H
#define TEENSY_AUDIO_LOOPSAMPLER_H

#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include "polyphonicsampler.h"
#include <vector>
#include "effect_envelope.h"
#include "mixer.h"
#include "sampler.h"
#include "loopsamplerenums.h"

class sdloopaudiosample {
public:
    sdloopaudiosample(uint8_t noteNumber, const char *filename, uint16_t numChannels) : 
        _noteNumber(noteNumber), 
        _filename(filename) {
    }

    sdloopaudiosample(uint8_t noteNumber, const char *filename) : 
        _noteNumber(noteNumber), 
        _filename(filename) {
    }

    uint8_t _noteNumber;    
    const char *_filename;

    triggertype _triggertype = triggertype::triggertype_play_until_end;
    playlooptype _playlooptype = playlooptype::playlooptype_once;
    playdirection _playdirection = playdirection::playdirection_begin_forward;
};

class loopsampler {
    public:

    loopsampler(samplermodel<sdloopaudiosample> &samplermodel, polyphonic<audiovoice<AudioPlaySdResmp>> &polyphony) : 
            _samplermodel(samplermodel),
            _polyphonicsampler(
                polyphony,
                [&] (audiovoice<AudioPlaySdResmp> *voice, sdloopaudiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t noteVelocity, bool isNoteOn, bool retrigger) {
                    noteEventCallback(voice, sample, noteNumber, noteChannel, noteVelocity, isNoteOn, retrigger);
                },
                [&] (uint8_t noteNumber, uint8_t noteChannel) -> sdloopaudiosample* { 
                    return _samplermodel.getNoteForChannelAndKey(noteChannel, noteNumber);
                } ) 
    {
    }

    loopsampler(const loopsampler&) = delete;
    virtual ~loopsampler() {
        
    }


 protected:
    samplermodel<sdloopaudiosample> &_samplermodel;
    polyphonicsampler<audiovoice<AudioPlaySdResmp>, sdloopaudiosample> _polyphonicsampler;

    void noteEventCallback(audiovoice<AudioPlaySdResmp>* voice, sdloopaudiosample *sample, uint8_t noteNumber, uint8_t noteChannel, uint8_t velocity, bool isNoteOn, bool retrigger)    
    {
        if (voice != nullptr && sample != nullptr) {
            if (isNoteOn) {                
                if (voice->_audiomixer != nullptr) {                        
                    voice->_audiomixer->gain( voice->_mixerChannel, velocity / 255.0);
                }
                if (voice->_audiomixer2 != nullptr) {                        
                    voice->_audiomixer2->gain(voice->_mixerChannel2, velocity / 255.0);
                }

                play(noteNumber, voice, sample);
            } else 
            {
//                    if (sample->_triggertype == triggertype_play_while_notedown)
                voice->_audioplayarray->stop();
            }
        }
    }


    static void play(uint8_t noteNumber, audiovoice<AudioPlaySdResmp> *voice, sdloopaudiosample *sample) {

        AudioPlaySdResmp &audioplay = *(voice->_audioplayarray);
        switch (sample->_playdirection) {
            case playdirection_begin_forward: audioplay.setPlaybackRate(1.0); break; 
            case playdirection_begin_backward: audioplay.setPlaybackRate(-1.0); break;
        }

        switch (sample->_playlooptype) {
            case playlooptype_once: audioplay.setLoopType(loop_type::looptype_none); break;
            case playlooptype_looping: audioplay.setLoopType(loop_type::looptype_repeat); break;
            case playlooptype_pingpong: audioplay.setLoopType(loop_type::looptype_pingpong); break;
        }

        audioplay.playWav(sample->_filename);
    }


};

#endif