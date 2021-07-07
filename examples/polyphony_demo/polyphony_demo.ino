#include <Arduino.h>
#include <MIDI.h>
#include <Audio.h>
#include "playsdwavresmp.h"
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
AudioPlayArrayResmp      voice0;         //xy=339,43
AudioPlayArrayResmp      voice1;         //xy=339,82
AudioPlayArrayResmp      voice2;         //xy=339,124
AudioPlayArrayResmp      voice3;         //xy=339,165
AudioMixer4              mixer;          //xy=590,98
AudioOutputI2S           i2s1;           //xy=818,94
AudioConnection          patchCord1(voice0, 0, mixer, 0);
AudioConnection          patchCord2(voice1, 0, mixer, 1);
AudioConnection          patchCord3(voice2, 0, mixer, 2);
AudioConnection          patchCord4(voice3, 0, mixer, 3);
AudioConnection          patchCord5(mixer, 0, i2s1, 0);
AudioConnection          patchCord6(mixer, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=588,162
// GUItool: end automatically generated code

sampler             _sampler;

AudioPlayArrayResmp      *_voices[NUM_VOICES] = {&voice0, &voice1, &voice2, &voice3};

void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    // Do whatever you want when a note is pressed.

    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.
    _sampler.noteEvent(pitch, velocity, true, false);
}

void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
    _sampler.noteEvent(pitch, velocity, false, false);
}

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
