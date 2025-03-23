//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSYAUDIO_LAUNCHCTRL_TEST_TEST_POLYPHONIC
#define TEENSYAUDIO_LAUNCHCTRL_TEST_TEST_POLYPHONIC

#include <boost/test/unit_test.hpp>
#include "polyphonicfixture.h"
#include <Audio.h>
#include "playarrayresmp.h"
#include "sampler.h"
#include "audiovoicepolyphonic.h"
#include "MySampler.h"
#include "output_test.h"

BOOST_AUTO_TEST_SUITE(test_mysampler)

    extern unsigned int kick_raw_len;
    extern unsigned char kick_raw[];
    #define NUM_VOICES 10
    #define KEY_NOTENUMBER_C1 36
    SDClass sd = SDClass("/Users/nicholasnewdigate/Development/sampler");

    BOOST_FIXTURE_TEST_CASE(mysampler_test, PolyphonicFixture) {
        AudioConnection::reset();
        #pragma region audio graph
        // GUItool: begin automatically generated code
        AudioPlaySdResmp        voice0(sd); 
        AudioPlaySdResmp        voice1(sd); 
        AudioPlaySdResmp        voice2(sd); 
        AudioPlaySdResmp        voice3(sd);
        AudioMixer4             mixer_l;
        AudioMixer4             mixer_r;
        TestAudioOutput         testout;       //xy=612,224
        AudioConnection         patchCord1(voice0, 0, mixer_l, 0);
        AudioConnection         patchCord2(voice1, 0, mixer_l, 1);
        AudioConnection         patchCord3(voice2, 0, mixer_l, 2);
        AudioConnection         patchCord4(voice3, 0, mixer_l, 3);
        AudioConnection         patchCord5(voice0, 1, mixer_r, 0);
        AudioConnection         patchCord6(voice1, 1, mixer_r, 1);
        AudioConnection         patchCord7(voice2, 1, mixer_r, 2);
        AudioConnection         patchCord8(voice3, 1, mixer_r, 3);
        AudioConnection         patchCord9(mixer_l, 0, testout, 0);
        AudioConnection         patchCord10(mixer_r, 0, testout, 1);
        // GUItool: end automatically generated code
        #pragma endregion

        audiovoicepolyphonic<AudioPlaySdResmp>   poly;
        samplermodel<sdsampleplayernote>         _samplerModel;
        newdigate::MyLoopSampler                 _sampler(_samplerModel, poly);

        poly.addVoice(&voice0);
        poly.addVoice(&voice1);
        poly.addVoice(&voice2);
        poly.addVoice(&voice3);

        testout.saveOutputFile("../../../output/", "TestMyLoopSampler.wav");

        sdsampleplayernote *sample = new sdsampleplayernote();
        sample->_filename = new char[9] { "HIT1.wav" };
        sample->_samplerNoteNumber = 0;    
        sample->_samplerNoteChannel = 0;    
        sample->isPlaying = false;
        sample->_triggertype = triggertype::triggertype_play_while_notedown;
        sample->_playlooptype = playlooptype::playlooptype_once;
        sample->_playdirection = playdirection::playdirection_begin_forward;
        //KEY_NOTENUMBER_C1, 0, (int16_t *)kick_raw, kick_raw_len / 2,  1);
        _samplerModel.allocateNote(0, 0, sample);

        unsigned progressCallback =
          _sampler
            .registerProgressCallback(
              0,
              0, 
              sample->_filename, 
              128, 
              [] (unsigned index, unsigned sampleNumber) {
                std::cout << index << " : " << sampleNumber << std::endl;
              }
            );

        AudioMemory(20);
        _sampler.trigger( 0, 0, 255, true);

        for (int i=0; i<30; i++) {
          testout.isr();
          _sampler.update();
          _sampler.updateProgress();
        }

        _sampler.trigger( 0, 0, 255, false);

        for (int i=0; i<30; i++) {
          testout.isr();
          _sampler.update();
          _sampler.updateProgress();
        }
        testout.closeOutputfile(2);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSYAUDIO_LAUNCHCTRL_TEST_TEST_POLYPHONIC
