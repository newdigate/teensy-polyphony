#include <Audio.h>
#include "playsdwavresmp.h"
#include "sampler.h"
#include "kick_raw.h"

#define NUM_VOICES 4
// GUItool: begin automatically generated code
AudioPlayArrayResmp      _voices[NUM_VOICES];
AudioMixer4              mixer;
AudioOutputI2S           i2s1;           //xy=675,518
AudioConnection          patchCord1(_voices[0], 0,      mixer, 0);
AudioConnection          patchCord2(_voices[1], 0,      mixer, 1);
AudioConnection          patchCord3(_voices[2], 0,      mixer, 2);
AudioConnection          patchCord4(_voices[3], 0,      mixer, 3);
AudioConnection          patchCord5(mixer,      0,      i2s1,    0);
AudioConnection          patchCord6(mixer,      0,      i2s1,    1);
AudioControlSGTL5000     sgtl5000_1;     //xy=521,588
// GUItool: end automatically generated code

sampler             _sampler;

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