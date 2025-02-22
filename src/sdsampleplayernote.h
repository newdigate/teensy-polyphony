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

#ifndef TEENSY_AUDIO_SAMPLER_SDSAMPLEPLAYERNOTE_H
#define TEENSY_AUDIO_SAMPLER_SDSAMPLEPLAYERNOTE_H

#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include "loopsamplerenums.h"

//template <typename TAudioPlay>
class sdsampleplayernote {
public:
    char * _filename = nullptr;
    uint8_t _samplerNoteNumber = 0;    
    uint8_t _samplerNoteChannel = 0;    
    //TAudioPlay *_voice = nullptr;
    bool isPlaying = false;
    triggertype _triggertype = triggertype::triggertype_play_until_subsequent_notedown;
    playlooptype _playlooptype = playlooptype::playlooptype_once;
    playdirection _playdirection = playdirection::playdirection_begin_forward;

};

#endif // TEENSY_AUDIO_SAMPLER_SDSAMPLEPLAYERNOTE_H