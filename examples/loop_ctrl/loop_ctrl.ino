#include <Arduino.h>
#include <MIDI.h>
#include <Audio.h>
#include <TeensyPolyphony.h>
#include <TeensyVariablePlayback.h>
#include "USBHost_t36.h"
#include <ST7735_t3.h>
#include "MySampler.h"

USBHost myusb;
MIDIDevice midi1(myusb);

#define NUM_VOICES 4
// GUItool: begin automatically generated code
AudioPlaySdResmp           playSdWav3;     //xy=340.0000457763672,459.0000305175781
AudioPlaySdResmp           playSdWav2;     //xy=340.0000305175781,382.0000305175781
AudioPlaySdResmp           playSdWav4;     //xy=340.0000457763672,536
AudioPlaySdResmp           playSdWav1;     //xy=340.0000419616699,302.0000238418579
AudioMixer4              mixerLeft;      //xy=650.0000419616699,404.0000238418579
AudioMixer4              mixerRight;     //xy=650.0000610351562,511.0000305175781
AudioOutputTDM           tdm_out;        //xy=958.0000610351562,466.0000305175781
AudioConnection          patchCord1(playSdWav3, 0, mixerLeft, 2);
AudioConnection          patchCord2(playSdWav3, 1, mixerRight, 2);
AudioConnection          patchCord3(playSdWav2, 0, mixerLeft, 1);
AudioConnection          patchCord4(playSdWav2, 1, mixerRight, 1);
AudioConnection          patchCord5(playSdWav4, 0, mixerLeft, 3);
AudioConnection          patchCord6(playSdWav4, 1, mixerRight, 3);
AudioConnection          patchCord7(playSdWav1, 0, mixerLeft, 0);
AudioConnection          patchCord8(playSdWav1, 1, mixerRight, 0);
AudioConnection          patchCord9(mixerLeft, 0, tdm_out, 0);
AudioConnection          patchCord10(mixerRight, 0, tdm_out, 2);
AudioControlCS42448      audioShield;

// GUItool: end automatically generated code
#define TFT_SCLK 13     // SCLK can also use pin 14
#define TFT_MOSI 11     // MOSI can also use pin 7
#define TFT_CS   6      // CS & DC can use pins 2, 6, 9, 10, 15, 20, 21, 22, 23
#define TFT_DC    2     //  but certain pairs must NOT be used: 2+10, 6+9, 20+23, 21+22
#define TFT_RST   -1    // RST can use any pin

ST7735_t3 tft = ST7735_t3(TFT_CS, TFT_DC, TFT_RST);
samplermodel<sdsampleplayernote> model;
audiovoicepolyphonic<AudioPlaySdResmp> polyphony;
newdigate::MyLoopSampler              sampler(model, polyphony);

audiovoice<AudioPlaySdResmp>         *_voices[NUM_VOICES] = {
        new audiovoice<AudioPlaySdResmp>(&playSdWav1),
        new audiovoice<AudioPlaySdResmp>(&playSdWav2),
        new audiovoice<AudioPlaySdResmp>(&playSdWav3),
        new audiovoice<AudioPlaySdResmp>(&playSdWav4)
};

void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);

void setup() {

    Serial.begin(9600);
    AudioMemory(20);
    
    audioShield.enable();
    audioShield.volume(0.5);
    
    while (!(SD.begin(BUILTIN_SDCARD))) {
        // stop here if no SD card, but print a message
        Serial.println("Unable to access the SD card...");
        delay(500);
    }
    tft.initR(INITR_144GREENTAB);
    tft.setRotation(3);
    myusb.begin();

    midi1.setHandleNoteOff(handleNoteOff);
    midi1.setHandleNoteOn(handleNoteOn);

    polyphony.addVoices(_voices, NUM_VOICES);
}

void loop() {
  myusb.Task();
  midi1.read();
}

void handleNoteOn(byte channel, byte pitch, byte velocity) {
    sampler.trigger(pitch, channel, velocity, true);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
    sampler.trigger(pitch, channel, velocity, false);
}