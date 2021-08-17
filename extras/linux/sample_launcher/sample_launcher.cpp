// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example plays a sample stored in an array
#include <Arduino.h>
#include <Audio.h>
#include <MIDI.h>
#include <SD.h>
#include <TeensyVariablePlayback.h>
#include "sampler.h"
#include "RtMidiMIDI.h"
#include "RtMidiTransport.h"
#include "output_soundio.h"
#include "sampleplaymidicontroller.h"

MIDI_CREATE_RTMIDI_INSTANCE(RtMidiMIDI, rtMIDI,  MIDI);

#define NUM_VOICES 4
#define KEY_NOTENUMBER_C1 36

// GUItool: begin automatically generated code
AudioPlaySdResmp         playSdAudio3;     //xy=334.0000457763672,459.0000305175781
AudioPlaySdResmp         playSdAudio2;     //xy=338.0000305175781,382.0000305175781
AudioPlaySdResmp         playSdAudio4;     //xy=339.0000457763672,536
AudioPlaySdResmp         playSdAudio1;     //xy=340.0000419616699,302.0000238418579
AudioMixer4              mixerLeft;         //xy=650.0000419616699,404.0000238418579
AudioMixer4              mixerRight;         //xy=650.0000610351562,511.0000305175781
AudioOutputSoundIO       sio_out1;       //xy=958.0000610351562,466.0000305175781
AudioConnection          patchCord1(playSdAudio3, 0, mixerLeft, 2);
AudioConnection          patchCord2(playSdAudio3, 1, mixerRight, 2);
AudioConnection          patchCord3(playSdAudio2, 0, mixerLeft, 1);
AudioConnection          patchCord4(playSdAudio2, 1, mixerRight, 1);
AudioConnection          patchCord5(playSdAudio4, 0, mixerLeft, 3);
AudioConnection          patchCord6(playSdAudio4, 1, mixerRight, 3);
AudioConnection          patchCord7(playSdAudio1, 0, mixerLeft, 0);
AudioConnection          patchCord8(playSdAudio1, 1, mixerRight, 0);
AudioConnection          patchCord9(mixerLeft, 0, sio_out1, 0);
AudioConnection          patchCord10(mixerRight, 0, sio_out1, 1);
// GUItool: end automatically generated code

sdwavsampler        _sampler;
sdsampleplayermidicontroller _controller(_sampler);

AudioPlaySdResmp           *_voices[NUM_VOICES] = {&playSdAudio1, &playSdAudio2, &playSdAudio3, &playSdAudio4};

void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);
void handleControlChange(byte channel, byte data1, byte data2);

void setup() {
    Serial.begin(9600);
    AudioMemory(20);

    while (!(SD.begin(10))) {
        // stop here if no SD card, but print a message
        Serial.println("Unable to access the SD card...");
        delay(500);
    }


    // Connect the handleNoteOn function to the library,
    // so it is called upon reception of a NoteOn.
    MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

    // Do the same for NoteOffs
    MIDI.setHandleNoteOff(handleNoteOff);

    MIDI.setHandleControlChange(handleControlChange);
    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);

    _sampler.addVoices(_voices, NUM_VOICES);
    
    _controller.begin();
    _controller.initialisation_prompt();
}

void loop() {
    MIDI.read();
    delay(1);
}

int main(int numArgs, char **args) {
    if (numArgs < 2)
    {
        std::cout << "usage: " << args[0] << " <path-to-SDCard>";
        exit(0);
    }
    std::cout << args[1] << std::endl;

    initialize_mock_arduino();
    SD.setSDCardFolderPath(args[1]);
    setup();
    while(true){
        loop();
    }
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    // Do whatever you want when a note is pressed.
    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.
    //byte pitchMapped = pitch + ((channel-1) * 5);
    //_sampler.noteEvent(pitchMapped, velocity, true, false);
    _controller.midiChannleVoiceMessage(0x90, pitch, velocity, channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  _controller.midiChannleVoiceMessage(0x80, pitch, velocity, channel);
}

void handleControlChange(byte channel, byte data1, byte data2) {
  _controller.midiChannleVoiceMessage(0xC0, data1, data2, channel);
}