#include <Arduino.h>
#include <MIDI.h>
#include <Audio.h>
#include <TeensyVariablePlayback.h>
#include <TeensyPolyphony.h>
#include "piano-studio-octave0_raw.h"
#include "piano-studio-octave1_raw.h"
#include "piano-studio-octave2_raw.h"

MIDI_CREATE_DEFAULT_INSTANCE();

#define NUM_VOICES 4
#define KEY_NOTENUMBER_C3 60
#define KEY_NOTENUMBER_C2 48
#define KEY_NOTENUMBER_C1 36

// GUItool: begin automatically generated code
AudioPlayArrayResmp      voice1;         //xy=375,182
AudioPlayArrayResmp      voice2;         //xy=375,221
AudioPlayArrayResmp      voice3;         //xy=375,263
AudioPlayArrayResmp      voice4;         //xy=376,304
AudioPlayArrayResmp      voice5;        //xy=377,357
AudioPlayArrayResmp      voice6;        //xy=377,401
AudioPlayArrayResmp      voice7;        //xy=377,439
AudioPlayArrayResmp      voice8;        //xy=379,477
AudioEffectEnvelope      envelope1;      //xy=520,177
AudioEffectEnvelope      envelope2;      //xy=521,221
AudioEffectEnvelope      envelope3;      //xy=519,263
AudioEffectEnvelope      envelope4;      //xy=522,305
AudioEffectEnvelope      envelope5;      //xy=522,357
AudioEffectEnvelope      envelope6;      //xy=526,401
AudioEffectEnvelope      envelope7;      //xy=526,439
AudioEffectEnvelope      envelope8;      //xy=526,477
AudioMixer4              mixer1;          //xy=736,230
AudioMixer4              mixer2;         //xy=737,395
AudioMixer4              mixer3;         //xy=895,329
AudioOutputI2S           i2s1;           //xy=1082,325
AudioConnection          patchCord1(voice1, envelope1);
AudioConnection          patchCord2(voice2, envelope2);
AudioConnection          patchCord3(voice3, envelope3);
AudioConnection          patchCord4(voice4, envelope4);
AudioConnection          patchCord5(voice5, envelope5);
AudioConnection          patchCord6(voice6, envelope6);
AudioConnection          patchCord7(voice7, envelope7);
AudioConnection          patchCord8(voice8, envelope8);
AudioConnection          patchCord9(envelope3, 0, mixer1, 2);
AudioConnection          patchCord10(envelope1, 0, mixer1, 0);
AudioConnection          patchCord11(envelope2, 0, mixer1, 1);
AudioConnection          patchCord12(envelope4, 0, mixer1, 3);
AudioConnection          patchCord13(envelope5, 0, mixer2, 0);
AudioConnection          patchCord14(envelope6, 0, mixer2, 1);
AudioConnection          patchCord15(envelope7, 0, mixer2, 2);
AudioConnection          patchCord16(envelope8, 0, mixer2, 3);
AudioConnection          patchCord17(mixer1, 0, mixer3, 0);
AudioConnection          patchCord18(mixer2, 0, mixer3, 1);
AudioConnection          patchCord19(mixer3, 0, i2s1, 0);
AudioConnection          patchCord20(mixer3, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=887,463
// GUItool: end automatically generated code

arraysampler             _sampler;
AudioPlayArrayResmp *voices[] = {&voice1, &voice2, &voice3, &voice4, &voice5, &voice6, &voice7, &voice8};
AudioEffectEnvelope *envelopes[] = {&envelope1, &envelope2, &envelope3, &envelope4, &envelope5, &envelope6, &envelope7, &envelope8  };

void handleNoteOn(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    _sampler.noteEvent(pitch, velocity, true, false);
}

void handleNoteOff(uint8_t channel, uint8_t pitch, uint8_t velocity)
{
    _sampler.noteEvent(pitch, velocity, false, false);
}

void setup() {
    for (int i=0; i<8; i++) {
        voices[i]->enableInterpolation(true);
        envelopes[i]->attack(0);
    }

    _sampler.addVoice( voice1, mixer1, 0, envelope1);
    _sampler.addVoice( voice2, mixer1, 1, envelope2);
    _sampler.addVoice( voice3, mixer1, 2, envelope3);
    _sampler.addVoice( voice4, mixer1, 3, envelope4);
    _sampler.addVoice( voice5, mixer2, 0, envelope5);
    _sampler.addVoice( voice6, mixer2, 1, envelope6);
    _sampler.addVoice( voice7, mixer2, 2, envelope7);
    _sampler.addVoice( voice8, mixer2, 3, envelope8);

    _sampler.addSample(KEY_NOTENUMBER_C1, (int16_t *)piano_studio_octave0_raw, piano_studio_octave0_raw_len / 2, 2);
    _sampler.addSample(KEY_NOTENUMBER_C2, (int16_t *)piano_studio_octave1_raw, piano_studio_octave1_raw_len / 2, 2);
    _sampler.addSample(KEY_NOTENUMBER_C3, (int16_t *)piano_studio_octave2_raw, piano_studio_octave2_raw_len / 2, 2);

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
