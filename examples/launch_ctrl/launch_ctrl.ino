#include <Arduino.h>
#include <MIDI.h>
#include <Audio.h>
#include <TeensyPolyphony.h>
#include "USBHost_t36.h"
#include "sdsampleplayernote.h"
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
polyphonic<audiovoice<AudioPlaySdResmp>> polyphony;
samplermodel<audiosample> samplerModel;
sdsampler    sampler(samplerModel, polyphony);
audiovoice<AudioPlaySdResmp>           *_voices[NUM_VOICES] = {
        new audiovoice<AudioPlaySdResmp>(&playSdWav1),
        new audiovoice<AudioPlaySdResmp>(&playSdWav2),
        new audiovoice<AudioPlaySdResmp>(&playSdWav3),
        new audiovoice<AudioPlaySdResmp>(&playSdWav4)
};

uint16_t getNumWavFilesInDirectory(char *directory);
void populateFilenames(char *directory);
void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);

int _numWaveFiles = 0;
char **_filenames;

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

    _numWaveFiles = getNumWavFilesInDirectory("/");
    Serial.printf("Num wave files: %d\n", _numWaveFiles);
    _filenames = new char*[_numWaveFiles];
    populateFilenames("/");
    myusb.begin();

    midi1.setHandleNoteOff(handleNoteOff);
    midi1.setHandleNoteOn(handleNoteOn);
    //midi1.setHandleControlChange(OnControlChange);

    polyphony.addVoices(_voices, NUM_VOICES);
    for (int i=0; i < _numWaveFiles; i++) {
        samplerModel.allocateNote(0, 53+i, new audiosample(53,0, _filenames[i], 1));
    }
    
    Serial.println("setup done");
}

void loop() {
  myusb.Task();
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    // Do whatever you want when a note is pressed.
    // Try to keep your callbacks short (no delays ect)
    // otherwise it would slow down the loop() and have a bad impact
    // on real-time performance.
    byte pitchMapped = pitch + ((channel-1) * 5);
    sampler.trigger(pitchMapped, 0, velocity, true);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
    byte pitchMapped = pitch + ((channel-1) * 5);
    sampler.trigger(pitchMapped, 0, velocity, false);
}

uint16_t getNumWavFilesInDirectory(char *directory) {
  File dir = SD.open(directory);
  uint16_t numWaveFiles = 0;

  while (true) { 

    File files =  dir.openNextFile();
    if (!files) {
      //If no more files, break out.
      break;
    }

    String curfile = files.name(); //put file in string
    
    int m = curfile.lastIndexOf(".WAV");
    int a = curfile.lastIndexOf(".wav");
    int underscore = curfile.indexOf("_");

    // if returned results is more then 0 add them to the list.
    if ((m > 0 || a > 0) && (underscore != 0)) {  
        numWaveFiles++;
    }
    
    files.close();
  }
  // close 
  dir.close();
  return numWaveFiles;
}

void populateFilenames(char *directory) {
  File dir = SD.open(directory);
  uint16_t index = 0;

  while (true) { 

    File files =  dir.openNextFile();
    if (!files) {
      //If no more files, break out.
      break;
    }

    String curfile = files.name(); //put file in string
    
    int m = curfile.lastIndexOf(".WAV");
    int a = curfile.lastIndexOf(".wav");
    int underscore = curfile.indexOf("_");

    // if returned results is more then 0 add them to the list.
    if ((m > 0 || a > 0) && (underscore != 0)) {  
        _filenames[index] = new char[curfile.length()+1] {0};
        memcpy(_filenames[index], curfile.c_str(), curfile.length());
        index++;
    } 
    files.close();
  }
  // close 
  dir.close();
}
