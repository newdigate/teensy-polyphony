# teensy-polyphony
[![teensy40](https://github.com/newdigate/teensy-polyphony/actions/workflows/teensy.yml/badge.svg)](https://github.com/newdigate/teensy-polyphony/actions/workflows/teensy.yml)
[![Ubuntu-x64](https://github.com/newdigate/teensy-polyphony/actions/workflows/ubuntu_x64_cmake.yml/badge.svg)](https://github.com/newdigate/teensy-polyphony/actions/workflows/ubuntu_x64_cmake.yml)

play multi-polyphonic audio samples with teensy audio library 

``` c++

#include <Audio.h>
#include "playsdwavresmp.h"
#include "sampler.h"
#include "kick_raw.h"

#define NUM_VOICES 4
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

sampler             _sampler;

AudioPlayArrayResmp      _voices[NUM_VOICES] = {voice0, voice1, voice2, voice3};
void setup() {
    _sampler.addVoices(_voices, NUM_VOICES);
    _sampler.begin((int16_t *)kick_raw, kick_raw_len / 2);
}

void loop() {
    _sampler.noteEvent(60, 128, true, false);
    delay(1000);
    _sampler.noteEvent(72, 128, true, false);
    delay(1000);
    _sampler.noteEvent(60, 128, false, false);
    delay(1000);
    _sampler.noteEvent(72, 128, false, false);
    delay(1000);
}

```
