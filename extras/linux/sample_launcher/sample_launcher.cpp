// Plays a RAW (16-bit signed) PCM audio file at slower or faster rate
// this example plays a sample stored in an array
#include <Arduino.h>
#include <Audio.h>
#include <MIDI.h>
#include <SD.h>
#include "sampler.h"
#include "RtMidiMIDI.h"
#include "RtMidiTransport.h"
#include "output_soundio.h"

MIDI_CREATE_RTMIDI_INSTANCE(RtMidiMIDI, rtMIDI,  MIDI);

#define NUM_VOICES 4
#define KEY_NOTENUMBER_C1 36

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav3;     //xy=334.0000457763672,459.0000305175781
AudioPlaySdWav           playSdWav2;     //xy=338.0000305175781,382.0000305175781
AudioPlaySdWav           playSdWav4;     //xy=339.0000457763672,536
AudioPlaySdWav           playSdWav1;     //xy=340.0000419616699,302.0000238418579
AudioMixer4              mixerLeft;         //xy=650.0000419616699,404.0000238418579
AudioMixer4              mixerRight;         //xy=650.0000610351562,511.0000305175781
AudioOutputSoundIO       sio_out1;       //xy=958.0000610351562,466.0000305175781
AudioConnection          patchCord1(playSdWav3, 0, mixerLeft, 2);
AudioConnection          patchCord2(playSdWav3, 1, mixerRight, 2);
AudioConnection          patchCord3(playSdWav2, 0, mixerLeft, 1);
AudioConnection          patchCord4(playSdWav2, 1, mixerRight, 1);
AudioConnection          patchCord5(playSdWav4, 0, mixerLeft, 3);
AudioConnection          patchCord6(playSdWav4, 1, mixerRight, 3);
AudioConnection          patchCord7(playSdWav1, 0, mixerLeft, 0);
AudioConnection          patchCord8(playSdWav1, 1, mixerRight, 0);
AudioConnection          patchCord9(mixerLeft, 0, sio_out1, 0);
AudioConnection          patchCord10(mixerRight, 0, sio_out1, 1);
// GUItool: end automatically generated code

unpitchedsdwavsampler    _sampler;

AudioPlaySdWav           *_voices[NUM_VOICES] = {&playSdWav1, &playSdWav2, &playSdWav3, &playSdWav4};

uint16_t getNumWavFilesInDirectory(char *directory);
void populateFilenames(char *directory);
void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);

int _numWaveFiles = 0;
char **_filenames;
void setup() {
    Serial.begin(9600);
    AudioMemory(20);

    while (!(SD.begin(10))) {
        // stop here if no SD card, but print a message
        Serial.println("Unable to access the SD card...");
        delay(500);
    }

    _numWaveFiles = getNumWavFilesInDirectory("/");
    Serial.printf("Num wave files: %d\n", _numWaveFiles);
    _filenames = new char*[_numWaveFiles];
    populateFilenames("/");

    // Connect the handleNoteOn function to the library,
    // so it is called upon reception of a NoteOn.
    MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

    // Do the same for NoteOffs
    MIDI.setHandleNoteOff(handleNoteOff);

    // Initiate MIDI communications, listen to all channels
    MIDI.begin(MIDI_CHANNEL_OMNI);

    _sampler.addVoices(_voices, NUM_VOICES);
    for (int i=0; i < _numWaveFiles; i++) {
        _sampler.addSample(53+i, _filenames[i]);
    }
    
    Serial.println("setup done");
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
    byte pitchMapped = pitch + ((channel-1) * 5);
    _sampler.noteEvent(pitchMapped, velocity, true, false);
    Serial.printf("Its alive...ch:%d pitch:%d vel:%d\n", channel, pitch, velocity);
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    // Do something when the note is released.
    // Note that NoteOn messages with 0 velocity are interpreted as NoteOffs.
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