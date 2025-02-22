//
// Created by Nicholas Newdigate on 18/07/2020.
//

#ifndef TEENSYAUDIO_LAUNCHCTRL_TEST_TEST_POLYPHONIC
#define TEENSYAUDIO_LAUNCHCTRL_TEST_TEST_POLYPHONIC

#include <boost/test/unit_test.hpp>
#include "polyphonicfixture.h"
#include <Audio.h>

BOOST_AUTO_TEST_SUITE(test_polyphony)

    BOOST_FIXTURE_TEST_CASE(basic_test, PolyphonicFixture) {
        AudioPlaySdWav wav1;
        AudioPlaySdWav wav2;
        AudioPlaySdWav wav3;
        AudioPlaySdWav wav4;

        polyphonic<AudioPlaySdWav> poly;
        poly.setCheckForUnsedVoices(false);

        poly.addVoice(wav1);
        poly.addVoice(wav2);
        poly.addVoice(wav3);
        poly.addVoice(wav4);

        AudioPlaySdWav *one = poly.useVoice();
        AudioPlaySdWav *two = poly.useVoice();
        AudioPlaySdWav *three = poly.useVoice();
        AudioPlaySdWav *four = poly.useVoice();
        AudioPlaySdWav *five = poly.useVoice();

        BOOST_CHECK_EQUAL(five, nullptr);

        poly.freeVoice(one);
        five = poly.useVoice();
        BOOST_CHECK_NE(five, nullptr);
    }

BOOST_AUTO_TEST_SUITE_END()

#endif //TEENSYAUDIO_LAUNCHCTRL_TEST_TEST_POLYPHONIC
