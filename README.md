# teensy-polyphony
[![teensy40](https://github.com/newdigate/teensy-polyphony/actions/workflows/teensy.yml/badge.svg)](https://github.com/newdigate/teensy-polyphony/actions/workflows/teensy.yml)
[![Ubuntu-x64](https://github.com/newdigate/teensy-polyphony/actions/workflows/ubuntu_x64_cmake.yml/badge.svg)](https://github.com/newdigate/teensy-polyphony/actions/workflows/ubuntu_x64_cmake.yml)

play multi-polyphonic audio samples with teensy audio library 

to compile your firmware using this library, you'll need to adjust your Teensyduino ```boards.txt``` file so that libc++ is linked... 
    ```
    teensy41.build.flags.libs=-larm_cortexM7lfsp_math -lm -lstdc++
    ```

## example usage
``` c++

#include <Arduino.h>
#include <MIDI.h>
#include <Audio.h>
#include "playarrayresmp.h"
#include "sampler.h"
#include "piano-studio-octave0_raw.h"
#include "piano-studio-octave1_raw.h"
#include "piano-studio-octave2_raw.h"

MIDI_CREATE_DEFAULT_INSTANCE();

#define NUM_VOICES 4
#define KEY_NOTENUMBER_C3 60
#define KEY_NOTENUMBER_C2 48
#define KEY_NOTENUMBER_C1 36

// GUItool: begin automatically generated code
AudioPlayArrayResmp      voice0;
AudioPlayArrayResmp      voice1;
AudioPlayArrayResmp      voice2;
AudioPlayArrayResmp      voice3;
AudioMixer4              mixer;
AudioOutputI2S           i2s1;           //xy=675,518
AudioConnection          patchCord1(voice0, 0,      mixer, 0);
AudioConnection          patchCord2(voice1, 0,      mixer, 1);
AudioConnection          patchCord3(voice2, 0,      mixer, 2);
AudioConnection          patchCord4(voice3, 0,      mixer, 3);
AudioConnection          patchCord5(mixer,      0,      i2s1,    0);
AudioConnection          patchCord6(mixer,      0,      i2s1,    1);
AudioControlSGTL5000     sgtl5000_1;     //xy=521,588
// GUItool: end automatically generated code

sampler                 _sampler;
AudioPlayArrayResmp     *_voices[NUM_VOICES] = {&voice0, &voice1, &voice2, &voice3};

void setup() {
    voice0.enableInterpolation(true);
    voice1.enableInterpolation(true);
    voice2.enableInterpolation(true);
    voice3.enableInterpolation(true); 

    _sampler.addVoices(_voices, NUM_VOICES);
    _sampler.addSample(KEY_NOTENUMBER_C1, (int16_t *)piano_studio_octave0_raw, piano_studio_octave0_raw_len / 2);
    _sampler.addSample(KEY_NOTENUMBER_C2, (int16_t *)piano_studio_octave1_raw, piano_studio_octave1_raw_len / 2);
    _sampler.addSample(KEY_NOTENUMBER_C3, (int16_t *)piano_studio_octave2_raw, piano_studio_octave2_raw_len / 2);

    MIDI.setHandleNoteOn(handleNoteOn);  
    MIDI.setHandleNoteOff(handleNoteOff);
    MIDI.begin(MIDI_CHANNEL_OMNI);

    sgtl5000_1.enable();
    sgtl5000_1.volume(0.5);

    AudioMemory(20);
}

void loop() {
    MIDI.read();
}

void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    _sampler.noteEvent(pitch, velocity, true, false);
}

void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    _sampler.noteEvent(pitch, velocity, false, false);
}


```
