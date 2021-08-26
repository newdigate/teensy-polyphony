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

#ifndef TEENSY_AUDIO_SAMPLER_SAMPLEPLAYMIDICONTROLLERENUMS_H
#define TEENSY_AUDIO_SAMPLER_SAMPLEPLAYMIDICONTROLLERENUMS_H

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
    triggerctrlfunction_selector_cc = 5,
};

#endif // TEENSY_AUDIO_SAMPLER_SAMPLEPLAYMIDICONTROLLERENUMS_H